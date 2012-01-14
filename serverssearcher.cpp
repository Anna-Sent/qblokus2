#include "serverssearcher.h"
#include "messagereceiver.h"
#include "constants.h"

ServersSearcher::ServersSearcher()
{
    messageReceiver = new MessageReceiver(&socket);
    connect(messageReceiver, SIGNAL(serverInfoMessageReceive(ServerInfoMessage, const QHostAddress &, quint16)), this, SLOT(serverInfoMessageReceive(ServerInfoMessage, const QHostAddress &, quint16)));
    timer.setInterval(1000);
    connect(&timer, SIGNAL(timeout()), this, SLOT(timeout()));
}

ServersSearcher::~ServersSearcher()
{
    messageReceiver->deleteLater();
}

void ServersSearcher::serverInfoMessageReceive(ServerInfoMessage msg, const QHostAddress &host, quint16 port)
{
    emit serverInfoMessageReceive(host, msg.list());
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
    socket.bind();
    timer.start();
}

void ServersSearcher::stop()
{
    timer.stop();
    socket.close();
}
