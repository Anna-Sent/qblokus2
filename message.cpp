#include "message.h"

Message *Message::next() const
{
    return new MessageHeader;
}

void Message::send(QTcpSocket *socket) const
{
    QByteArray data = serialize();
    socket->write(data.data(), data.size());
}

void Message::send(QUdpSocket *socket, const QHostAddress &host, quint16 port) const
{
    QByteArray data = serialize();
    socket->writeDatagram(data, host, port);
}

Message *MessageHeader::next() const
{
    switch(_msgType)
    {
    case mtHeader :                 return new MessageHeader;
    case mtChat :                   return new ChatMessage(*this);
    case mtClientConnect :          return new ClientConnectMessage(*this);
    case mtClientDisconnect :       return new ClientDisconnectMessage(*this);
    case mtConnectionAccepted :     return new ConnectionAcceptedMessage(*this);
    case mtServerInfo :             return new ServerInfoMessage(*this);
    case mtServerReady :            return new ServerReadyMessage(*this);
    case mtServerRequest :          return new ServerRequestMessage(*this);
    case mtSurrender :              return new SurrenderMessage(*this);
    case mtPing :                   return new PingMessage(*this);
    case mtPlayersList :            return new PlayersListMessage(*this);
    case mtRestartGame :            return new RestartGameMessage(*this);
    case mtTryToConnect :           return new TryToConnectMessage(*this);
    case mtTurn :                   return new TurnMessage(*this);
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

ChatMessage::ChatMessage(QString name, QColor color, QString text) {
    _info.setName(name);
    _info.setColor(color);
    _text = text;
    _header.setMsgLength(_info.size() + _text.toUtf8().size() + sizeof(int));
    _header.setMsgType(mtChat);
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

RestartGameMessage::RestartGameMessage(QList<ClientInfo> list)
{
    _list = list;
    qint64 len = sizeof(int);
    for (int i = 0; i < list.size(); len += list[i++].size()) { }
    _header.setMsgLength(len);
    _header.setMsgType(mtRestartGame);
}

ServerInfoMessage::ServerInfoMessage(QList<ClientInfo> list)
{
    _list = list;
    qint64 len = sizeof(int);
    for (int i = 0; i < list.size(); len += list[i++].size()) { }
    _header.setMsgLength(len);
    _header.setMsgType(mtServerInfo);
}

SurrenderMessage::SurrenderMessage(QString name, QColor color)
{
    _info.setName(name);
    _info.setColor(color);
    _header.setMsgLength(_info.size());
    _header.setMsgType(mtSurrender);
}

TryToConnectMessage::TryToConnectMessage(ClientInfo info)
{
    _info = info;
    _header.setMsgLength(_info.size());
    _header.setMsgType(mtTryToConnect);
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
