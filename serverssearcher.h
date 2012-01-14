#ifndef SERVERSSEARCHER_H
#define SERVERSSEARCHER_H

#include "clientinfo.h"
#include "messagereceiver.h"
#include <QObject>
#include <QtNetwork/QUdpSocket>
#include <QTimer>

class ServersSearcher : public QObject
{
    Q_OBJECT
public:
    explicit ServersSearcher();
    ~ServersSearcher();

private:
    UdpMessageReceiver *messageReceiver;
    int port;
    QUdpSocket socket;
    QTimer timer;

signals:
    void serverInfoMessageReceive(const QHostAddress &, QList<ClientInfo>);

private slots:
    void serverInfoMessageReceive(ServerInfoMessage, const QHostAddress &, quint16);
    void timeout();

public slots:
    bool isActive() const;
    void setPort(int port);
    void start();
    void stop();
};

#endif // SERVERSSEARCHER_H
