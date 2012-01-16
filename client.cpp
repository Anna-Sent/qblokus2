#include "client.h"
#define PING_INTERVAL 5000
#define PING_TIME 1500000

LocalClient::LocalClient() : _isStarted(false), _lastPingTime(QTime::currentTime())
{
    _localTimer.setInterval(PING_INTERVAL);
    connect(&_localTimer, SIGNAL(timeout()), this, SLOT(timeout()));

    _socket = new QTcpSocket;
    _messageReceiver = new TcpMessageReceiver(_socket);
    connect(_messageReceiver, SIGNAL(chatMessageReceive(ChatMessage)), this, SLOT(chatMessageReceived(ChatMessage)));
    connect(_messageReceiver, SIGNAL(clientConnectMessageReceive(ClientConnectMessage)), this, SLOT(clientConnectMessageReceived(ClientConnectMessage)));
    connect(_messageReceiver, SIGNAL(clientDisconnectMessageReceive(ClientDisconnectMessage)), this, SLOT(clientDisconnectMessageReceived(ClientDisconnectMessage)));
    connect(_messageReceiver, SIGNAL(connectionAcceptedMessageReceive(ConnectionAcceptedMessage)), this, SLOT(connectionAcceptedMessageReceived(ConnectionAcceptedMessage)));
    connect(_messageReceiver, SIGNAL(pingMessageReceive(PingMessage)), this, SLOT(pingMessageReceived(PingMessage)));
    connect(_messageReceiver, SIGNAL(playersListMessageReceive(PlayersListMessage)), this, SLOT(playersListMessageReceived(PlayersListMessage)));
    connect(_messageReceiver, SIGNAL(restartGameMessageReceive(RestartGameMessage)), this, SLOT(restartGameMessageReceived(RestartGameMessage)));
    connect(_messageReceiver, SIGNAL(serverReadyMessageReceive(ServerReadyMessage)), this, SLOT(serverReadyMessageReceived(ServerReadyMessage)));
    connect(_messageReceiver, SIGNAL(startGameMessageReceive(StartGameMessage)), this, SLOT(startGameMessageReceived(StartGameMessage)));
    connect(_messageReceiver, SIGNAL(surrenderMessageReceive(SurrenderMessage)), this, SLOT(surrenderMessageReceived(SurrenderMessage)));
    connect(_messageReceiver, SIGNAL(turnMessageReceive(TurnMessage)), this, SLOT(turnMessageReceived(TurnMessage)));
    connect(_socket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));
    connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
}

LocalClient::~LocalClient()
{
    _messageReceiver->deleteLater();
    //socket->deleteLater();
}

void LocalClient::start(QString hostname, quint16 port)
{
    _socket->connectToHost(hostname, port);
    _localTimer.start();
    _isStarted = true;
}

void LocalClient::stop()
{
    _isStarted = false;
    _localTimer.stop();
    _socket->disconnectFromHost();
}

void LocalClient::chatMessageReceived(ChatMessage msg)
{
    emit chatMessageReceived(msg.name(), msg.color(), msg.text());
}

void LocalClient::clientConnectMessageReceived(ClientConnectMessage msg)
{
    emit clientConnectMessageReceived(msg.name(), msg.color());
}

void LocalClient::clientDisconnectMessageReceived(ClientDisconnectMessage msg)
{
    emit clientDisconnectMessageReceived(msg.name(), msg.color());
}

void LocalClient::connectionAcceptedMessageReceived(ConnectionAcceptedMessage msg)
{
    if (msg.errorCode() == 0)
    {
        emit connectionAccepted();
    }
    else
    {
        QString reason;
        switch (msg.errorCode())
        {
        case 1: reason = "This color is already in use"; break;
        case 2: reason = "This nickname is already in use"; break;
        case 4: reason = "The maximum allowed number of players has been reached for the game"; break;
        default:reason = "Unknown reason";
        }

        emit connectionRejected(reason);
        stop();
    }
}

void LocalClient::pingMessageReceived(PingMessage msg)
{
    msg.send(_socket);
    _lastPingTime.start();
}

