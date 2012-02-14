#ifndef _SERVER_H_
#define _SERVER_H_

#include "remoteclient.h"
#include "message.h"
#include <QTcpServer>
#include <QTimer>

class Server : public QObject
{
    Q_OBJECT

private:
    QList<RemoteClient *>   _clients;
    bool                    _isGameStarted;
    QUdpSocket             *_listener;
    int                     _playersCount;
    UdpMessageReceiver     *_messageReceiver;
    QTcpServer             *_tcpServer;
    QTimer                 *_timer;
    QList<ClientInfo> clients() const;
    void sendPlayersList();
    void sendToAll(const Message &);

private slots:
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
    QString errorString() const         { return _tcpServer->errorString(); }
    bool    isListening() const         { return _tcpServer->isListening(); }
    int     playersCount() const        { return _playersCount; }
    int     currentPlayersCount() const;

public slots:
    // from app
    void start(int maxClientsCount, quint16 port);
    void startGame();
    void stop();
    void stopGame();

signals:
    void started();
};

#endif
