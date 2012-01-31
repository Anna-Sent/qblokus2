#include "client.h"
#include "constants.h"

LocalClient::LocalClient() : _isStarted(false), _lastPingTime(QTime::currentTime())
{
    _socket = new QTcpSocket;
    _messageReceiver = new TcpMessageReceiver(_socket);
    connect(_messageReceiver, SIGNAL(chatMessageReceived(ChatMessage)), this, SLOT(chatMessageReceived(ChatMessage)));
    connect(_messageReceiver, SIGNAL(clientConnectMessageReceived(ClientConnectMessage)), this, SLOT(clientConnectMessageReceived(ClientConnectMessage)));
    connect(_messageReceiver, SIGNAL(clientDisconnectMessageReceived(ClientDisconnectMessage)), this, SLOT(clientDisconnectMessageReceived(ClientDisconnectMessage)));
    connect(_messageReceiver, SIGNAL(connectionAcceptedMessageReceived(ConnectionAcceptedMessage)), this, SLOT(connectionAcceptedMessageReceived(ConnectionAcceptedMessage)));
    connect(_messageReceiver, SIGNAL(pingMessageReceived(PingMessage)), this, SLOT(pingMessageReceived(PingMessage)));
    connect(_messageReceiver, SIGNAL(playersListMessageReceived(PlayersListMessage)), this, SLOT(playersListMessageReceived(PlayersListMessage)));
    connect(_messageReceiver, SIGNAL(restartGameMessageReceived(RestartGameMessage)), this, SLOT(restartGameMessageReceived(RestartGameMessage)));
    connect(_messageReceiver, SIGNAL(serverReadyMessageReceived(ServerReadyMessage)), this, SLOT(serverReadyMessageReceived(ServerReadyMessage)));
    connect(_messageReceiver, SIGNAL(startGameMessageReceived(StartGameMessage)), this, SLOT(startGameMessageReceived(StartGameMessage)));
    connect(_messageReceiver, SIGNAL(surrenderMessageReceived(SurrenderMessage)), this, SLOT(surrenderMessageReceived(SurrenderMessage)));
    connect(_messageReceiver, SIGNAL(turnMessageReceived(TurnMessage)), this, SLOT(turnMessageReceived(TurnMessage)));
    connect(_socket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()), Qt::QueuedConnection);
    connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));

    connect(this, SIGNAL(destroyed()), this, SLOT(stop()));
    connect(this, SIGNAL(destroyed()), _messageReceiver, SLOT(deleteLater()));
    connect(this, SIGNAL(destroyed()), _socket, SLOT(deleteLater()));

    _localTimer.setInterval(PING_INTERVAL);
    connect(&_localTimer, SIGNAL(timeout()), this, SLOT(timeout()));
}

void LocalClient::start(QString hostname, quint16 port)
{
    if (!_isStarted)
    {
        _isStarted = true;
        _socket->connectToHost(hostname, port);
        _localTimer.start();
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
        case 3: reason = "The game is already started. Wait for finish of the game"; break;
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
    if (_isStarted)
    {
        _isStarted = false;
        _localTimer.stop();
    }

    emit disconnected();
}

void LocalClient::socketError(QAbstractSocket::SocketError)
{
    stop();
    emit errorOccurred(_socket->errorString());
    emit errorOccurred();
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
        stop();
        emit errorOccurred(QString::fromUtf8("Ping timeout"));
        emit errorOccurred();
    }
}

RemoteClient::RemoteClient(QTcpSocket *s) : _lastPingTime(QTime::currentTime()), _state(1)
{
    _socket = s;
    _messageReceiver = new TcpMessageReceiver(s);
    connect(_messageReceiver, SIGNAL(chatMessageReceived(ChatMessage)), this, SLOT(remoteChatMessageReceive(ChatMessage)));
    connect(_messageReceiver, SIGNAL(pingMessageReceived(PingMessage)), this, SLOT(remotePingMessageReceive(PingMessage)));
    connect(_messageReceiver, SIGNAL(surrenderMessageReceived(SurrenderMessage)), this, SLOT(remoteSurrenderMessageReceive(SurrenderMessage)));
    connect(_messageReceiver, SIGNAL(tryToConnectMessageReceived(TryToConnectMessage)), this, SLOT(remoteTryToConnectMessageReceive(TryToConnectMessage)));
    connect(_messageReceiver, SIGNAL(turnMessageReceived(TurnMessage)), this, SLOT(remoteTurnMessageReceive(TurnMessage)));
    connect(_socket, SIGNAL(disconnected()), this, SLOT(remoteDisconnected()), Qt::QueuedConnection);
    connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(remoteError(QAbstractSocket::SocketError)));
    connect(this, SIGNAL(destroyed()), this, SLOT(setDisconnectedFromGame()));
    connect(this, SIGNAL(destroyed()), _messageReceiver, SLOT(deleteLater()));
    connect(this, SIGNAL(destroyed()), _socket, SLOT(deleteLater()));
}

void RemoteClient::setConnectedToGame(const QString &name, const QColor &color)
{
    if (!isConnectedToGame())
    {
        _state = 2;
        _info.setName(name);
        _info.setColor(color);
    }
}

void RemoteClient::setDisconnectedFromGame()
{
    if (_state != 3)
    {
        _state = 3;
        _socket->disconnectFromHost();
    }
}

void RemoteClient::sendMessage(const Message &msg)
{
    msg.send(_socket);
}

void RemoteClient::remoteChatMessageReceive(ChatMessage msg)
{
    emit chatMessageReceived(msg, this);
}

void RemoteClient::remoteDisconnected()
{
    if (_state != 3)
    {
        _state = 3;
    }

    emit disconnected(this);
}

void RemoteClient::remoteError(QAbstractSocket::SocketError)
{
    setDisconnectedFromGame();
    emit errorOccurred(this);
}

void RemoteClient::remotePingMessageReceive(PingMessage)
{
    _lastPingTime.start();
}

void RemoteClient::remoteSurrenderMessageReceive(SurrenderMessage msg)
{
    emit surrenderMessageReceived(msg, this);
}

void RemoteClient::remoteTurnMessageReceive(TurnMessage msg)
{
    emit turnMessageReceived(msg, this);
}

void RemoteClient::remoteTryToConnectMessageReceive(TryToConnectMessage msg)
{
    emit tryToConnectMessageReceived(msg, this);
}
