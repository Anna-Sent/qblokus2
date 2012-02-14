#include "localclient.h"
#include "constants.h"

LocalClient::LocalClient() : _isStarted(false), _lastPingTime(QTime::currentTime())
{
    _socket = new QTcpSocket(this);
    _messageReceiver = new TcpMessageReceiver(_socket, this);
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
    connect(_socket, SIGNAL(disconnected()), this, SLOT(processSocketDisconnected()));
    connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(processSocketError(QAbstractSocket::SocketError)));

    connect(this, SIGNAL(destroyed()), this, SLOT(stop()));
    connect(this, SIGNAL(destroyed()), _messageReceiver, SLOT(deleteLater()));
    connect(this, SIGNAL(destroyed()), _socket, SLOT(deleteLater()));

    _timer = new QTimer(this);
    _timer->setInterval(PING_INTERVAL);
    connect(_timer, SIGNAL(timeout()), this, SLOT(timeout()));
}

void LocalClient::start(const QString &hostname, quint16 port)
{
    if (!_isStarted)
    {
        _isStarted = true;
        _socket->connectToHost(hostname, port);
        _timer->start();
        _lastPingTime.start();
    }
}

void LocalClient::stop()
{
    if (_isStarted)
    {
        _isStarted = false;
        _timer->stop();
        _socket->disconnectFromHost();
    }
}

void LocalClient::receiveChatMessage(const ChatMessage &msg)
{
    emit chatMessageReceived(msg.info(), msg.text());
}

void LocalClient::receiveClientConnectMessage(const ClientConnectMessage &msg)
{
    emit clientConnectMessageReceived(msg.info());
}

void LocalClient::receiveClientDisconnectMessage(const ClientDisconnectMessage &msg)
{
    emit clientDisconnectMessageReceived(msg.info());
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
    emit surrenderMessageReceived(msg.info());
}

void LocalClient::receiveTurnMessage(const TurnMessage &msg)
{
    emit turnMessageReceived(msg.color(), msg.mask(), msg.id(), msg.x(), msg.y());
}

void LocalClient::processSocketDisconnected()
{
    if (_isStarted)
    {
        _isStarted = false;
        _timer->stop();
    }

    emit disconnected();
}

void LocalClient::processSocketError(QAbstractSocket::SocketError)
{
    stop();
    emit errorOccurred(_socket->errorString());
}

void LocalClient::sendChatMessage(const ClientInfo &info, const QString &text)
{
    ChatMessage msg(info, text);
    msg.send(_socket);
}

void LocalClient::sendSurrenderMessage(const ClientInfo &info)
{
    SurrenderMessage msg(info);
    msg.send(_socket);
}

void LocalClient::sendTurnMessage(const ClientInfo &info, const QString &tile, int id, int x, int y)
{
    TurnMessage msg(info, tile, id, x, y);
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