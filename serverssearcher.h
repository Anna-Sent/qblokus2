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
    void timeout();
    void readFromSocket();

public slots:
    bool isActive() const {return timer.isActive();}
    void start() {socket.bind();timer.start();}
    void stop() {timer.stop();socket.close();}
    void setPort(int port) {this->port = port;}
};

#endif // SERVERSSEARCHER_H
