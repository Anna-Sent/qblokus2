#include "server.h"
#include "constants.h"

#define PING_INTERVAL 5000
#define PING_TIME 1500000

Server::Server()
{
    connect(&_tcpServer, SIGNAL(newConnection()), this, SLOT(newConnection()));

    _messageReceiver = new UdpMessageReceiver(&_listener);
    connect(_messageReceiver, SIGNAL(serverRequestMessageReceive(ServerRequestMessage, const QHostAddress &, quint16)), this, SLOT(serverRequestMessageReceive(ServerRequestMessage, const QHostAddress &, quint16)));

    _timer.setInterval(PING_INTERVAL);
    connect(&_timer, SIGNAL(timeout()), this, SLOT(ping()));
}

Server::~Server()
{
    _messageReceiver->deleteLater();
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
        PingMessage msg;
        msg.send(_clients[i]->socket);
        QTime last = _clients[i]->lastpingtime;
        int elapsed = last.elapsed();
        if (elapsed > PING_TIME)
        {
            _clients[i]->socket->close();
        }
    }
}

void Server::run()
{
    exec();
    stop();
}

void Server::sendPlayersList()
{
    QList<ClientInfo> list;
    for (int i = 0; i < _clients.size(); ++i)
    {
        if (_clients[i]->isConnectedToGame())
        {
            list.append(_clients[i]->info());
        }
    }

    PlayersListMessage msg(list);
    sendToAll(&msg);
}

void Server::sendToAll(Message *msg)
{
    QList<RemoteClient *>::iterator i;
    for (i = _clients.begin(); i != _clients.end(); ++i)
    {
        if ((*i)->isConnectedToGame())
        {
            msg->send((*i)->socket);
        }
    }
}

void Server::startGame()
{
    StartGameMessage msg;
    sendToAll(&msg);
}

void Server::restartGame(QList<ClientInfo> list)
{
    RestartGameMessage msg(list);
    sendToAll(&msg);
}

bool Server::start(int maxClientsCount, quint16 port)
{
    _maxClientsCount = maxClientsCount;
    bool listening = _tcpServer.listen(QHostAddress::Any, port);
    if (listening)
    {
        _timer.start();
        _listener.bind(QHostAddress::Any, port);
        QThread::start();
    }

    return listening;
}

void Server::stop()
{
    if (_tcpServer.isListening())
    {
        _tcpServer.close();
        _listener.close();
        _timer.stop();
        while (_clients.size()>0)
        {
            removeClient(_clients[0]);
        }

        _clients.clear();
    }
}

void Server::removeClient(RemoteClient *client)
{
    client->socket->close();
    _clients.removeAt(_clients.indexOf(client));
    client->deleteLater();
}

void Server::remoteChatMessageReceive(ChatMessage msg, RemoteClient *)
{
    sendToAll(&msg);
}

void Server::remoteDisconnected(RemoteClient *client)
{
    if (client->isConnectedToGame())
    {
        ClientDisconnectMessage msg(client->name(), client->color());
        sendToAll(&msg);
        sendPlayersList();
    }

    removeClient(client);
}

void Server::remoteError(RemoteClient *client)
{
    client->socket->close();
}

void Server::remotePingMessageReceive(PingMessage, RemoteClient *client)
{
    client->lastpingtime.start();
}

void Server::remoteSurrenderMessageReceive(SurrenderMessage msg, RemoteClient *)
{
    sendToAll(&msg);
}

void Server::remoteTurnMessageReceive(TurnMessage msg, RemoteClient *)
{
    sendToAll(&msg);
}

void Server::remoteTryToConnectMessageReceive(TryToConnectMessage msg, RemoteClient *client)
{
    int i, error = 0;
    for (i = 0; i < _clients.size()
        && (msg.color() != _clients[i]->color() || !_clients[i]->isConnectedToGame()); ++i) { }

    if (i != _clients.size())
    {
        error = 1;
    }

    for (i = 0; i < _clients.size()
        && (msg.name() != _clients[i]->name() || !_clients[i]->isConnectedToGame()); ++i) { }

    if (i != _clients.size())
    {
        error = 2;
    }

    if (playersCount() == _maxClientsCount)
    {
        error = 4;
    }

    ConnectionAcceptedMessage msg1(error);
    msg1.send(client->socket);
    if (!error)
    {
        client->setConnectedToGame(msg.name(), msg.color());
        ClientConnectMessage msg1(msg.name(), msg.color());
        sendToAll(&msg1);
        sendPlayersList();
    }
    else
    {
        removeClient(client);
    }
}

void Server::newConnection()
{
    if (_tcpServer.hasPendingConnections())
    {
        QTcpSocket *s = _tcpServer.nextPendingConnection();
        RemoteClient *client = new RemoteClient(s);
        connect(client, SIGNAL(rcChatMessageReceive(ChatMessage, RemoteClient *)), this, SLOT(remoteChatMessageReceive(ChatMessage, RemoteClient *)));
        connect(client, SIGNAL(rcTryToConnectMessageReceive(TryToConnectMessage, RemoteClient *)), this, SLOT(remoteTryToConnectMessageReceive(TryToConnectMessage, RemoteClient *)));
        connect(client, SIGNAL(rcPingMessageReceive(PingMessage, RemoteClient *)), this, SLOT(remotePingMessageReceive(PingMessage, RemoteClient *)));
        connect(client, SIGNAL(rcTurnMessageReceive(TurnMessage, RemoteClient *)), this, SLOT(remoteTurnMessageReceive(TurnMessage, RemoteClient *)));
        connect(client, SIGNAL(rcSurrenderMessageReceive(SurrenderMessage, RemoteClient *)), this, SLOT(remoteSurrenderMessageReceive(SurrenderMessage, RemoteClient *)));
        connect(client, SIGNAL(rcDisconnected(RemoteClient *)), this, SLOT(remoteDisconnected(RemoteClient *)));
        connect(client, SIGNAL(rcError(RemoteClient *)), this, SLOT(remoteError(RemoteClient *)));
        _clients.append(client);
        ServerReadyMessage msg;
        msg.send(s);
    }
}

void Server::serverRequestMessageReceive(ServerRequestMessage, const QHostAddress &host, quint16 port)
{
    QList<ClientInfo> list;
    for (int i = 0; i < _clients.size(); ++i)
    {
        if (_clients[i]->isConnectedToGame())
        {
            list.append(_clients[i]->info());
        }
    }

    ServerInfoMessage msg(list);
    msg.send(&_listener, host, port);
}
