#include "messagereceiver.h"
#include <strings.h>
#include <QHostAddress>
#include <QUdpSocket>

MessageReceiver::MessageReceiver(QAbstractSocket *socket)
{
    current = new MessageHeader;
    this->socket = socket;
    if (QUdpSocket *udpSocket = dynamic_cast<QUdpSocket *>(socket))
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
    processData();
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
            processData();
        }
    }
}

void MessageReceiver::processData()
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
                emit serverInfoMessageReceive(*msg);
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

Message *Message::next() const
{
    return new MessageHeader;
}

void Message::send(QAbstractSocket *socket) const
{
    QByteArray data = serialize();
    socket->write(data.data(), data.size());
}

Message *MessageHeader::next() const
{
    switch(_msgType)
    {
    case mtHeader :                 return new MessageHeader;
    case mtChat :                   return new ChatMessage(*this);
    case mtPlayersList :            return new PlayersListMessage(*this);
    case mtClientConnect :          return new ClientConnectMessage(*this);
    case mtClientDisconnect :       return new ClientDisconnectMessage(*this);
    case mtServerReady :            return new ServerReadyMessage(*this);
    case mtServerInfo :             return new ServerInfoMessage(*this);
    case mtConnectionAccepted :     return new ConnectionAcceptedMessage(*this);
    case mtPing :                   return new PingMessage(*this);
    case mtTryToConnect :           return new TryToConnectMessage(*this);
    case mtStartGame :              return new StartGameMessage(*this);
    case mtRestartGame :            return new RestartGameMessage(*this);
    case mtTurn :                   return new TurnMessage(*this);
    case mtSurrender :              return new SurrenderMessage(*this);
    default :                       return NULL;
    }
}

QByteArray MessageHeader::serialize() const
{
    QByteArray result;
    result.append(QByteArray::fromRawData((const char *)&_msgType, sizeof(MessageType)));
    result.append(QByteArray::fromRawData((const char *)&_msgLen, sizeof(qint64)));
    return result;
}

void MessageHeader::fill(const QByteArray &buffer)
{
    const char *data = buffer.data();
    ::memmove(&_msgType, data, sizeof(MessageType));
    data += sizeof(MessageType);
    ::memmove(&_msgLen, data, sizeof(qint64));
}

ChatMessage::ChatMessage(QString name, QString text, QColor color) {
    _info.setName(name);
    _info.setColor(color);
    _header.setMsgLength(_info.size() + _text.toUtf8().size() + sizeof(int));
    _header.setMsgType(mtChat);
    _text = text;
}

QByteArray ChatMessage::serialize() const
{
    QByteArray result;
    result.append(_info.serialize());
    QByteArray tmp = _text.toUtf8();
    int size = tmp.size();
    result.append(QByteArray::fromRawData((const char *)&size, sizeof(int)));
    result.append(tmp);
    return _header.serialize().append(result);
}

void ChatMessage::fill(const QByteArray &buffer)
{
    const char *data = buffer.data();
    _info.fill(data);
    data += _info.size();
    int textlen = *((int *)data);
    data += sizeof(int);
    _text = QString::fromUtf8(data, textlen);
}

RestartGameMessage::RestartGameMessage(QList<ClientInfo> list)
{
    _list = list;
    qint64 len = sizeof(int);
    for (int i = 0; i < list.size(); len += list[i++].size()) { }
    _header.setMsgLength(len);
    _header.setMsgType(mtRestartGame);
}

PlayersListMessage::PlayersListMessage(QList<ClientInfo> list)
{
    _list = list;
    qint64 len = sizeof(int);
    for (int i = 0; i < list.size(); len += list[i++].size()) { }
    _header.setMsgLength(len);
    _header.setMsgType(mtPlayersList);
}

QByteArray PlayersListMessage::serialize() const
{
    QByteArray result;
    int size = _list.size();
    result.append(QByteArray::fromRawData((const char *)&size, sizeof(int)));
    for (int i = 0; i < _list.size(); result.append(_list[i++].serialize())) { }
    return _header.serialize().append(result);
}

void PlayersListMessage::fill(const QByteArray &buffer)
{
    const char *data = buffer.data();
    int count = *((int *)data);
    data += sizeof(int);
    for (int i = 0; i < count; ++i)
    {
        ClientInfo item;
        item.fill(data);
        data += item.size();
        _list.append(item);
    }
}

