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
    QColor  color() const       { return _info.color(); }
    bool    isStarted() const   { return _isStarted; }
    QString name() const        { return _info.name(); }

public slots:
    void sendChatMessage(QString text);
    void sendSurrenderMessage(QString name, QColor color);
    void sendTurnMessage(QString name, QColor color, QString tile, int id, int x, int y);

    void start(const QColor &color, const QString &name, const QString &hostname, quint16 port);
    void stop();

private slots:
    void processSocketDisconnected();
    void processSocketError(QAbstractSocket::SocketError);

    void receiveChatMessage(ChatMessage);
    void receiveClientConnectMessage(ClientConnectMessage);
    void receiveClientDisconnectMessage(ClientDisconnectMessage);
    void receiveConnectionAcceptedMessage(ConnectionAcceptedMessage);
    void receivePingMessage(PingMessage);
    void receivePlayersListMessage(PlayersListMessage);
    void receiveStartGameMessage(StartGameMessage);
    void receiveServerReadyMessage(ServerReadyMessage);
    void receiveSurrenderMessage(SurrenderMessage);
    void receiveTurnMessage(TurnMessage);

    void timeout();

signals:
    void connectionAccepted();
    void connectionRejected(const QString &);
    void disconnected();
    void errorOccurred();
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
    void receiveChatMessage(ChatMessage);
    void receivePingMessage(PingMessage);
    void receiveSurrenderMessage(SurrenderMessage);
    void receiveTryToConnectMessage(TryToConnectMessage);
    void receiveTurnMessage(TurnMessage);

signals:
    void chatMessageReceived(ChatMessage, RemoteClient *);
    void disconnected(RemoteClient *);
    void errorOccurred(RemoteClient *);
    void surrenderMessageReceived(SurrenderMessage, RemoteClient *);
    void tryToConnectMessageReceived(TryToConnectMessage, RemoteClient *);
    void turnMessageReceived(TurnMessage, RemoteClient *);
};

#endif
