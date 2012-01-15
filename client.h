#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "messagereceiver.h"
#include <QTime>
#include <QTimer>

class LocalClient : public QObject
{
    Q_OBJECT

private:
    bool _isStarted;
    ClientInfo _info;
    QTime _lastPingTime;
    QTimer _localTimer;
    TcpMessageReceiver *_messageReceiver;
    QTcpSocket *_socket;

public:
    LocalClient();
    ~LocalClient();
    QColor color() const { return _info.color(); }
    bool isStarted() const { return _isStarted; }
    QString nickname() const {return _info.name(); }

public slots:
    void setNickname(QString name) { _info.setName(name); }
    void setColor(QColor color) { _info.setColor(color); }

    void start(QString hostname, quint16 port);
    void stop();

    void doTurn(QString name, QColor color, QString tile, int id, int x, int y);
    void sendMessage(QString text);
    void surrender(QString name, QColor color);

private slots:
    void chatMessageReceived(ChatMessage);
    void clientConnectMessageReceived(ClientConnectMessage);
    void clientDisconnectMessageReceived(ClientDisconnectMessage);
    void connectionAcceptedMessageReceived(ConnectionAcceptedMessage);
    void pingMessageReceived(PingMessage);
    void playersListMessageReceived(PlayersListMessage);
    void restartGameMessageReceived(RestartGameMessage);
    void serverReadyMessageReceived(ServerReadyMessage);
    void startGameMessageReceived(StartGameMessage);
    void surrenderMessageReceived(SurrenderMessage);
    void turnMessageReceived(TurnMessage);

    void socketDisconnected();
    void socketError(QAbstractSocket::SocketError);

    void timeout();

signals:
    void connectionAccepted();
    void connectionRejected(const QString &);
    void disconnected();
    void error();
    void error(QString);

    void chatMessageReceived(QString, QColor, QString);
    void clientConnectMessageReceived(QString, QColor);
    void clientDisconnectMessageReceived(QString, QColor);
    void playersListMessageReceived(QList<ClientInfo>);
    void restartGameMessageReceived(QList<ClientInfo>);
    void startGameMessageReceived();
    void surrenderMessageReceived(QString, QColor);
    void turnMessageReceived(QColor, int, int, int, QString);
};

class RemoteClient : public QObject {
    Q_OBJECT
public:
    int state;
    QTime lastpingtime;
    QTcpSocket* socket;
    TcpMessageReceiver* messageReceiver;
    ClientInfo info;
public:
    ~RemoteClient() { messageReceiver->deleteLater(); }
    RemoteClient(QTcpSocket*);
private slots:
    void remoteChatMessageReceive(ChatMessage);
    void remoteTryToConnectMessageReceive(TryToConnectMessage);
    void remotePingMessageReceive(PingMessage);
    void remoteTurnMessageReceive(TurnMessage);
    void remoteSurrenderMessageReceive(SurrenderMessage);
    void remoteDisconnected();
    void remoteError(QAbstractSocket::SocketError);
signals:
    void rcChatMessageReceive(ChatMessage,RemoteClient*);
    void rcTryToConnectMessageReceive(TryToConnectMessage,RemoteClient*);
    void rcPingMessageReceive(PingMessage,RemoteClient*);
    void rcTurnMessageReceive(TurnMessage,RemoteClient*);
    void rcSurrenderMessageReceive(SurrenderMessage,RemoteClient*);
    void rcDisconnected(RemoteClient*);
    void rcError(RemoteClient*);
};

#endif
