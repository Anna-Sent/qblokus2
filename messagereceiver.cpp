#include "messagereceiver.h"

Message *next(Message *current)
{
    MessageHeader *header = dynamic_cast<MessageHeader *>(current);
    if (header)
    {
        switch(header->msgType())
        {
        case mtHeader :                 return new MessageHeader;
        case mtChat :                   return new ChatMessage(*header);
        case mtClientConnect :          return new ClientConnectMessage(*header);
        case mtClientDisconnect :       return new ClientDisconnectMessage(*header);
        case mtConnectionAccepted :     return new ConnectionAcceptedMessage(*header);
        case mtServerInfo :             return new ServerInfoMessage(*header);
        case mtServerReady :            return new ServerReadyMessage(*header);
        case mtServerRequest :          return new ServerRequestMessage(*header);
        case mtServerStartGame :        return new ServerStartGameMessage(*header);
        case mtServerStopGame :         return new ServerStopGameMessage(*header);
        case mtStartGame :              return new StartGameMessage(*header);
        case mtSurrender :              return new SurrenderMessage(*header);
        case mtPing :                   return new PingMessage(*header);
        case mtPlayersList :            return new PlayersListMessage(*header);
        case mtTryToConnect :           return new TryToConnectMessage(*header);
        case mtTurn :                   return new TurnMessage(*header);
        default :                       return NULL;
        }
    }
    else
    {
        return new MessageHeader;
    }
}

TcpMessageReceiver::TcpMessageReceiver(QTcpSocket *socket, QObject *parent) : QObject(parent)
{
    current = new MessageHeader;
    this->socket = socket;
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
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

#define DISPATCH_MESSAGE(signal, type) if (type *msg = dynamic_cast<type *>(current)) emit signal(*msg);

void TcpMessageReceiver::processData()
{
    while (current->length() <= buffer.size())
    {
        current->fill(buffer);
        switch (current->type())
        {
        case mtChat:                DISPATCH_MESSAGE(chatMessageReceived, ChatMessage); break;
        case mtClientConnect:       DISPATCH_MESSAGE(clientConnectMessageReceived, ClientConnectMessage); break;
        case mtClientDisconnect:    DISPATCH_MESSAGE(clientDisconnectMessageReceived, ClientDisconnectMessage); break;
        case mtConnectionAccepted:  DISPATCH_MESSAGE(connectionAcceptedMessageReceived, ConnectionAcceptedMessage); break;
        case mtPing:                DISPATCH_MESSAGE(pingMessageReceived, PingMessage); break;
        case mtPlayersList:         DISPATCH_MESSAGE(playersListMessageReceived, PlayersListMessage); break;
        case mtServerReady:         DISPATCH_MESSAGE(serverReadyMessageReceived, ServerReadyMessage); break;
        case mtServerStartGame:     DISPATCH_MESSAGE(serverStartGameMessageReceived, ServerStartGameMessage); break;
        case mtServerStopGame:      DISPATCH_MESSAGE(serverStopGameMessageReceived, ServerStopGameMessage); break;
        case mtStartGame:           DISPATCH_MESSAGE(startGameMessageReceived, StartGameMessage); break;
        case mtSurrender:           DISPATCH_MESSAGE(surrenderMessageReceived, SurrenderMessage); break;
        case mtTurn:                DISPATCH_MESSAGE(turnMessageReceived, TurnMessage); break;
        case mtTryToConnect:        DISPATCH_MESSAGE(tryToConnectMessageReceived, TryToConnectMessage); break;
        default: ;
        }

        buffer.remove(0, current->length());
        Message* old = current;
        current = next(current);
        delete old;
    }
}

UdpMessageReceiver::UdpMessageReceiver(QUdpSocket *socket, QObject *parent) : QObject(parent)
{
    this->socket = socket;
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
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

#define DISPATCH_UDP_MESSAGE(signal, type) if (type *msg = dynamic_cast<type *>(current)) emit signal(*msg, host, port);

void UdpMessageReceiver::processData(QByteArray &buffer, const QHostAddress &host, quint16 port)
{
    Message *current = new MessageHeader;
    while (current->length() <= buffer.size())
    {
        current->fill(buffer);
        switch (current->type())
        {
        case mtServerInfo:      DISPATCH_UDP_MESSAGE(serverInfoMessageReceived, ServerInfoMessage); break;
        case mtServerRequest:   DISPATCH_UDP_MESSAGE(serverRequestMessageReceived, ServerRequestMessage); break;
        default: ;
        }

        buffer.remove(0, current->length());
        Message* old = current;
        current = next(current);
        delete old;
    }
}
