#ifndef MESSAGERECV_H_
#define MESSAGERECV_H_

#include "message.h"
#include <QAbstractSocket>

class MessageReceiver : public QObject
{
    Q_OBJECT

private:
    QByteArray buffer;
    Message *current;
    QAbstractSocket *socket;
    void processData(const QHostAddress &, quint16);

public:
    MessageReceiver(QAbstractSocket *);
    ~MessageReceiver();

private slots:
    void readyRead();
    void readyReadUdp();

signals:
    void getMessage(QByteArray);

    void chatMessageReceive(ChatMessage);
    void clientConnectMessageReceive(ClientConnectMessage);
    void clientDisconnectMessageReceive(ClientDisconnectMessage);
    void connectionAcceptedMessageReceive(ConnectionAcceptedMessage);
    void pingMessageReceive(PingMessage);
    void playersListMessageReceive(PlayersListMessage);
    void restartGameMessageReceive(RestartGameMessage);
    void serverInfoMessageReceive(ServerInfoMessage, const QHostAddress &, quint16);
    void serverReadyMessageReceive(ServerReadyMessage);
    void serverRequestMessageReceive(ServerRequestMessage, const QHostAddress &, quint16);
    void startGameMessageReceive(StartGameMessage);
    void surrenderMessageReceive(SurrenderMessage);
    void tryToConnectMessageReceive(TryToConnectMessage);
    void turnMessageReceive(TurnMessage);
};

#endif
