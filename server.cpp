#include "server.h"
#include "constants.h"

Server::Server() : _isGameStarted(false)
{
    connect(&_tcpServer, SIGNAL(newConnection()), this, SLOT(newConnection()));

    _messageReceiver = new UdpMessageReceiver(&_listener);
    connect(_messageReceiver, SIGNAL(serverRequestMessageReceive(ServerRequestMessage, const QHostAddress &, quint16)), this, SLOT(serverRequestMessageReceive(ServerRequestMessage, const QHostAddress &, quint16)));
    connect(this, SIGNAL(destroyed()), _messageReceiver, SLOT(deleteLater()));

    _timer.setInterval(PING_INTERVAL);
    connect(&_timer, SIGNAL(timeout()), this, SLOT(ping()));
    connect(this, SIGNAL(finished()), &_timer, SLOT(stop()));
    connect(this, SIGNAL(terminated()), &_timer, SLOT(stop()));

    connect(this, SIGNAL(finished()), this, SLOT(stop()));
    connect(this, SIGNAL(terminated()), this, SLOT(stop()));
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
    for(int i = 0; i < _clients.size(); ++i)
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

void Server::startGame()
{
    _isGameStarted = true;
    StartGameMessage msg;
    sendToAll(msg);
}

void Server::restartGame(QList<ClientInfo> list)
{
    _isGameStarted = true;
    RestartGameMessage msg(list);
    sendToAll(msg);
}

void Server::gameOver()
{
    _isGameStarted = false;
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

void Server::stop()
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

void Server::remoteChatMessageReceive(ChatMessage msg, RemoteClient *)
{
    sendToAll(msg);
}

void Server::remoteDisconnected(RemoteClient *client)
{
    ClientDisconnectMessage msg(client->name(), client->color());
    sendToAll(msg);
    sendPlayersList();
}

void Server::remoteError(RemoteClient *)
{
}

void Server::remoteSurrenderMessageReceive(SurrenderMessage msg, RemoteClient *)
{
    sendToAll(msg);
}

void Server::remoteTurnMessageReceive(TurnMessage msg, RemoteClient *)
{
    sendToAll(msg);
}

void Server::remoteTryToConnectMessageReceive(TryToConnectMessage msg, RemoteClient *client)
{
    int i, error = 0;
    if (_isGameStarted)
    {
        error = 3;
    }
    else
    {
        for (i = 0; i < _clients.size()
            && (msg.name() != _clients[i]->name() || !_clients[i]->isConnectedToGame()); ++i) { }
        if (i != _clients.size())
        {
            error = 2;
        }
        else
        {
            for (i = 0; i < _clients.size()
                && (msg.color() != _clients[i]->color() || !_clients[i]->isConnectedToGame()); ++i) { }
            if (i != _clients.size())
            {
                error = 1;
            }
            else
            {
                if (playersCount() == _maxClientsCount)
                {
                    error = 4;
                }
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
        connect(client, SIGNAL(rcChatMessageReceive(ChatMessage, RemoteClient *)), this, SLOT(remoteChatMessageReceive(ChatMessage, RemoteClient *)));
        connect(client, SIGNAL(rcDisconnected(RemoteClient *)), this, SLOT(remoteDisconnected(RemoteClient *)));
        connect(client, SIGNAL(rcSurrenderMessageReceive(SurrenderMessage, RemoteClient *)), this, SLOT(remoteSurrenderMessageReceive(SurrenderMessage, RemoteClient *)));
        connect(client, SIGNAL(rcTurnMessageReceive(TurnMessage, RemoteClient *)), this, SLOT(remoteTurnMessageReceive(TurnMessage, RemoteClient *)));
        client->setConnectedToGame(msg.name(), msg.color());
        ClientConnectMessage msg1(msg.name(), msg.color());
        sendToAll(msg1);
        sendPlayersList();
    }
}

void Server::newConnection()
{
    if (_tcpServer.hasPendingConnections())
    {
        QTcpSocket *s = _tcpServer.nextPendingConnection();
        RemoteClient *client = new RemoteClient(s);
        connect(client, SIGNAL(rcDisconnected(RemoteClient *)), this, SLOT(removeClient(RemoteClient *)));
        connect(client, SIGNAL(rcTryToConnectMessageReceive(TryToConnectMessage, RemoteClient *)), this, SLOT(remoteTryToConnectMessageReceive(TryToConnectMessage, RemoteClient *)));
        connect(this, SIGNAL(finished()), client, SLOT(setDisconnectedFromGame()));
        connect(this, SIGNAL(finished()), client, SLOT(deleteLater()));
        connect(this, SIGNAL(terminated()), client, SLOT(setDisconnectedFromGame()));
        connect(this, SIGNAL(terminated()), client, SLOT(deleteLater()));
        _clients.append(client);
        ServerReadyMessage msg;
        msg.send(s);
    }
}

void Server::serverRequestMessageReceive(ServerRequestMessage, const QHostAddress &host, quint16 port)
{
    ServerInfoMessage msg(clients());
    msg.send(&_listener, host, port);
}
