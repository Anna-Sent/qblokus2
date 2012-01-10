#ifndef SERVERSSEARCHER_H
#define SERVERSSEARCHER_H

#include "clientinfo.h"
#include <QObject>
#include <QtNetwork/QUdpSocket>
#include <QTimer>

class ServersSearcher : public QObject
{
    Q_OBJECT
public:
    explicit ServersSearcher();

private:
    QUdpSocket socket;
    QTimer timer;
    int port;

signals:
    void getServer(QString address, QList<ClientInfo> clients);

private slots:
    void readFromSocket();
    void timeout();

public slots:
    bool isActive() const;
    void setPort(int port);
    void start();
    void stop();
};

#endif // SERVERSSEARCHER_H
