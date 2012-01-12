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
    MessageReceiver *messageReceiver;
    QUdpSocket socket;
    QTimer timer;
    int port;

signals:
    void serverInfoMessageReceive(QString, QList<ClientInfo>);

private slots:
    void serverInfoMessageReceive(ServerInfoMessage);
    void timeout();

public slots:
    bool isActive() const;
    void setPort(int port);
    void start();
    void stop();
};

#endif // SERVERSSEARCHER_H
