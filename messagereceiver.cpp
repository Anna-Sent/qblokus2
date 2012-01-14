#include "messagereceiver.h"
#include <QUdpSocket>

MessageReceiver::MessageReceiver(QAbstractSocket *socket)
{
    current = new MessageHeader;
    this->socket = socket;
    if (dynamic_cast<QUdpSocket *>(socket))
    {
        connect(socket, SIGNAL(readyRead()), this, SLOT(readyReadUdp()));
    }
    else
    {
        connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    }
}

MessageReceiver::~MessageReceiver()
{
    socket->deleteLater();
}

void MessageReceiver::readyRead()
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
    processData(QHostAddress::Any, 0);
}

void MessageReceiver::readyReadUdp()
{
    if (QUdpSocket *udpSocket = dynamic_cast<QUdpSocket *>(socket))
    {
        while (udpSocket->hasPendingDatagrams())
        {
            QByteArray datagram;
            datagram.resize(udpSocket->pendingDatagramSize());
            QHostAddress sender;
            quint16 senderPort;
            int len = udpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
            if (len < 0)
            {
                qDebug() << "UDP socket error occurs: " << udpSocket->errorString();
                return;
            }

            buffer.append(datagram);
            processData(sender, senderPort);
        }
    }
}

void MessageReceiver::processData(const QHostAddress &host, quint16 port)
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
            ServerInfoMessage *msg;
            if ((msg = dynamic_cast<ServerInfoMessage *>(current)))
                emit serverInfoMessageReceive(*msg, host, port);
        }
        {
            ServerReadyMessage *msg;
            if ((msg = dynamic_cast<ServerReadyMessage *>(current)))
                emit serverReadyMessageReceive(*msg);
        }
        {
            ServerRequestMessage *msg;
            if ((msg = dynamic_cast<ServerRequestMessage *>(current)))
                emit serverRequestMessageReceive(*msg, host, port);
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

