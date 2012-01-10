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
        header.len = datagramSize - header.getLength();
        header.type = mtPlayersList;
        PlayersListMessage msg(header);
        msg.fill(QByteArray::fromRawData(data + header.getLength(), header.len));
        emit getServer(address.toString(), msg.getList());
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
