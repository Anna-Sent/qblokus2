#ifndef CLIENTINFO_H
#define CLIENTINFO_H

#include <QColor>
#include <QString>
#include <QMetaType>

class ClientInfo {
private:
    QColor _color;
    QString _name;

public:
    ClientInfo() : _color(Qt::black), _name("") { }
    ClientInfo(const ClientInfo &other) { _color = other._color; _name = other._name; }
    ~ClientInfo() { }
    QColor color() const { return _color; }
    QString name() const { return _name; }
    void setColor(const QColor &value) { _color = value; }
    void setName(const QString &value) { _name = value; }
    QByteArray serialize() const;
    int size() const;
    void fill(const char*);
};

Q_DECLARE_METATYPE(ClientInfo)
Q_DECLARE_METATYPE(QList<ClientInfo>)

QDataStream &operator<<(QDataStream &out, const ClientInfo& ci);
QDataStream &operator>>(QDataStream &in, ClientInfo& ci);

#endif
