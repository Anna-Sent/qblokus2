#include "remoteclient.h"

#define STATE_CONNECTED_TO_SERVER       1
#define STATE_ACCEPTED_TO_GAME          2
#define STATE_DISCONNECTED_FROM_SERVER  3

RemoteClient::RemoteClient(QTcpSocket *s) : _lastPingTime(QTime::currentTime()), _state(STATE_CONNECTED_TO_SERVER)
{
    _socket = s;
    _messageReceiver = new TcpMessageReceiver(s);
    connect(_messageReceiver, SIGNAL(chatMessageReceived(ChatMessage)), this, SLOT(receiveChatMessage(ChatMessage)));
    connect(_messageReceiver, SIGNAL(pingMessageReceived(PingMessage)), this, SLOT(receivePingMessage(PingMessage)));
    connect(_messageReceiver, SIGNAL(serverStartGameMessageReceived(ServerStartGameMessage)), this, SLOT(receiveServerStartGameMessage(ServerStartGameMessage)));
    connect(_messageReceiver, SIGNAL(serverStopGameMessageReceived(ServerStopGameMessage)), this, SLOT(receiveServerStopGameMessage(ServerStopGameMessage)));
    connect(_messageReceiver, SIGNAL(surrenderMessageReceived(SurrenderMessage)), this, SLOT(receiveSurrenderMessage(SurrenderMessage)));
    connect(_messageReceiver, SIGNAL(tryToConnectMessageReceived(TryToConnectMessage)), this, SLOT(receiveTryToConnectMessage(TryToConnectMessage)));
    connect(_messageReceiver, SIGNAL(turnMessageReceived(TurnMessage)), this, SLOT(receiveTurnMessage(TurnMessage)));
    connect(_socket, SIGNAL(disconnected()), this, SLOT(processSocketDisconnected()), Qt::QueuedConnection);
    connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(processSocketError(QAbstractSocket::SocketError)));
    connect(this, SIGNAL(destroyed()), this, SLOT(setDisconnectedFromGame()));
    connect(this, SIGNAL(destroyed()), _messageReceiver, SLOT(deleteLater()));
    connect(this, SIGNAL(destroyed()), _socket, SLOT(deleteLater()));
    _lastPingTime.start();
}

void RemoteClient::setConnectedToGame(const ClientInfo &info)
{
    if (!isConnectedToGame())
    {
        _state = STATE_ACCEPTED_TO_GAME;
        _info = info;
    }
}

void RemoteClient::setDisconnectedFromGame()
{
    if (_state != STATE_DISCONNECTED_FROM_SERVER)
    {
        _state = STATE_DISCONNECTED_FROM_SERVER;
        _socket->disconnectFromHost();
    }
}

void RemoteClient::sendMessage(const Message &msg) const
{
    msg.send(_socket);
}

void RemoteClient::processSocketDisconnected()
{
    if (_state != STATE_DISCONNECTED_FROM_SERVER)
    {
        _state = STATE_DISCONNECTED_FROM_SERVER;
    }

    emit disconnected(this);
}

void RemoteClient::processSocketError(QAbstractSocket::SocketError)
{
    setDisconnectedFromGame();
    emit errorOccurred(this);
}

void RemoteClient::receiveChatMessage(const ChatMessage &msg)
{
    emit chatMessageReceived(msg, this);
}

void RemoteClient::receivePingMessage(const PingMessage &)
{
    _lastPingTime.start();
}

void RemoteClient::receiveServerStartGameMessage(const ServerStartGameMessage &)
{
    emit startGameMessageReceived(this);
}

void RemoteClient::receiveServerStopGameMessage(const ServerStopGameMessage &)
{
    emit stopGameMessageReceived(this);
}

void RemoteClient::receiveSurrenderMessage(const SurrenderMessage &msg)
{
    emit surrenderMessageReceived(msg, this);
}

void RemoteClient::receiveTryToConnectMessage(const TryToConnectMessage &msg)
{
    emit tryToConnectMessageReceived(msg, this);
}

void RemoteClient::receiveTurnMessage(const TurnMessage &msg)
{
    emit turnMessageReceived(msg, this);
}
