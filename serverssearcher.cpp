#include "serverssearcher.h"
#include "messagereceiver.h"
#include "constants.h"

ServerSearcher::ServerSearcher()
{
    messageReceiver = new UdpMessageReceiver(&socket);
    connect(messageReceiver, SIGNAL(serverInfoMessageReceived(ServerInfoMessage, const QHostAddress &, quint16)), this, SLOT(receiveServerInfoMessage(ServerInfoMessage, const QHostAddress &, quint16)));

    timer.setInterval(1000);
    connect(&timer, SIGNAL(timeout()), this, SLOT(timeout()));

    connect(this, SIGNAL(destroyed()), messageReceiver, SLOT(deleteLater()));
}

void ServerSearcher::receiveServerInfoMessage(ServerInfoMessage msg, const QHostAddress &host, quint16 port)
{
    Q_UNUSED(port);
    emit serverInfoMessageReceived(host, msg.list());
}

void ServerSearcher::timeout()
{
    ServerRequestMessage msg;
    msg.send(&socket, QHostAddress::Broadcast, port);
}

bool ServerSearcher::isActive() const
{
    return timer.isActive();
}

void ServerSearcher::setPort(int port)
{
    this->port = port;
}

void ServerSearcher::start()
{
    if (!timer.isActive())
    {
        socket.bind();
        timer.start();
    }
}

void ServerSearcher::stop()
{
    if (timer.isActive())
    {
        timer.stop();
        socket.close();
    }
}
