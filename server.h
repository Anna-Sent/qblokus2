#ifndef _SERVER_H_
#define _SERVER_H_

#include "client.h"
#include "message.h"
#include <QTcpServer>
#include <QThread>

class Server : public QThread {
    Q_OBJECT

private:
    QList<RemoteClient *> _clients;
    bool _isGameStarted;
    QUdpSocket _listener;
    int _maxClientsCount;
    UdpMessageReceiver *_messageReceiver;
    QTcpServer _tcpServer;
    QTimer _timer;
    void sendPlayersList();
    void sendToAll(const Message &);

private slots:
    void clear();
    void removeClient(RemoteClient *);

    //from timer
    void ping();

    //from message receiver
    void serverRequestMessageReceive(ServerRequestMessage, const QHostAddress &, quint16);

    //from tcpserver
    void newConnection();

    //from remote client
    void remoteChatMessageReceive(ChatMessage, RemoteClient *);
    void remoteDisconnected(RemoteClient *);
    void remoteError(RemoteClient *);
    void remoteSurrenderMessageReceive(SurrenderMessage, RemoteClient *);
    void remoteTryToConnectMessageReceive(TryToConnectMessage, RemoteClient *);
    void remoteTurnMessageReceive(TurnMessage, RemoteClient *);

public:
    Server();
    QList<ClientInfo> clients() const;
    QString errorString() const { return _tcpServer.errorString(); }
    int maxClientsCount() const { return _maxClientsCount; }
    int playersCount() const;
    void start(int maxClientsCount, quint16 port);

public slots:
    void stop();

    //from app
    void finishGame();
    void startGame(QList<ClientInfo>);
};

#endif
