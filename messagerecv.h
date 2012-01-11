#ifndef MESSAGERECV_H_
#define MESSAGERECV_H_

#include "clientinfo.h"
#include <QtNetwork/QTcpSocket>

enum MessageType
{
    mtHeader,
    mtChat,
    mtPlayersList,
    mtClientConnect,
    mtClientDisconnect,
    mtServerReady,
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
    MessageType _type;
public:
    virtual qint64 length() const = 0;
    virtual Message *next() const;
    void send(QTcpSocket *) const;
    virtual QByteArray serialize() const = 0;
    virtual void fill(const QByteArray &) = 0;
};

class MessageHeader : public Message
{
private:
    qint64 _len;
public:
    MessageHeader() : _len(0) { _type = mtHeader; }
    qint64 length() const { return sizeof(MessageType) + sizeof(qint64); }
    qint64 msgLength() const { return _len; }
    Message *next() const;
    QByteArray serialize() const;
    void setMsgLength(qint64 value) { _len = value; }
    void setType(MessageType value) { _type = value; }
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
    ClientInfo info() const { return _info; }
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
    ServerReadyMessage(const MessageHeader &header) { _header = header; }
    ServerReadyMessage() { _header.setMsgLength(0); _header.setType(mtServerReady); }
};

class PingMessage : public ComplexMessage
{
public:
    PingMessage(const MessageHeader &header) { _header = header; }
    PingMessage() { _header.setMsgLength(0); _header.setType(mtPing); }
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

class StartGameMessage : public ComplexMessage
{
public:
    StartGameMessage(const MessageHeader &header) { _header = header; }
    StartGameMessage() { _header.setMsgLength(0); _header.setType(mtStartGame);}
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

class MessageReceiver : public QObject
{
    Q_OBJECT

private:
    QByteArray buffer;
    Message *current;
    QTcpSocket *socket;

public:
    MessageReceiver(QTcpSocket *);
    ~MessageReceiver();

public slots:
    void readyRead();

signals:
    void getMessage(QByteArray);

    void chatMessageReceive(ChatMessage);
    void clientConnectMessageReceive(ClientConnectMessage);
    void clientDisconnectMessageReceive(ClientDisconnectMessage);
    void connectionAcceptedMessageReceive(ConnectionAcceptedMessage);
    void pingMessageReceive(PingMessage);
    void playersListMessageReceive(PlayersListMessage);
    void restartGameMessageReceive(RestartGameMessage);
    void serverReadyMessageReceive(ServerReadyMessage);
    void startGameMessageReceive(StartGameMessage);
    void surrenderMessageReceive(SurrenderMessage);
    void tryToConnectMessageReceive(TryToConnectMessage);
    void turnMessageReceive(TurnMessage);
};

#endif
