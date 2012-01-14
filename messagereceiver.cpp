#include "messagereceiver.h"

TcpMessageReceiver::TcpMessageReceiver(QTcpSocket *socket)
{
    current = new MessageHeader;
    this->socket = socket;
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

TcpMessageReceiver::~TcpMessageReceiver()
{
    // important. do not use delete socket;
    socket->deleteLater();
}

void TcpMessageReceiver::readyRead()
{
    int avail = socket->bytesAvailable();
    char *tmp = (char *)malloc(avail);
    int len = socket->read(tmp, avail);
    if (len < 0)
    {
        qDebug() << "TCP socket error occurs: " << socket->errorString();
        return;
    }

    buffer.append(QByteArray(tmp, len));
    processData();
}

void TcpMessageReceiver::processData()
{
    while (current->length() <= buffer.size())
    {
        current->fill(buffer);
        {
            ChatMessage *msg;
            if ((msg = dynamic_cast<ChatMessage *>(current)))
                emit chatMessageReceive(*msg);
        }
        {
            ClientConnectMessage *msg;
            if ((msg = dynamic_cast<ClientConnectMessage *>(current)))
                emit clientConnectMessageReceive(*msg);
        }
        {
            ClientDisconnectMessage *msg;
            if ((msg = dynamic_cast<ClientDisconnectMessage *>(current)))
                emit clientDisconnectMessageReceive(*msg);
        }
        {
            ConnectionAcceptedMessage *msg;
            if ((msg = dynamic_cast<ConnectionAcceptedMessage *>(current)))
                emit connectionAcceptedMessageReceive(*msg);
        }
        {
            PingMessage *msg;
            if ((msg = dynamic_cast<PingMessage *>(current)))
                emit pingMessageReceive(*msg);
        }
        {
            PlayersListMessage *msg;
            if ((msg = dynamic_cast<PlayersListMessage *>(current)))
                emit playersListMessageReceive(*msg);
        }
        {
            RestartGameMessage *msg;
            if ((msg = dynamic_cast<RestartGameMessage *>(current)))
                emit restartGameMessageReceive(*msg);
        }
        {
            ServerReadyMessage *msg;
            if ((msg = dynamic_cast<ServerReadyMessage *>(current)))
                emit serverReadyMessageReceive(*msg);
        }
        {
            StartGameMessage *msg;
            if ((msg = dynamic_cast<StartGameMessage *>(current)))
                emit startGameMessageReceive(*msg);
        }
        {
            SurrenderMessage *msg;
            if ((msg = dynamic_cast<SurrenderMessage *>(current)))
                emit surrenderMessageReceive(*msg);
        }
        {
            TurnMessage *msg;
            if ((msg = dynamic_cast<TurnMessage *>(current)))
                emit turnMessageReceive(*msg);
        }
        {
            TryToConnectMessage *msg;
            if ((msg = dynamic_cast<TryToConnectMessage *>(current)))
                emit tryToConnectMessageReceive(*msg);
        }

        buffer.remove(0, current->length());
        Message* old = current;
        current = current->next();
        delete old;
    }
}

UdpMessageReceiver::UdpMessageReceiver(QUdpSocket *socket)
{
    this->socket = socket;
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

UdpMessageReceiver::~UdpMessageReceiver()
{
    // important. do not use delete socket;
    socket->deleteLater();
}

void UdpMessageReceiver::readyRead()
{
    while (socket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(socket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        int len = socket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
        if (len < 0)
        {
            qDebug() << "UDP socket error occurs: " << socket->errorString();
            return;
        }

        processData(datagram, sender, senderPort);
    }
}

void UdpMessageReceiver::processData(QByteArray &buffer, const QHostAddress &host, quint16 port)
{
    Message *current = new MessageHeader;
    while (current->length() <= buffer.size())
    {
        current->fill(buffer);
        {
            ServerInfoMessage *msg;
            if ((msg = dynamic_cast<ServerInfoMessage *>(current)))
                emit serverInfoMessageReceive(*msg, host, port);
        }
        {
            ServerRequestMessage *msg;
            if ((msg = dynamic_cast<ServerRequestMessage *>(current)))
                emit serverRequestMessageReceive(*msg, host, port);
        }

        buffer.remove(0, current->length());
        Message* old = current;
        current = current->next();
        delete old;
    }
}
