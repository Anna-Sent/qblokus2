#ifndef SERVERSEARCHER_H
#define SERVERSEARCHER_H

#include "clientinfo.h"
#include "messagereceiver.h"
#include <QObject>
#include <QTimer>
#include <QUdpSocket>

class ServerSearcher : public QObject
{
    Q_OBJECT

private:
    UdpMessageReceiver *messageReceiver;
    int port;
    QUdpSocket socket;
    QTimer timer;

private slots:
    void receiveServerInfoMessage(ServerInfoMessage, const QHostAddress &, quint16);
    void timeout();

public:
    explicit ServerSearcher();

public slots:
    bool isActive() const;
    void setPort(int port);
    void start();
    void stop();

signals:
    void serverInfoMessageReceived(const QHostAddress &, QList<ClientInfo>);
};

#endif // SERVERSEARCHER_H
