#include "serverssearcher.h"
#include "messagereceiver.h"
#include "constants.h"

ServersSearcher::ServersSearcher()
{
    messageReceiver = new MessageReceiver(&socket);
    connect(messageReceiver, SIGNAL(serverInfoMessageReceive(ServerInfoMessage)), this, SLOT(serverInfoMessageReceive(ServerInfoMessage)));
    timer.setInterval(1000);
    connect(&timer, SIGNAL(timeout()), this, SLOT(timeout()));
}

ServersSearcher::~ServersSearcher()
{
    messageReceiver->deleteLater();
}

void ServersSearcher::serverInfoMessageReceive(ServerInfoMessage msg)
{
    emit serverInfoMessageReceive(msg.address(), msg.list());
}

void ServersSearcher::timeout()
{
    int query = MAGIC_NUMBER;
    socket.writeDatagram((char *)&query, sizeof(query), QHostAddress::Broadcast, port);
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
