#ifndef MESSAGE_H
#define MESSAGE_H

#include "clientinfo.h"
#include <QAbstractSocket>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QColor>

enum MessageType
{
    mtHeader,
    mtChat,
    mtPlayersList,
    mtServerInfo,
    mtClientConnect,
    mtClientDisconnect,
    mtServerReady,
    mtServerRequest,
    mtConnectionAccepted,
    mtPing,
    mtTryToConnect,
    mtStartGame,
    mtTurn,
    mtSurrender
};

class Message
{
public:
    virtual qint64 length() const = 0;
    virtual MessageType type() const = 0;
    void send(QTcpSocket *) const;
    void send(QUdpSocket *, const QHostAddress &, quint16) const;
    virtual QByteArray serialize() const = 0;
    virtual void fill(const QByteArray &) = 0;
};

class MessageHeader : public Message
{
private:
    qint64 _msgLen;
    MessageType _msgType;
public:
    MessageHeader() { _msgLen = 0; }
    qint64 length() const { return sizeof(MessageType) + sizeof(qint64); }
    MessageType type() const { return mtHeader; }
    qint64 msgLength() const { return _msgLen; }
    MessageType msgType() const { return _msgType; }
    QByteArray serialize() const;
    void setMsgLength(qint64 value) { _msgLen = value; }
    void setMsgType(MessageType value) { _msgType = value; }
    void fill(const QByteArray &);
};

class ComplexMessage : public Message
{
protected:
    MessageHeader _header;
public:
    qint64 length() const { return _header.msgLength(); }
    MessageType type() const { return _header.msgType(); }
    QByteArray serialize() const { return _header.serialize(); }
    void fill(const QByteArray &) { }
};

class ClientMessage : public ComplexMessage
{
protected:
    ClientInfo _info;
public:
    ClientMessage() { _header.setMsgLength(_info.size()); }
    QColor color() const { return _info.color(); }
    ClientInfo info() const { return _info; }
    QString name() const { return _info.name(); }
    virtual QByteArray serialize() const;
    virtual void fill(const QByteArray &);
};

class ChatMessage : public ClientMessage
{
private:
    QString _text;
public:
    ChatMessage() { _header.setMsgLength(sizeof(int) + _info.size()); _header.setMsgType(mtChat); }
    ChatMessage(const MessageHeader &header) { _header = header; }
    ChatMessage(const ClientInfo &, const QString &);
    QColor color() const { return _info.color(); }
    QString name() const { return _info.name(); }
    QByteArray serialize() const;
    QString text() const { return _text; }
    void fill(const QByteArray &);
};

class ClientConnectMessage : public ClientMessage
{
public:
    ClientConnectMessage() { _header.setMsgType(mtClientConnect); }
    ClientConnectMessage(const MessageHeader &header) { _header = header; }
    ClientConnectMessage(QString, QColor);
};

class ClientDisconnectMessage : public ClientMessage
{
public:
    ClientDisconnectMessage() { _header.setMsgType(mtClientDisconnect); }
    ClientDisconnectMessage(const MessageHeader &header) { _header = header; }
    ClientDisconnectMessage(QString, QColor);
};

class ConnectionAcceptedMessage : public ComplexMessage
{
private:
    int _errorCode;
public:
    ConnectionAcceptedMessage() { _header.setMsgLength(sizeof(int)); _header.setMsgType(mtConnectionAccepted); }
    ConnectionAcceptedMessage(const MessageHeader &header) { _header = header; }
    ConnectionAcceptedMessage(int errorCode);
    int errorCode() const { return _errorCode; }
    QByteArray serialize() const;
    void fill(const QByteArray &);
};

class PingMessage : public ComplexMessage
{
public:
    PingMessage() { _header.setMsgLength(0); _header.setMsgType(mtPing); }
    PingMessage(const MessageHeader &header) { _header = header; }
};

class PlayersListMessage : public ComplexMessage
{
protected:
    QList<ClientInfo> _list;
public:
    PlayersListMessage() { _header.setMsgLength(sizeof(int)); _header.setMsgType(mtPlayersList); }
    PlayersListMessage(const MessageHeader &header) { _header = header; }
    PlayersListMessage(QList<ClientInfo>);
    QList<ClientInfo> list() const { return _list; }
    QByteArray serialize() const;
    void fill(const QByteArray &);
};

class StartGameMessage : public PlayersListMessage
{
public:
    StartGameMessage() { _header.setMsgType(mtStartGame); }
    StartGameMessage(const MessageHeader &header) { _header = header; }
    StartGameMessage(QList<ClientInfo>);
};

class ServerInfoMessage : public PlayersListMessage
{
public:
    ServerInfoMessage() { _header.setMsgType(mtServerInfo); }
    ServerInfoMessage(const MessageHeader &header) { _header = header; }
    ServerInfoMessage(QList<ClientInfo>);
};

class ServerReadyMessage : public ComplexMessage
{
public:
    ServerReadyMessage() { _header.setMsgLength(0); _header.setMsgType(mtServerReady); }
    ServerReadyMessage(const MessageHeader &header) { _header = header; }
};

class ServerRequestMessage : public ComplexMessage
{
public:
    ServerRequestMessage() { _header.setMsgLength(0); _header.setMsgType(mtServerRequest); }
    ServerRequestMessage(const MessageHeader &header) { _header = header; }
};

class SurrenderMessage : public ClientMessage
{
public:
    SurrenderMessage() { _header.setMsgType(mtSurrender); }
    SurrenderMessage(const MessageHeader &header) { _header = header; }
    SurrenderMessage(const ClientInfo &);
};

class TryToConnectMessage : public ClientMessage
{
public:
    TryToConnectMessage() { _header.setMsgType(mtTryToConnect); }
    TryToConnectMessage(const MessageHeader &header) { _header = header; }
    TryToConnectMessage(ClientInfo);
};

class TurnMessage : public ClientMessage
{
private:
    int _id, _x, _y;
    QString _mask;
public:
    TurnMessage() { _header.setMsgType(mtTurn); }
    TurnMessage(const MessageHeader &header) { _header = header; }
    TurnMessage(const ClientInfo &, const QString &, int id, int x, int y);
    int id() const { return _id; }
    int x() const { return _x; }
    int y() const { return _y; }
    QString mask() const { return _mask; }
    QByteArray serialize() const;
    void fill(const QByteArray &);
};

#endif // MESSAGE_H
