#ifndef LOCALCLIENT_H
#define LOCALCLIENT_H

#include "clientinfo.h"
#include "messagereceiver.h"
#include <QTime>
#include <QTimer>

class LocalClient : public QObject
{
    Q_OBJECT

private:
    bool                _isStarted;
    ClientInfo          _info;
    QTime               _lastPingTime;
    QTimer             *_timer;
    TcpMessageReceiver *_messageReceiver;
    QTcpSocket         *_socket;

public:
    LocalClient();
    QColor              color() const               { return _info.color(); }
    QString             socketErrorString() const   { return _socket->errorString(); }
    const ClientInfo   &info() const                { return _info; }
    bool                isStarted() const           { return _isStarted; }
    QString             name() const                { return _info.name(); }
    void setColor(const QColor &color)              { _info.setColor(color); }
    void setName(const QString &name)               { _info.setName(name); }

public slots:
    void sendChatMessage(const ClientInfo &, const QString &);
    void sendSurrenderMessage(const ClientInfo &);
    void sendStartGameMessage();
    void sendStopGameMessage();
    void sendTurnMessage(const ClientInfo &, const QString &tile, int id, int x, int y);

    void start(const QString &hostname, quint16 port);
    void stop();

private slots:
    void processSocketDisconnected();
    void processSocketError(QAbstractSocket::SocketError);

    void receiveChatMessage(const ChatMessage &);
    void receiveClientConnectMessage(const ClientConnectMessage &);
    void receiveClientDisconnectMessage(const ClientDisconnectMessage &);
    void receiveConnectionAcceptedMessage(const ConnectionAcceptedMessage &);
    void receiveErrorMessage(const ErrorMessage &);
    void receivePingMessage(const PingMessage &);
    void receivePlayersListMessage(const PlayersListMessage &);
    void receiveStartGameMessage(const StartGameMessage &);
    void receiveServerReadyMessage(const ServerReadyMessage &);
    void receiveSurrenderMessage(const SurrenderMessage &);
    void receiveTurnMessage(const TurnMessage &);

    void timeout();

signals:
    void connectionAccepted();
    void disconnected();
    void errorOccurred(int);

    void chatMessageReceived(const ClientInfo &, const QString &);
    void clientConnectMessageReceived(const ClientInfo &);
    void clientDisconnectMessageReceived(const ClientInfo &);
    void playersListMessageReceived(QList<ClientInfo>);
    void startGameMessageReceived(QList<ClientInfo>);
    void surrenderMessageReceived(const ClientInfo &);
    void turnMessageReceived(QColor, QString, int, int, int);
};

#endif // LOCALCLIENT_H
