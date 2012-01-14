#ifndef MESSAGERECV_H_
#define MESSAGERECV_H_

#include "message.h"
#include <QTcpSocket>
#include <QUdpSocket>

class TcpMessageReceiver : public QObject
{
    Q_OBJECT

private:
    QByteArray buffer;
    Message *current;
    QTcpSocket *socket;
    void processData();

public:
    TcpMessageReceiver(QTcpSocket *);
    ~TcpMessageReceiver();

private slots:
    void readyRead();

signals:
    void chatMessageReceive(ChatMessage);
    void clientConnectMessageReceive(ClientConnectMessage);
    void clientDisconnectMessageReceive(ClientDisconnectMessage);
    void connectionAcceptedMessageReceive(ConnectionAcceptedMessage);
    void pingMessageReceive(PingMessage);
    void playersListMessageReceive(PlayersListMessage);
    void restartGameMessageReceive(RestartGameMessage);
    void serverReadyMessageReceive(ServerReadyMessage);
    void startGameMessageReceive(StartGameMessage);
    void surrenderMessageReceive(SurrenderMessage);
    void tryToConnectMessageReceive(TryToConnectMessage);
    void turnMessageReceive(TurnMessage);
};

class UdpMessageReceiver : public QObject
{
    Q_OBJECT

private:
    QUdpSocket *socket;
    void processData(QByteArray &, const QHostAddress &, quint16);

public:
    UdpMessageReceiver(QUdpSocket *);
    ~UdpMessageReceiver();

private slots:
    void readyRead();

signals:
    void serverInfoMessageReceive(ServerInfoMessage, const QHostAddress &, quint16);
    void serverRequestMessageReceive(ServerRequestMessage, const QHostAddress &, quint16);
};

#endif
