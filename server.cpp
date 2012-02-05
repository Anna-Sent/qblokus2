#include "server.h"
#include "constants.h"

#define MAX_CONNECTIONS_COUNT   10

Server::Server() : _isGameStarted(false)
{
    connect(&_tcpServer, SIGNAL(newConnection()), this, SLOT(processNewConnection()));

    _messageReceiver = new UdpMessageReceiver(&_listener);
    connect(_messageReceiver, SIGNAL(serverRequestMessageReceived(ServerRequestMessage, const QHostAddress &, quint16)), this, SLOT(receiveServerRequestMessage(ServerRequestMessage, const QHostAddress &, quint16)));
    connect(this, SIGNAL(destroyed()), _messageReceiver, SLOT(deleteLater()));

    _timer.setInterval(PING_INTERVAL);
    connect(&_timer, SIGNAL(timeout()), this, SLOT(ping()));

    connect(this, SIGNAL(finished()), &_timer, SLOT(stop()));
    connect(this, SIGNAL(finished()), this, SLOT(stopGame()));
    connect(this, SIGNAL(finished()), this, SLOT(clear()));
    connect(this, SIGNAL(terminated()), &_timer, SLOT(stop()));
    connect(this, SIGNAL(terminated()), this, SLOT(stopGame()));
    connect(this, SIGNAL(terminated()), this, SLOT(clear()));
}

QList<ClientInfo> Server::clients() const
{
    QList<ClientInfo> list;
    for (int i = 0; i < _clients.size(); ++i)
    {
        if (_clients[i]->isConnectedToGame())
        {
            list.append(_clients[i]->info());
        }
    }

    return list;
}

int Server::playersCount() const
{
    int count = 0;
    for (int i = 0; i < _clients.size(); ++i)
    {
        if (_clients[i]->isConnectedToGame())
        {
            ++count;
        }
    }

    return count;
}

void Server::ping()
{
    for (int i = 0; i < _clients.size(); ++i)
    {
        int elapsed = _clients[i]->elapsedSinceLastPing();
        if (elapsed > PING_TIME)
        {
            _clients[i]->setDisconnectedFromGame();
        }
        else
        {
            PingMessage msg;
            _clients[i]->sendMessage(msg);
        }
    }
}

void Server::sendPlayersList()
{
    PlayersListMessage msg(clients());
    sendToAll(msg);
}

void Server::sendToAll(const Message &msg)
{
    QList<RemoteClient *>::iterator i;
    for (i = _clients.begin(); i != _clients.end(); ++i)
    {
        if ((*i)->isConnectedToGame())
        {
            (*i)->sendMessage(msg);
        }
    }
}


void Server::start(int maxClientsCount, quint16 port)
{
    if (!isRunning())
    {
        _maxClientsCount = maxClientsCount;
        bool listening = _tcpServer.listen(QHostAddress::Any, port);
        if (listening)
        {
            _timer.start();
            _listener.bind(QHostAddress::Any, port);
            QThread::start();
        }
    }
}

void Server::startGame(QList<ClientInfo> list)
{
    _isGameStarted = true;
    StartGameMessage msg(list);
    sendToAll(msg);
}

void Server::stopGame()
{
    _isGameStarted = false;
}

void Server::stop()
{
    if (isRunning())
    {
        quit();
    }
}

void Server::clear()
{
    _clients.clear();
    _tcpServer.close();
    _listener.disconnectFromHost();
}

void Server::removeClient(RemoteClient *client)
{
    int index = _clients.indexOf(client);
    if (index == -1)
    {
        qDebug() << "Client " << client->name() << " is already removed from list";
    }
    else
    {
        _clients.removeAt(index);
    }

    client->deleteLater();
}

void Server::processClientDisconnected(RemoteClient *client)
{
    ClientDisconnectMessage msg(client->name(), client->color());
    sendToAll(msg);
    sendPlayersList();
}

void Server::processClientError(RemoteClient *)
{
}

