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
    mtRestartGame,
    mtTurn,
    mtSurrender
};

class Message
{
protected:
public:
    virtual qint64 length() const = 0;
    virtual Message *next() const;
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
    MessageHeader() { }
    qint64 length() const { return sizeof(MessageType) + sizeof(qint64); }
    qint64 msgLength() const { return _msgLen; }
    Message *next() const;
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
    QByteArray serialize() const { return _header.serialize(); }
    void fill(const QByteArray &) { }
};

class ChatMessage : public ComplexMessage
{
private:
    QString _text;
    ClientInfo _info;
public:
    ChatMessage() { }
    ChatMessage(const MessageHeader &header) { _header = header; }
    ChatMessage(QString name, QString text, QColor color);
    QColor color() const { return _info.color(); }
    QString name() const { return _info.name(); }
    QByteArray serialize() const;
    QString text() const { return _text; }
    void fill(const QByteArray &);
};

class PlayersListMessage : public ComplexMessage
{
protected:
    QList<ClientInfo> _list;
public:
    PlayersListMessage() { }
    PlayersListMessage(const MessageHeader &header) { _header = header; }
    PlayersListMessage(QList<ClientInfo>);
    QList<ClientInfo> list() const { return _list; }
    QByteArray serialize() const;
    void fill(const QByteArray &);
};

class ServerInfoMessage : public PlayersListMessage
{
private:
    QString _address;
public:
    ServerInfoMessage() { }
    ServerInfoMessage(const MessageHeader &header) { _header = header; }
    ServerInfoMessage(QString, QList<ClientInfo>);
    QString address() const { return _address; }
    QByteArray serialize() const;
    void fill(const QByteArray &);
};

class RestartGameMessage : public PlayersListMessage
{
public:
    RestartGameMessage() { }
    RestartGameMessage(const MessageHeader &header) { _header = header; }
    RestartGameMessage(QList<ClientInfo>);
};

class ClientMessage : public ComplexMessage
{
protected:
    ClientInfo _info;
public:
    QColor color() const { return _info.color(); }
    QString name() const { return _info.name(); }
    virtual QByteArray serialize() const;
    virtual void fill(const QByteArray &);
};

class TryToConnectMessage : public ClientMessage
{
public:
    TryToConnectMessage(const MessageHeader &header) { _header = header; }
    TryToConnectMessage(ClientInfo);
};

class ClientConnectMessage : public ClientMessage
{
public:
    ClientConnectMessage() { }
    ClientConnectMessage(const MessageHeader &header) { _header = header; }
    ClientConnectMessage(QString, QColor);
};

class ClientDisconnectMessage : public ClientMessage
{
public:
    ClientDisconnectMessage() { }
    ClientDisconnectMessage(const MessageHeader &header) { _header = header; }
    ClientDisconnectMessage(QString, QColor);
};

class ServerReadyMessage : public ComplexMessage
{
public:
    ServerReadyMessage() { _header.setMsgLength(0); _header.setMsgType(mtServerReady); }
    ServerReadyMessage(const MessageHeader &header) { _header = header; }
};

class PingMessage : public ComplexMessage
{
public:
    PingMessage() { _header.setMsgLength(0); _header.setMsgType(mtPing); }
    PingMessage(const MessageHeader &header) { _header = header; }
};

class ConnectionAcceptedMessage : public ComplexMessage
{
private:
    int _errorCode;
public:
    ConnectionAcceptedMessage() { }
    ConnectionAcceptedMessage(const MessageHeader &header) { _header = header; }
    ConnectionAcceptedMessage(int errorCode);
    int errorCode() const { return _errorCode; }
    QByteArray serialize() const;
    void fill(const QByteArray &);
};

class ServerRequestMessage : public ComplexMessage
{
public:
    ServerRequestMessage() { _header.setMsgLength(0); _header.setMsgType(mtServerRequest); }
    ServerRequestMessage(const MessageHeader &header) { _header = header; }
};

class StartGameMessage : public ComplexMessage
{
public:
    StartGameMessage() { _header.setMsgLength(0); _header.setMsgType(mtStartGame); }
    StartGameMessage(const MessageHeader &header) { _header = header; }
};

class TurnMessage : public ClientMessage
{
private:
    int _id, _x, _y;
    QString _mask;
public:
    TurnMessage() { }
    TurnMessage(const MessageHeader &header) { _header = header; }
    TurnMessage(QString, QColor, QString, int id, int x, int y);
    int id() const { return _id; }
    int x() const { return _x; }
    int y() const { return _y; }
    QString mask() const { return _mask; }
    QByteArray serialize() const;
    void fill(const QByteArray &);
};

class SurrenderMessage : public ClientMessage
{
public:
    SurrenderMessage() { }
    SurrenderMessage(const MessageHeader &header) { _header = header; }
    SurrenderMessage(QString name, QColor color);
};

#endif // MESSAGE_H
