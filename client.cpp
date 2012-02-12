#include "client.h"
#include "constants.h"

#define STATE_CONNECTED_TO_SERVER       1
#define STATE_ACCEPTED_TO_GAME          2
#define STATE_DISCONNECTED_FROM_SERVER  3

LocalClient::LocalClient() : _isStarted(false), _lastPingTime(QTime::currentTime())
{
    _socket = new QTcpSocket;
    _messageReceiver = new TcpMessageReceiver(_socket);
    connect(_messageReceiver, SIGNAL(chatMessageReceived(const ChatMessage &)), this, SLOT(receiveChatMessage(const ChatMessage &)));
    connect(_messageReceiver, SIGNAL(clientConnectMessageReceived(ClientConnectMessage)), this, SLOT(receiveClientConnectMessage(ClientConnectMessage)));
    connect(_messageReceiver, SIGNAL(clientDisconnectMessageReceived(ClientDisconnectMessage)), this, SLOT(receiveClientDisconnectMessage(ClientDisconnectMessage)));
    connect(_messageReceiver, SIGNAL(connectionAcceptedMessageReceived(ConnectionAcceptedMessage)), this, SLOT(receiveConnectionAcceptedMessage(ConnectionAcceptedMessage)));
    connect(_messageReceiver, SIGNAL(pingMessageReceived(PingMessage)), this, SLOT(receivePingMessage(PingMessage)));
    connect(_messageReceiver, SIGNAL(playersListMessageReceived(PlayersListMessage)), this, SLOT(receivePlayersListMessage(PlayersListMessage)));
    connect(_messageReceiver, SIGNAL(startGameMessageReceived(StartGameMessage)), this, SLOT(receiveStartGameMessage(StartGameMessage)));
    connect(_messageReceiver, SIGNAL(serverReadyMessageReceived(ServerReadyMessage)), this, SLOT(receiveServerReadyMessage(ServerReadyMessage)));
    connect(_messageReceiver, SIGNAL(surrenderMessageReceived(SurrenderMessage)), this, SLOT(receiveSurrenderMessage(SurrenderMessage)));
    connect(_messageReceiver, SIGNAL(turnMessageReceived(TurnMessage)), this, SLOT(receiveTurnMessage(TurnMessage)));
    connect(_socket, SIGNAL(disconnected()), this, SLOT(processSocketDisconnected()), Qt::QueuedConnection);
    connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(processSocketError(QAbstractSocket::SocketError)));

    connect(this, SIGNAL(destroyed()), this, SLOT(stop()));
    connect(this, SIGNAL(destroyed()), _messageReceiver, SLOT(deleteLater()));
    connect(this, SIGNAL(destroyed()), _socket, SLOT(deleteLater()));

    _localTimer.setInterval(PING_INTERVAL);
    connect(&_localTimer, SIGNAL(timeout()), this, SLOT(timeout()));
}

void LocalClient::start(const QString &hostname, quint16 port)
{
    if (!_isStarted)
    {
        _isStarted = true;
        _socket->connectToHost(hostname, port);
        _localTimer.start();
        _lastPingTime.start();
    }
}

void LocalClient::stop()
{
    if (_isStarted)
    {
        _isStarted = false;
        _localTimer.stop();
        _socket->disconnectFromHost();
    }
}

void LocalClient::receiveChatMessage(const ChatMessage &msg)
{
    emit chatMessageReceived(msg.name(), msg.color(), msg.text());
}

void LocalClient::receiveClientConnectMessage(const ClientConnectMessage &msg)
{
    emit clientConnectMessageReceived(msg.name(), msg.color());
}

void LocalClient::receiveClientDisconnectMessage(const ClientDisconnectMessage &msg)
{
    emit clientDisconnectMessageReceived(msg.name(), msg.color());
}

