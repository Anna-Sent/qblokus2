#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "messagereceiver.h"
#include <QTime>
#include <QTimer>

class LocalClient : public QObject
{
    Q_OBJECT

private:
    bool                _isStarted;
    ClientInfo          _info;
    QTime               _lastPingTime;
    QTimer              _localTimer;
    TcpMessageReceiver *_messageReceiver;
    QTcpSocket         *_socket;

public:
    LocalClient();
    QColor              color() const       { return _info.color(); }
    const ClientInfo   &info() const        { return _info; }
    bool                isStarted() const   { return _isStarted; }
    QString             name() const        { return _info.name(); }

public slots:
    void sendChatMessage(QString text);
    void sendSurrenderMessage(QString name, QColor color);
    void sendTurnMessage(QString name, QColor color, QString tile, int id, int x, int y);

    void start(const QColor &color, const QString &name, const QString &hostname, quint16 port);
    void stop();

private slots:
    void processSocketDisconnected();
    void processSocketError(QAbstractSocket::SocketError);

    void receiveChatMessage(const ChatMessage &);
    void receiveClientConnectMessage(const ClientConnectMessage &);
    void receiveClientDisconnectMessage(const ClientDisconnectMessage &);
    void receiveConnectionAcceptedMessage(const ConnectionAcceptedMessage &);
    void receivePingMessage(const PingMessage &);
    void receivePlayersListMessage(const PlayersListMessage &);
    void receiveStartGameMessage(const StartGameMessage &);
    void receiveServerReadyMessage(const ServerReadyMessage &);
    void receiveSurrenderMessage(const SurrenderMessage &);
    void receiveTurnMessage(const TurnMessage &);

    void timeout();

signals:
    void connectionAccepted();
    void connectionRejected(const QString &);
    void disconnected();
    void errorOccurred(QString);

    void chatMessageReceived(QString, QColor, QString);
    void clientConnectMessageReceived(QString, QColor);
    void clientDisconnectMessageReceived(QString, QColor);
    void playersListMessageReceived(QList<ClientInfo>);
    void startGameMessageReceived(QList<ClientInfo>);
    void surrenderMessageReceived(QString, QColor);
    void turnMessageReceived(QColor, int, int, int, QString);
};

class RemoteClient : public QObject
{
    Q_OBJECT

private:
    ClientInfo          _info;
    QTime               _lastPingTime;
    TcpMessageReceiver *_messageReceiver;
    QTcpSocket         *_socket;
    int                 _state;

public:
    RemoteClient(QTcpSocket *);
    QColor              color() const                   { return _info.color(); }
    int                 elapsedSinceLastPing() const    { return _lastPingTime.elapsed(); }
    const ClientInfo   &info() const                    { return _info; }
    bool                isConnectedToGame() const       { return _state == 2; }
    QString             name() const                    { return _info.name(); }

public slots:
    void sendMessage(const Message &msg);
    void setConnectedToGame(const QString &name, const QColor &color);
    void setDisconnectedFromGame();

private slots:
    void processSocketDisconnected();
    void processSocketError(QAbstractSocket::SocketError);
    void receiveChatMessage(const ChatMessage &);
    void receivePingMessage(const PingMessage &);
    void receiveSurrenderMessage(const SurrenderMessage &);
    void receiveTryToConnectMessage(const TryToConnectMessage &);
    void receiveTurnMessage(const TurnMessage &);

signals:
    void chatMessageReceived(const ChatMessage &, RemoteClient *);
    void disconnected(RemoteClient *);
    void errorOccurred(RemoteClient *);
    void surrenderMessageReceived(const SurrenderMessage &, RemoteClient *);
    void tryToConnectMessageReceived(const TryToConnectMessage &, RemoteClient *);
    void turnMessageReceived(const TurnMessage &, RemoteClient *);
};

#endif
