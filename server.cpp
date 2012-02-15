#include "server.h"
#include "constants.h"

#define MAX_CONNECTIONS_COUNT 10

Server::Server() : _isGameStarted(false)
{
    _tcpServer = new QTcpServer(this);
    connect(_tcpServer, SIGNAL(newConnection()), this, SLOT(processNewConnection()));

    _listener = new QUdpSocket(this);
    _messageReceiver = new UdpMessageReceiver(_listener, this);
    connect(_messageReceiver, SIGNAL(serverRequestMessageReceived(ServerRequestMessage, QHostAddress, quint16)), this, SLOT(receiveServerRequestMessage(ServerRequestMessage, QHostAddress, quint16)));

    _timer = new QTimer(this);
    _timer->setInterval(PING_INTERVAL);
    connect(_timer, SIGNAL(timeout()), this, SLOT(ping()));

    connect(this, SIGNAL(destroyed()), this, SLOT(stop()));
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

int Server::currentPlayersCount() const
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

void Server::start(int playersCount, quint16 port)
{
    _mutex.lock();
    bool listening = _tcpServer->isListening();
    if (!listening)
    {
        _playersCount = playersCount;
        listening = _tcpServer->listen(QHostAddress::Any, port);
        if (listening)
        {
            _timer->start();
            _listener->bind(QHostAddress::Any, port);
        }
    }

    _mutex.unlock();
    emit started();
}

void Server::waitForStart(int msec)
{
    if (_mutex.tryLock(msec))
    {
        _mutex.unlock();
    }
}

void Server::stop()
{
    if (_tcpServer->isListening())
    {
        _isGameStarted = false;
        _timer->stop();
        _listener->disconnectFromHost();
        for (int i = 0; i < _clients.size(); ++i)
        {
            _clients[i]->setDisconnectedFromGame();
            _clients[i]->deleteLater();
        }

        _clients.clear();
        _tcpServer->close();
    }
}

void Server::startGame(RemoteClient *client)
{
    _isGameStarted = true;
    if (_tcpServer->isListening())
    {
        if (currentPlayersCount() != _playersCount)
        {
            ErrorMessage msg1(ERROR_WAIT_FOR_OTHER);
            client->sendMessage(msg1);
            return;
        }

        RemoteClient *first = NULL;
        for (int i = 0; i < _clients.size(); ++i)
        {
            RemoteClient *current = _clients[i];
            if (current->isConnectedToGame())
            {
                first = current;
                break;
            }
        }

        if (first == NULL)
        {
            qDebug() << "Something wrong";
            return;
        }

        if (client != first)
        {
            ErrorMessage msg1(ERROR_YOU_ARE_NOT_FIRST);
            client->sendMessage(msg1);
            return;
        }

        StartGameMessage msg(clients());
        sendToAll(msg);
    }
}

void Server::stopGame(RemoteClient *)
{
    _isGameStarted = false;
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
    ClientDisconnectMessage msg(client->info());
    sendToAll(msg);
    sendPlayersList();
}

void Server::processClientError(RemoteClient *)
{
}

void Server::receiveChatMessage(const ChatMessage &msg, RemoteClient *)
{
    sendToAll(msg);
}

void Server::receiveSurrenderMessage(const SurrenderMessage &msg, RemoteClient *)
{
    sendToAll(msg);
}

void Server::receiveTryToConnectMessage(const TryToConnectMessage &msg, RemoteClient *client)
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
    else if (currentPlayersCount() == _playersCount)
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

    if (error)
    {
        ErrorMessage msg1(error);
        client->sendMessage(msg1);
        client->setDisconnectedFromGame();
    }
    else
    {
        connect(client, SIGNAL(chatMessageReceived(ChatMessage, RemoteClient *)), this, SLOT(receiveChatMessage(ChatMessage, RemoteClient *)));
        connect(client, SIGNAL(disconnected(RemoteClient *)), this, SLOT(processClientDisconnected(RemoteClient *)));
        connect(client, SIGNAL(errorOccurred(RemoteClient *)), this, SLOT(processClientError(RemoteClient *)));
        connect(client, SIGNAL(startGameMessageReceived(RemoteClient *)), this, SLOT(startGame(RemoteClient *)));
        connect(client, SIGNAL(stopGameMessageReceived(RemoteClient *)), this, SLOT(stopGame(RemoteClient *)));
        connect(client, SIGNAL(surrenderMessageReceived(SurrenderMessage, RemoteClient *)), this, SLOT(receiveSurrenderMessage(SurrenderMessage, RemoteClient *)));
        connect(client, SIGNAL(turnMessageReceived(TurnMessage, RemoteClient *)), this, SLOT(receiveTurnMessage(TurnMessage, RemoteClient *)));
        client->setConnectedToGame(msg.info());
        ConnectionAcceptedMessage msg1;
        client->sendMessage(msg1);
        ClientConnectMessage msg2(msg.info());
        sendToAll(msg2);
        sendPlayersList();
    }
}

void Server::receiveTurnMessage(const TurnMessage &msg, RemoteClient *)
{
    sendToAll(msg);
}

void Server::processNewConnection()
{
    if (_tcpServer->hasPendingConnections())
    {
        QTcpSocket *s = _tcpServer->nextPendingConnection();
        if (_clients.size() >= MAX_CONNECTIONS_COUNT)
        {
            s->disconnectFromHost();
            s->deleteLater();
            return;
        }

        RemoteClient *client = new RemoteClient(s);
        connect(client, SIGNAL(disconnected(RemoteClient *)), this, SLOT(removeClient(RemoteClient *)));
        connect(client, SIGNAL(tryToConnectMessageReceived(TryToConnectMessage, RemoteClient *)), this, SLOT(receiveTryToConnectMessage(TryToConnectMessage, RemoteClient *)));
        _clients.append(client);
        ServerReadyMessage msg;
        msg.send(s);
    }
}

void Server::receiveServerRequestMessage(const ServerRequestMessage &, const QHostAddress &host, quint16 port)
{
    ServerInfoMessage msg(clients());
    msg.send(_listener, host, port);
}
