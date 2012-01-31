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
    void tryToConnectMessageReceived(TryToConnectMessage);
    void turnMessageReceived(TurnMessage);
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
    void serverInfoMessageReceived(ServerInfoMessage, const QHostAddress &, quint16);
    void serverRequestMessageReceived(ServerRequestMessage, const QHostAddress &, quint16);
};

#endif
