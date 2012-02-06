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

private slots:
    void readyRead();

public:
    TcpMessageReceiver(QTcpSocket *);

signals:
    void chatMessageReceived(const ChatMessage &);
    void clientConnectMessageReceived(const ClientConnectMessage &);
    void clientDisconnectMessageReceived(const ClientDisconnectMessage &);
    void connectionAcceptedMessageReceived(const ConnectionAcceptedMessage &);
    void pingMessageReceived(const PingMessage &);
    void playersListMessageReceived(const PlayersListMessage &);
    void startGameMessageReceived(const StartGameMessage &);
    void serverReadyMessageReceived(const ServerReadyMessage &);
    void surrenderMessageReceived(const SurrenderMessage &);
    void tryToConnectMessageReceived(const TryToConnectMessage &);
    void turnMessageReceived(const TurnMessage &);
};

class UdpMessageReceiver : public QObject
{
    Q_OBJECT

private:
    QUdpSocket *socket;
    void processData(QByteArray &, const QHostAddress &, quint16);

private slots:
    void readyRead();

public:
    UdpMessageReceiver(QUdpSocket *);

signals:
    void serverInfoMessageReceived(const ServerInfoMessage &, const QHostAddress &, quint16);
    void serverRequestMessageReceived(const ServerRequestMessage &, const QHostAddress &, quint16);
};

#endif
