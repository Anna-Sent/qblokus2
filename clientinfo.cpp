#include "clientinfo.h"

QByteArray ClientInfo::serialize() const
{
    QByteArray result;
    QByteArray tmp = _name.toUtf8();
    int size = tmp.size();
    result.append(QByteArray::fromRawData((const char *)&size, sizeof(int)));
    result.append(tmp);
    result.append(QByteArray::fromRawData((const char *)&_color, sizeof(QColor)));
    return result;
}

int ClientInfo::size() const
{
    return _name.toUtf8().size() + sizeof(int) + sizeof(QColor);
}

void ClientInfo::fill(const char *data)
{
    int size = *((int *)data);
    data += sizeof(int);
    _name = QString::fromUtf8(data, size);
    data += size;
    ::memmove(&_color, data, sizeof(QColor));
}

bool operator==(const ClientInfo &a1,const ClientInfo &a2)
{
    return a1.name() == a2.name() && a1.color() == a2.color();
}

QDataStream &operator<<(QDataStream &out, const ClientInfo &ci)
{
    out << ci.name() << ci.color();
    return out;
}

QDataStream &operator>>(QDataStream &in, ClientInfo &ci)
{
    QString name = ci.name();
    QColor color = ci.color();
    in >> name >> color;
    return in;
}