void LocalClient::receiveConnectionAcceptedMessage(const ConnectionAcceptedMessage &msg)
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
        case ERROR_MAX_CONNECTIONS_NUM: reason = QString::fromUtf8("The maximum allowed connections number has been reached for the server"); break;
        case ERROR_GAME_STARTED:        reason = QString::fromUtf8("The game is already started. Wait for finish of the game"); break;
        case ERROR_MAX_PLAYERS_NUM:     reason = QString::fromUtf8("The maximum allowed number of players has been reached for the game"); break;
        case ERROR_NAME_IN_USE:         reason = QString::fromUtf8("This nickname is already in use"); break;
        case ERROR_COLOR_IN_USE:        reason = QString::fromUtf8("This color is already in use"); break;
        default:                        reason = QString::fromUtf8("Unknown reason");
        }

        emit connectionRejected(reason);
        stop();
    }
}

void LocalClient::receivePingMessage(const PingMessage &msg)
{
    msg.send(_socket);
    _lastPingTime.start();
}

void LocalClient::receivePlayersListMessage(const PlayersListMessage &msg)
{
    emit playersListMessageReceived(msg.list());
}

void LocalClient::receiveStartGameMessage(const StartGameMessage &msg)
{
    emit startGameMessageReceived(msg.list());
}

void LocalClient::receiveServerReadyMessage(const ServerReadyMessage &)
{
    TryToConnectMessage msg(_info);
    msg.send(_socket);
}

void LocalClient::receiveSurrenderMessage(const SurrenderMessage &msg)
{
    emit surrenderMessageReceived(msg.name(), msg.color());
}

void LocalClient::receiveTurnMessage(const TurnMessage &msg)
{
    emit turnMessageReceived(msg.color(), msg.x(), msg.y(), msg.id(), msg.mask());
}

void LocalClient::processSocketDisconnected()
{
    if (_isStarted)
    {
        _isStarted = false;
        _localTimer.stop();
    }

    emit disconnected();
}

void LocalClient::processSocketError(QAbstractSocket::SocketError)
{
    stop();
    emit errorOccurred(_socket->errorString());
}

void LocalClient::sendChatMessage(QString text)
{
    ChatMessage msg(_info.name(), _info.color(), text);
    msg.send(_socket);
}

void LocalClient::sendSurrenderMessage()
{
    SurrenderMessage msg(_info.name(), _info.color());
    msg.send(_socket);
}

void LocalClient::sendTurnMessage(QString name, QColor color, QString tile, int id, int x, int y)
{
    TurnMessage msg(name, color, tile, id, x, y);
    msg.send(_socket);
}

void LocalClient::timeout()
{
    int elapsed = _lastPingTime.elapsed();
    if (elapsed > PING_TIME)
    {
        stop();
        emit errorOccurred(QString::fromUtf8("Ping timeout"));
    }
}

RemoteClient::RemoteClient(QTcpSocket *s) : _lastPingTime(QTime::currentTime()), _state(STATE_CONNECTED_TO_SERVER)
{
    _socket = s;
    _messageReceiver = new TcpMessageReceiver(s);
    connect(_messageReceiver, SIGNAL(chatMessageReceived(const ChatMessage &)), this, SLOT(receiveChatMessage(const ChatMessage &)));
    connect(_messageReceiver, SIGNAL(pingMessageReceived(PingMessage)), this, SLOT(receivePingMessage(PingMessage)));
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

void RemoteClient::setConnectedToGame(const QString &name, const QColor &color)
{
    if (!isConnectedToGame())
    {
        _state = STATE_ACCEPTED_TO_GAME;
        _info.setName(name);
        _info.setColor(color);
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

void RemoteClient::sendMessage(const Message &msg)
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

void RemoteClient::receiveSurrenderMessage(const SurrenderMessage &msg)
{
    emit surrenderMessageReceived(msg, this);
}

void RemoteClient::receiveTurnMessage(const TurnMessage &msg)
{
    emit turnMessageReceived(msg, this);
}

void RemoteClient::receiveTryToConnectMessage(const TryToConnectMessage &msg)
{
    emit tryToConnectMessageReceived(msg, this);
}
