#ifndef CLIENTINFO_H
#define CLIENTINFO_H

#include <QColor>
#include <QString>
#include <QMetaType>

class ClientInfo {
public:
    QString name;
    QColor color;
    QByteArray serialize() const;
    void fill(const char*);
    int size() const;
    ClientInfo():name(""),color(Qt::black){}
    ClientInfo(const ClientInfo &other) {name = other.name;color = other.color;}
    ~ClientInfo() {}
};

Q_DECLARE_METATYPE(ClientInfo)
Q_DECLARE_METATYPE(QList<ClientInfo>)

QDataStream &operator<<(QDataStream &out, const ClientInfo& ci);
QDataStream &operator>>(QDataStream &in, ClientInfo& ci);

#endif
