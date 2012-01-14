#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "messagereceiver.h"
#include <QTime>
#include <QTimer>
#include <QThread>
#include <QtNetwork/QTcpSocket>

class LocalClient: public QObject {
    Q_OBJECT
private:
    QTimer localtimer;
private:
    QTime lastpingtime;
    QTcpSocket* socket;
    TcpMessageReceiver* messageReceiver;
    ClientInfo info;
    bool _isStarted;
    void stop() {socket->close();localtimer.stop();_isStarted=false;}
public:
    LocalClient();
    ~LocalClient() { messageReceiver->deleteLater(); }
    void quit() { stop(); }
    void start(QString hostname, quint16 port) {socket->connectToHost(hostname, port);localtimer.start();_isStarted=true;}
    void setNickname(QString name) {info.setName(name);}
    void setColor(QColor color) {info.setColor(color);}
    QString getNickname() {return info.name();}
    QColor getColor() {return info.color();}
    bool isStarted() const {return _isStarted;}
private slots:
    void localChatMessageReceive(ChatMessage);
    void localPlayersListMessageReceive(PlayersListMessage);
    void localServerReadyMessageReceive(ServerReadyMessage);
    void localClientConnectMessageReceive(ClientConnectMessage);
    void localClientDisconnectMessageReceive(ClientDisconnectMessage);
    void localConnectionAcceptedMessageReceive(ConnectionAcceptedMessage);
    void localPingMessageReceive(PingMessage);
    void localStartGameMessageReceive(StartGameMessage);
    void localRestartGameMessageReceive(RestartGameMessage);
    void localTurnMessageReceive(TurnMessage);
    void localSurrenderMessageReceive(SurrenderMessage);
    void localConnected();
    void localDisconnected();
    void localError(QAbstractSocket::SocketError);
    // from timer
    void localTimerCheck();
    // from form
    void sendMessage(QString text);
    void turnDone(QString name,QColor color,QString tile,int id,int x,int y);
    void playerSurrendered(QString name,QColor color);
signals:
    void lcChatMessageReceive(QString, QColor, QString);
    void lcPlayersListMessageReceive(QList<ClientInfo>);
    void lcClientConnectMessageReceive(QString, QColor);
    void lcClientDisconnectMessageReceive(QString, QColor);
    void lcConnectionAcceptedMessageReceive(int);
    void lcStartGameMessageReceive();
    void lcRestartGameMessageReceive(QList<ClientInfo>);
    void lcTurnMessageReceive(QColor, int, int, int, QString);
    void lcSurrenderMessageReceive(QString, QColor);
    void lcConnected();
    void lcDisconnected();
    void lcError(QString);
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
