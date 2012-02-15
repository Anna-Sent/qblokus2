#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "messagereceiver.h"
#include <QTime>

class RemoteClient : public QObject
{
    Q_OBJECT

private:
    ClientInfo          _info;
    QTime               _lastPingTime;
    TcpMessageReceiver *_messageReceiver;
    QTcpSocket         *_socket;
    int                 _state;

public:
    RemoteClient(QTcpSocket *);
    QColor              color() const                           { return _info.color(); }
    int                 elapsedSinceLastPing() const            { return _lastPingTime.elapsed(); }
    const ClientInfo   &info() const                            { return _info; }
    bool                isConnectedToGame() const               { return _state == 2; }
    QString             name() const                            { return _info.name(); }
    void                sendMessage(const Message &msg) const;

public slots:
    void setConnectedToGame(const QString &name, const QColor &color);
    void setDisconnectedFromGame();

private slots:
    void processSocketDisconnected();
    void processSocketError(QAbstractSocket::SocketError);
    void receiveChatMessage(const ChatMessage &);
    void receivePingMessage(const PingMessage &);
    void receiveServerStartGameMessage(const ServerStartGameMessage &);
    void receiveServerStopGameMessage(const ServerStopGameMessage &);
    void receiveSurrenderMessage(const SurrenderMessage &);
    void receiveTryToConnectMessage(const TryToConnectMessage &);
    void receiveTurnMessage(const TurnMessage &);

signals:
    void chatMessageReceived(const ChatMessage &, RemoteClient *);
    void disconnected(RemoteClient *);
    void errorOccurred(RemoteClient *);
    void startGameMessageReceived(RemoteClient *);
    void stopGameMessageReceived(RemoteClient *);
    void surrenderMessageReceived(const SurrenderMessage &, RemoteClient *);
    void tryToConnectMessageReceived(const TryToConnectMessage &, RemoteClient *);
    void turnMessageReceived(const TurnMessage &, RemoteClient *);
};

#endif
