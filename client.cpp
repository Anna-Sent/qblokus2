#include "client.h"
#define PING_INTERVAL 5000
#define PING_TIME 1500000

RemoteClient::RemoteClient(QTcpSocket *s) : state(1), lastpingtime(QTime::currentTime()) {
    messageReceiver = new TcpMessageReceiver(s);
    connect(messageReceiver, SIGNAL(chatMessageReceive(ChatMessage)), this, SLOT(remoteChatMessageReceive(ChatMessage)));
    connect(messageReceiver, SIGNAL(tryToConnectMessageReceive(TryToConnectMessage)), this, SLOT(remoteTryToConnectMessageReceive(TryToConnectMessage)));
    connect(messageReceiver, SIGNAL(pingMessageReceive(PingMessage)), this, SLOT(remotePingMessageReceive(PingMessage)));
    connect(messageReceiver, SIGNAL(turnMessageReceive(TurnMessage)), this, SLOT(remoteTurnMessageReceive(TurnMessage)));
    connect(messageReceiver, SIGNAL(surrenderMessageReceive(SurrenderMessage)), this, SLOT(remoteSurrenderMessageReceive(SurrenderMessage)));
    connect(s, SIGNAL(disconnected()), this, SLOT(remoteDisconnected()));
    connect(s, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(remoteError(QAbstractSocket::SocketError)));
    this->socket = s;
    this->state = 1;
}

void RemoteClient::remoteChatMessageReceive(ChatMessage msg) {
    emit rcChatMessageReceive(msg,this);
}

void RemoteClient::remoteTryToConnectMessageReceive(TryToConnectMessage msg) {
    emit rcTryToConnectMessageReceive(msg,this);
}

void RemoteClient::remotePingMessageReceive(PingMessage msg) {
    emit rcPingMessageReceive(msg,this);
}

void RemoteClient::remoteTurnMessageReceive(TurnMessage msg) {
    emit rcTurnMessageReceive(msg,this);
}

void RemoteClient::remoteSurrenderMessageReceive(SurrenderMessage msg) {
    emit rcSurrenderMessageReceive(msg,this);
}

void RemoteClient::remoteDisconnected() {
    emit rcDisconnected(this);
}

void RemoteClient::remoteError(QAbstractSocket::SocketError) {
    emit rcError(this);
}

LocalClient::LocalClient():lastpingtime(QTime::currentTime()),_isStarted(false) {
    localtimer.setInterval(PING_INTERVAL);
    connect(&localtimer, SIGNAL(timeout()), this, SLOT(localTimerCheck()));
    socket = new QTcpSocket;
    messageReceiver = new TcpMessageReceiver(socket);
    connect(messageReceiver, SIGNAL(chatMessageReceive(ChatMessage)), this, SLOT(localChatMessageReceive(ChatMessage)));
    connect(messageReceiver, SIGNAL(playersListMessageReceive(PlayersListMessage)), this, SLOT(localPlayersListMessageReceive(PlayersListMessage)));
    connect(messageReceiver, SIGNAL(serverReadyMessageReceive(ServerReadyMessage)), this, SLOT(localServerReadyMessageReceive(ServerReadyMessage)));
    connect(messageReceiver, SIGNAL(clientConnectMessageReceive(ClientConnectMessage)), this, SLOT(localClientConnectMessageReceive(ClientConnectMessage)));
    connect(messageReceiver, SIGNAL(clientDisconnectMessageReceive(ClientDisconnectMessage)), this, SLOT(localClientDisconnectMessageReceive(ClientDisconnectMessage)));
    connect(messageReceiver, SIGNAL(connectionAcceptedMessageReceive(ConnectionAcceptedMessage)), this, SLOT(localConnectionAcceptedMessageReceive(ConnectionAcceptedMessage)));
    connect(messageReceiver, SIGNAL(pingMessageReceive(PingMessage)), this, SLOT(localPingMessageReceive(PingMessage)));
    connect(messageReceiver, SIGNAL(startGameMessageReceive(StartGameMessage)), this, SLOT(localStartGameMessageReceive(StartGameMessage)));
    connect(messageReceiver, SIGNAL(restartGameMessageReceive(RestartGameMessage)), this, SLOT(localRestartGameMessageReceive(RestartGameMessage)));
    connect(messageReceiver, SIGNAL(turnMessageReceive(TurnMessage)), this, SLOT(localTurnMessageReceive(TurnMessage)));
    connect(messageReceiver, SIGNAL(surrenderMessageReceive(SurrenderMessage)), this, SLOT(localSurrenderMessageReceive(SurrenderMessage)));
    connect(socket, SIGNAL(connected()), this, SLOT(localConnected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(localDisconnected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(localError(QAbstractSocket::SocketError)));
}

void LocalClient::localChatMessageReceive(ChatMessage msg) {
    emit lcChatMessageReceive(msg.name(), msg.color(), msg.text());
}

void LocalClient::localPlayersListMessageReceive(PlayersListMessage msg) {
    emit lcPlayersListMessageReceive(msg.list());
}

void LocalClient::localServerReadyMessageReceive(ServerReadyMessage) {
    TryToConnectMessage msg1(info);
    msg1.send(socket);
}

void LocalClient::localClientConnectMessageReceive(ClientConnectMessage msg) {
    emit lcClientConnectMessageReceive(msg.name(), msg.color());
}

void LocalClient::localClientDisconnectMessageReceive(ClientDisconnectMessage msg) {
    emit lcClientDisconnectMessageReceive(msg.name(), msg.color());
}

void LocalClient::localConnectionAcceptedMessageReceive(ConnectionAcceptedMessage msg) {
    emit lcConnectionAcceptedMessageReceive(msg.errorCode());
}

void LocalClient::localStartGameMessageReceive(StartGameMessage) {
    emit lcStartGameMessageReceive();
}

void LocalClient::localRestartGameMessageReceive(RestartGameMessage msg) {
    emit lcRestartGameMessageReceive(msg.list());
}

void LocalClient::localTurnMessageReceive(TurnMessage msg) {
    emit lcTurnMessageReceive(msg.color(), msg.x(), msg.y(), msg.id(), msg.mask());
}

void LocalClient::localSurrenderMessageReceive(SurrenderMessage msg) {
    emit lcSurrenderMessageReceive(msg.name(), msg.color());
}

void LocalClient::localConnected() {
    emit lcConnected();
}

void LocalClient::localDisconnected() {
    emit lcDisconnected();
}

void LocalClient::localError(QAbstractSocket::SocketError) {
    emit lcError(socket->errorString());
    emit lcError();
    socket->abort();
}

void LocalClient::localPingMessageReceive(PingMessage msg) {
    msg.send(socket);
    lastpingtime.start();
}

void LocalClient::localTimerCheck() {
    int elapsed = lastpingtime.elapsed();
    if (elapsed > PING_TIME)
    {
        emit lcError(QString::fromUtf8("Проверьте кабель"));
        emit lcError();
    }
}

void LocalClient::sendMessage(QString text) {
    ChatMessage msg(info.name(), info.color(), text);
    msg.send(socket);
}

void LocalClient::turnDone(QString name,QColor color,QString tile,int id,int x,int y) {
    TurnMessage msg(name,color,tile,id,x,y);
    msg.send(socket);
}

void LocalClient::playerSurrendered(QString name,QColor color) {
    SurrenderMessage msg(name,color);
    msg.send(socket);
}
