#include "server.h"
#include "constants.h"

#define PING_INTERVAL 5000
#define PING_TIME 1500000

QList<ClientInfo> Server::getClients() const {
    QList<ClientInfo> list;
    for (int i=0;i<clients.size();++i)
        if (clients[i]->state==2)
            list.append(clients[i]->info);
    return list;
}

void Server::serverRequestMessageReceive(ServerRequestMessage, const QHostAddress &host, quint16 port)
{
    QList<ClientInfo> list;
    for (int i = 0; i < clients.size(); ++i)
    {
        if (clients[i]->state == 2)
        {
            list.append(clients[i]->info);
        }
    }

    ServerInfoMessage msg(list);
    msg.send(&listener, host, port);
}

void Server::remotePingMessageReceive(PingMessage, RemoteClient* client) {
    client->lastpingtime.start();
}

void Server::removeClient(RemoteClient* client) {
    client->socket->close();
    clients.removeAt(clients.indexOf(client));
    client->deleteLater();
}

Server::Server() {
    connect(&serverConnection, SIGNAL(newConnection()), this, SLOT(newConnection()));

    messageReceiver = new UdpMessageReceiver(&listener);
    connect(messageReceiver, SIGNAL(serverRequestMessageReceive(ServerRequestMessage, const QHostAddress &, quint16)), this, SLOT(serverRequestMessageReceive(ServerRequestMessage, const QHostAddress &, quint16)));

    timer.setInterval(PING_INTERVAL);
    connect(&timer, SIGNAL(timeout()), this, SLOT(ping()));
}

Server::~Server()
{
    delete messageReceiver;
}

bool Server::start(int maxClientsCount, quint16 port) {
    this->maxClientsCount = maxClientsCount;
    bool listening = serverConnection.listen(QHostAddress::Any, port);
    if (listening) {
        timer.start();
        listener.bind(QHostAddress::Any, port);
        QThread::start();
    }
    return listening;
}

void Server::run() {
    exec();
    stop();
}

void Server::stop() {
    if (serverConnection.isListening()) {
        serverConnection.close();
        listener.close();
        timer.stop();
        while (clients.size()>0)
            removeClient(clients[0]);
        clients.clear();
    }
}

void Server::ping() {
    for (int i=0; i < clients.size(); ++i) {
        PingMessage msg;
        msg.send(clients[i]->socket);
        QTime last = clients[i]->lastpingtime;
        int elapsed = last.elapsed();
        if (elapsed > PING_TIME) {
            clients[i]->socket->close();
        }
    }
}

void Server::remoteTurnMessageReceive(TurnMessage msg, RemoteClient*) {
    sendToAll(&msg);
}

void Server::remoteSurrenderMessageReceive(SurrenderMessage msg, RemoteClient*) {
    sendToAll(&msg);
}

void Server::remoteTryToConnectMessageReceive(TryToConnectMessage msg, RemoteClient* client) {
    int i, error=0;
    for (i=0; i<clients.size() && (msg.color()!=clients[i]->info.color() || clients[i]->state!=2); ++i) {}
    if (i!=clients.size())
        error=1;
    for (i=0; i<clients.size() && (msg.name()!=clients[i]->info.name() || clients[i]->state!=2); ++i) {}
    if (i!=clients.size())
        error=2;
    if (getPlayersCount()==maxClientsCount)
        error=4;
    ConnectionAcceptedMessage msg1(error);
    msg1.send(client->socket);
    if (!error) {
        client->info.setName(msg.name());
        client->info.setColor(msg.color());
        client->state = 2;
        ClientConnectMessage msg1(msg.name(), msg.color());
        sendToAll(&msg1);
        sendPlayersList();
    } else
        removeClient(client);
}

void Server::remoteError(RemoteClient *client) {
    client->socket->close();
}

void Server::newConnection() {
    if (serverConnection.hasPendingConnections()) {
        QTcpSocket* s = serverConnection.nextPendingConnection();
        RemoteClient *client = new RemoteClient(s);
        connect(client, SIGNAL(rcChatMessageReceive(ChatMessage,RemoteClient*)), this, SLOT(remoteChatMessageReceive(ChatMessage,RemoteClient*)));
        connect(client, SIGNAL(rcTryToConnectMessageReceive(TryToConnectMessage,RemoteClient*)), this, SLOT(remoteTryToConnectMessageReceive(TryToConnectMessage,RemoteClient*)));
        connect(client, SIGNAL(rcPingMessageReceive(PingMessage,RemoteClient*)), this, SLOT(remotePingMessageReceive(PingMessage,RemoteClient*)));
        connect(client, SIGNAL(rcTurnMessageReceive(TurnMessage,RemoteClient*)), this, SLOT(remoteTurnMessageReceive(TurnMessage,RemoteClient*)));
        connect(client, SIGNAL(rcSurrenderMessageReceive(SurrenderMessage,RemoteClient*)), this, SLOT(remoteSurrenderMessageReceive(SurrenderMessage,RemoteClient*)));
        connect(client, SIGNAL(rcDisconnected(RemoteClient*)), this, SLOT(remoteDisconnected(RemoteClient*)));
        connect(client, SIGNAL(rcError(RemoteClient*)), this, SLOT(remoteError(RemoteClient*)));
        clients.append(client);
        ServerReadyMessage msg;
        msg.send(s);
    }
}

void Server::remoteDisconnected(RemoteClient *client) {
    if (client->state==2) {
        ClientDisconnectMessage msg(client->info.name(), client->info.color());
        sendToAll(&msg);
        sendPlayersList();
    }
    removeClient(client);
}

void Server::remoteChatMessageReceive(ChatMessage msg, RemoteClient*) {
    sendToAll(&msg);
}

void Server::sendToAll(Message *msg) {
    QList<RemoteClient*>::iterator i;
    for (i = clients.begin(); i != clients.end(); ++i)
        if ((*i)->state==2) {
        msg->send((*i)->socket);
    }
}

void Server::sendPlayersList() {
    QList<ClientInfo> list;
    for (int i=0;i<clients.size();++i)
        if (clients[i]->state==2)
            list.append(clients[i]->info);
    PlayersListMessage msg(list);
    sendToAll(&msg);
}

void Server::startGame() {
    StartGameMessage msg;
    sendToAll(&msg);
}

void Server::restartGame(QList<ClientInfo> list) {
    RestartGameMessage msg(list);
    sendToAll(&msg);
}
