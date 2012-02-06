#include "serverssearcher.h"
#include "messagereceiver.h"
#include "constants.h"

ServersSearcher::ServersSearcher()
{
    messageReceiver = new UdpMessageReceiver(&socket);
    connect(messageReceiver, SIGNAL(serverInfoMessageReceived(ServerInfoMessage, const QHostAddress &, quint16)), this, SLOT(receiveServerInfoMessage(ServerInfoMessage, const QHostAddress &, quint16)));

    timer.setInterval(1000);
    connect(&timer, SIGNAL(timeout()), this, SLOT(timeout()));

    connect(this, SIGNAL(destroyed()), messageReceiver, SLOT(deleteLater()));
}

void ServersSearcher::receiveServerInfoMessage(ServerInfoMessage msg, const QHostAddress &host, quint16 port)
{
    Q_UNUSED(port);
    emit serverInfoMessageReceived(host, msg.list());
}

void ServersSearcher::timeout()
{
    ServerRequestMessage msg;
    msg.send(&socket, QHostAddress::Broadcast, port);
}

bool ServersSearcher::isActive() const
{
    return timer.isActive();
}

void ServersSearcher::setPort(int port)
{
    this->port = port;
}

void ServersSearcher::start()
{
    //if (!timer.isActive())
    {
        socket.bind();
        timer.start();
    }
}

void ServersSearcher::stop()
{
    //if (timer.isActive())
    {
        timer.stop();
        socket.close();
    }
}