ServerInfoMessage::ServerInfoMessage(QString address, QList<ClientInfo> list)
{
    _address = address;
    _list = list;
    qint64 len = sizeof(int) * 2 + address.toUtf8().size();
    for (int i = 0; i < list.size(); len += list[i++].size()) { }
    _header.setMsgLength(len);
    _header.setMsgType(mtServerInfo);
}

QByteArray ServerInfoMessage::serialize() const
{
    QByteArray result;
    QByteArray tmp = _address.toUtf8();
    int size = tmp.size();
    result.append(QByteArray::fromRawData((const char *)&size, sizeof(int)));
    result.append(tmp);
    size = _list.size();
    result.append(QByteArray::fromRawData((const char *)&size, sizeof(int)));
    for (int i = 0; i < _list.size(); result.append(_list[i++].serialize())) { }
    return _header.serialize().append(result);
}

void ServerInfoMessage::fill(const QByteArray &buffer)
{
    const char *data = buffer.data();
    int len = *((int *)data);
    data += sizeof(int);
    _address = QString::fromUtf8(data, len);
    data += len;
    len = *((int *)data);
    data += sizeof(int);
    for (int i = 0; i < len; ++i)
    {
        ClientInfo item;
        item.fill(data);
        data += item.size();
        _list.append(item);
    }
}

QByteArray ClientMessage::serialize() const
{
    QByteArray result;
    result.append(_info.serialize());
    return _header.serialize().append(result);
}

void ClientMessage::fill(const QByteArray &buffer)
{
    const char *data = buffer.data();
    _info.fill(data);
}

ClientConnectMessage::ClientConnectMessage(QString name, QColor color)
{
    _info.setName(name);
    _info.setColor(color);
    _header.setMsgLength(_info.size());
    _header.setMsgType(mtClientConnect);
}

ClientDisconnectMessage::ClientDisconnectMessage(QString name, QColor color)
{
    _info.setName(name);
    _info.setColor(color);
    _header.setMsgLength(_info.size());
    _header.setMsgType(mtClientDisconnect);
}

TurnMessage::TurnMessage(QString name, QColor color, QString tile, int id, int x, int y)
{
    _info.setName(name);
    _info.setColor(color);
    _id = id;
    _x = x;
    _y = y;
    _mask = tile;
    _header.setMsgLength(_info.size() + 4 * sizeof(int) + _mask.toUtf8().size());
    _header.setMsgType(mtTurn);
}

QByteArray TurnMessage::serialize() const
{
    QByteArray result = _info.serialize();
    result.append(QByteArray::fromRawData((const char *)&_id, sizeof(int)));
    result.append(QByteArray::fromRawData((const char *)&_x, sizeof(int)));
    result.append(QByteArray::fromRawData((const char *)&_y, sizeof(int)));
    QByteArray tmp = _mask.toUtf8();
    int size = tmp.size();
    result.append(QByteArray::fromRawData((const char *)&size, sizeof(int)));
    result.append(tmp);
    return _header.serialize().append(result);
}

void TurnMessage::fill(const QByteArray &buffer)
{
    const char *data = buffer.data();
    _info.fill(data);
    data += _info.size();
    _id = *((int *)data);
    data += sizeof(int);
    _x = *((int *)data);
    data += sizeof(int);
    _y = *((int *)data);
    data += sizeof(int);
    int textlen = *((int *)data);
    data += sizeof(int);
    _mask = QString::fromUtf8(data, textlen);
}

TryToConnectMessage::TryToConnectMessage(ClientInfo info)
{
    _info = info;
    _header.setMsgLength(_info.size());
    _header.setMsgType(mtTryToConnect);
}

ConnectionAcceptedMessage::ConnectionAcceptedMessage(int errorCode)
{
    _errorCode = errorCode;
    _header.setMsgLength(sizeof(int));
    _header.setMsgType(mtConnectionAccepted);
}

QByteArray ConnectionAcceptedMessage::serialize() const
{
    QByteArray result = QByteArray::fromRawData((const char *)(&_errorCode), sizeof(int));
    return _header.serialize().append(result);
}

void ConnectionAcceptedMessage::fill(const QByteArray &buffer)
{
    const char *data = buffer.data();
    ::memmove((void *)(&_errorCode), data, sizeof(int));
}

SurrenderMessage::SurrenderMessage(QString name, QColor color)
{
    _info.setName(name);
    _info.setColor(color);
    _header.setMsgLength(_info.size());
    _header.setMsgType(mtSurrender);
}
