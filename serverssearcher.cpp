#include "serverssearcher.h"
#include "messagerecv.h"
#include "constants.h"

ServersSearcher::ServersSearcher()
{
    timer.setInterval(1000);
    connect(&timer, SIGNAL(timeout()), this, SLOT(timeout()));
    connect(&socket, SIGNAL(readyRead()), this, SLOT(readFromSocket()));
}

void ServersSearcher::readFromSocket()
{
    if (socket.hasPendingDatagrams())
    {
        qint64 datagramSize = socket.pendingDatagramSize();
        char *data = (char *)::malloc(datagramSize);
        QHostAddress address;
        quint16 port;
        int res = socket.readDatagram(data, datagramSize, &address, &port);
        MessageHeader header;
        header.setMsgLength(datagramSize - header.length());
        header.setType(mtPlayersList);
        PlayersListMessage msg(header);
        msg.fill(QByteArray::fromRawData(data + header.length(), header.msgLength()));
        emit getServer(address.toString(), msg.list());
        free(data);
    }
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