void Server::receiveChatMessage(ChatMessage msg, RemoteClient *)
{
    sendToAll(msg);
}

void Server::receiveSurrenderMessage(SurrenderMessage msg, RemoteClient *)
{
    sendToAll(msg);
}

void Server::receiveTryToConnectMessage(TryToConnectMessage msg, RemoteClient *client)
{
    int i, error = 0;
    if (_clients.size() >= MAX_CONNECTIONS_COUNT)
    {
        error = ERROR_MAX_CONNECTIONS_NUM;
    }
    else if (_isGameStarted)
    {
        error = ERROR_GAME_STARTED;
    }
    else if (playersCount() == _maxClientsCount)
    {
        error = ERROR_MAX_PLAYERS_NUM;
    }
    else
    {
        for (i = 0; i < _clients.size()
            && (msg.name() != _clients[i]->name() || !_clients[i]->isConnectedToGame()); ++i) { }
        if (i != _clients.size())
        {
            error = ERROR_NAME_IN_USE;
        }
        else
        {
            for (i = 0; i < _clients.size()
                && (msg.color() != _clients[i]->color() || !_clients[i]->isConnectedToGame()); ++i) { }
            if (i != _clients.size())
            {
                error = ERROR_COLOR_IN_USE;
            }
        }
    }

    ConnectionAcceptedMessage msg1(error);
    client->sendMessage(msg1);
    if (error)
    {
        client->setDisconnectedFromGame();
    }
    else
    {
        connect(client, SIGNAL(chatMessageReceived(ChatMessage, RemoteClient *)), this, SLOT(receiveChatMessage(ChatMessage, RemoteClient *)));
        connect(client, SIGNAL(disconnected(RemoteClient *)), this, SLOT(processClientDisconnected(RemoteClient *)));
        connect(client, SIGNAL(errorOccurred(RemoteClient *)), this, SLOT(processClientError(RemoteClient *)));
        connect(client, SIGNAL(surrenderMessageReceived(SurrenderMessage, RemoteClient *)), this, SLOT(receiveSurrenderMessage(SurrenderMessage, RemoteClient *)));
        connect(client, SIGNAL(turnMessageReceived(TurnMessage, RemoteClient *)), this, SLOT(receiveTurnMessage(TurnMessage, RemoteClient *)));
        client->setConnectedToGame(msg.name(), msg.color());
        ClientConnectMessage msg1(msg.name(), msg.color());
        sendToAll(msg1);
        sendPlayersList();
    }
}

void Server::receiveTurnMessage(TurnMessage msg, RemoteClient *)
{
    sendToAll(msg);
}

void Server::processNewConnection()
{
    if (_tcpServer.hasPendingConnections())
    {
        QTcpSocket *s = _tcpServer.nextPendingConnection();
        if (_clients.size() >= MAX_CONNECTIONS_COUNT)
        {
            s->disconnectFromHost();
            s->deleteLater();
            return;
        }

        RemoteClient *client = new RemoteClient(s);
        connect(client, SIGNAL(disconnected(RemoteClient *)), this, SLOT(removeClient(RemoteClient *)));
        connect(client, SIGNAL(tryToConnectMessageReceived(TryToConnectMessage, RemoteClient *)), this, SLOT(receiveTryToConnectMessage(TryToConnectMessage, RemoteClient *)));
        connect(this, SIGNAL(finished()), client, SLOT(setDisconnectedFromGame()));
        connect(this, SIGNAL(finished()), client, SLOT(deleteLater()));
        connect(this, SIGNAL(terminated()), client, SLOT(setDisconnectedFromGame()));
        connect(this, SIGNAL(terminated()), client, SLOT(deleteLater()));
        _clients.append(client);
        ServerReadyMessage msg;
        msg.send(s);
    }
}

void Server::receiveServerRequestMessage(ServerRequestMessage, const QHostAddress &host, quint16 port)
{
    ServerInfoMessage msg(clients());
    msg.send(&_listener, host, port);
}
