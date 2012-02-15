#ifndef MESSAGERECEIVER_H
#define MESSAGERECEIVER_H

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
    TcpMessageReceiver(QTcpSocket *, QObject *parent = 0);

signals:
    void chatMessageReceived(const ChatMessage &);
    void clientConnectMessageReceived(const ClientConnectMessage &);
    void clientDisconnectMessageReceived(const ClientDisconnectMessage &);
    void connectionAcceptedMessageReceived(const ConnectionAcceptedMessage &);
    void errorMessageReceived(const ErrorMessage &);
    void pingMessageReceived(const PingMessage &);
    void playersListMessageReceived(const PlayersListMessage &);
    void serverReadyMessageReceived(const ServerReadyMessage &);
    void serverStartGameMessageReceived(const ServerStartGameMessage &);
    void serverStopGameMessageReceived(const ServerStopGameMessage &);
    void startGameMessageReceived(const StartGameMessage &);
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
    UdpMessageReceiver(QUdpSocket *, QObject *parent = 0);

signals:
    void serverInfoMessageReceived(const ServerInfoMessage &, const QHostAddress &, quint16);
    void serverRequestMessageReceived(const ServerRequestMessage &, const QHostAddress &, quint16);
};

#endif