void LocalClient::playersListMessageReceived(PlayersListMessage msg)
{
    emit playersListMessageReceived(msg.list());
}

void LocalClient::restartGameMessageReceived(RestartGameMessage msg)
{
    emit restartGameMessageReceived(msg.list());
}

void LocalClient::serverReadyMessageReceived(ServerReadyMessage)
{
    TryToConnectMessage msg(_info);
    msg.send(_socket);
}

void LocalClient::startGameMessageReceived(StartGameMessage)
{
    emit startGameMessageReceived();
}

void LocalClient::surrenderMessageReceived(SurrenderMessage msg)
{
    emit surrenderMessageReceived(msg.name(), msg.color());
}

void LocalClient::turnMessageReceived(TurnMessage msg)
{
    emit turnMessageReceived(msg.color(), msg.x(), msg.y(), msg.id(), msg.mask());
}

void LocalClient::socketDisconnected()
{
    _isStarted = false;
    _localTimer.stop();
    emit disconnected();
}

void LocalClient::socketError(QAbstractSocket::SocketError)
{
    emit error(_socket->errorString());
    emit error();
    _socket->abort();
    _isStarted = false;
    _localTimer.stop();
}

void LocalClient::doTurn(QString name, QColor color, QString tile, int id, int x, int y)
{
    TurnMessage msg(name, color, tile, id, x, y);
    msg.send(_socket);
}

void LocalClient::sendMessage(QString text)
{
    ChatMessage msg(_info.name(), _info.color(), text);
    msg.send(_socket);
}

void LocalClient::surrender(QString name, QColor color)
{
    SurrenderMessage msg(name, color);
    msg.send(_socket);
}

void LocalClient::timeout()
{
    int elapsed = _lastPingTime.elapsed();
    if (elapsed > PING_TIME)
    {
        emit error(QString::fromUtf8("Disconnected"));
        emit error();
    }
}

RemoteClient::RemoteClient(QTcpSocket *s) : _state(1), lastpingtime(QTime::currentTime())
{
    socket = s;
    _messageReceiver = new TcpMessageReceiver(s);
    connect(_messageReceiver, SIGNAL(chatMessageReceive(ChatMessage)), this, SLOT(remoteChatMessageReceive(ChatMessage)));
    connect(_messageReceiver, SIGNAL(pingMessageReceive(PingMessage)), this, SLOT(remotePingMessageReceive(PingMessage)));
    connect(_messageReceiver, SIGNAL(surrenderMessageReceive(SurrenderMessage)), this, SLOT(remoteSurrenderMessageReceive(SurrenderMessage)));
    connect(_messageReceiver, SIGNAL(tryToConnectMessageReceive(TryToConnectMessage)), this, SLOT(remoteTryToConnectMessageReceive(TryToConnectMessage)));
    connect(_messageReceiver, SIGNAL(turnMessageReceive(TurnMessage)), this, SLOT(remoteTurnMessageReceive(TurnMessage)));
    connect(socket, SIGNAL(disconnected()), this, SLOT(remoteDisconnected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(remoteError(QAbstractSocket::SocketError)));
}

void RemoteClient::remoteChatMessageReceive(ChatMessage msg)
{
    emit rcChatMessageReceive(msg, this);
}

void RemoteClient::remoteDisconnected()
{
    emit rcDisconnected(this);
}

void RemoteClient::remoteError(QAbstractSocket::SocketError)
{
    emit rcError(this);
}

void RemoteClient::remotePingMessageReceive(PingMessage msg)
{
    emit rcPingMessageReceive(msg, this);
}

void RemoteClient::remoteSurrenderMessageReceive(SurrenderMessage msg)
{
    emit rcSurrenderMessageReceive(msg, this);
}

void RemoteClient::remoteTurnMessageReceive(TurnMessage msg)
{
    emit rcTurnMessageReceive(msg, this);
}

void RemoteClient::remoteTryToConnectMessageReceive(TryToConnectMessage msg)
{
    emit rcTryToConnectMessageReceive(msg, this);
}
