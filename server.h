#ifndef _SERVER_H_
#define _SERVER_H_

#include "client.h"
#include "message.h"
#include <QTcpServer>
#include <QThread>

class Server : public QThread
{
    Q_OBJECT

private:
    QList<RemoteClient *>   _clients;
    bool                    _isGameStarted;
    QUdpSocket              _listener;
    int                     _maxClientsCount;
    UdpMessageReceiver     *_messageReceiver;
    QTcpServer              _tcpServer;
    QTimer                  _timer;
    void sendPlayersList();
    void sendToAll(const Message &);

private slots:
    void clear();
    void removeClient(RemoteClient *);

    // from timer
    void ping();

    // from message receiver
    void receiveServerRequestMessage(const ServerRequestMessage &, const QHostAddress &, quint16);

    // from tcpserver
    void processNewConnection();

    // from remote client
    void processClientDisconnected(RemoteClient *);
    void processClientError(RemoteClient *);
    void receiveChatMessage(const ChatMessage &, RemoteClient *);
    void receiveSurrenderMessage(const SurrenderMessage &, RemoteClient *);
    void receiveTryToConnectMessage(const TryToConnectMessage &, RemoteClient *);
    void receiveTurnMessage(const TurnMessage &, RemoteClient *);

public:
    Server();
    QList<ClientInfo>   clients() const;
    QString             errorString() const     { return _tcpServer.errorString(); }
    int                 maxClientsCount() const { return _maxClientsCount; }
    int                 playersCount() const;
    void                start(int maxClientsCount, quint16 port);

public slots:
    // from app
    void startGame(QList<ClientInfo>);
    void stopGame();

    void stop();
};

#endif
