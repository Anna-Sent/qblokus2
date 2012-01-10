#include "clientinfo.h"

QDataStream &operator<<(QDataStream &out, const ClientInfo& ci)
{
    out << ci.name << ci.color;
    return out;
}

QDataStream &operator>>(QDataStream &in, ClientInfo& ci)
{
    in >> ci.name >> ci.color;
    return in;
}

int ClientInfo::size() const { return name.toUtf8().size() + sizeof(int) + sizeof(QColor); }

QByteArray ClientInfo::serialize() const {
    QByteArray result;
    QByteArray tmp = name.toUtf8();
    int size = tmp.size();
    result.append(QByteArray::fromRawData((const char*)&size,sizeof(int)));
    result.append(tmp);
    result.append(QByteArray::fromRawData((const char*)&color,sizeof(QColor)));
    return result;
}

void ClientInfo::fill(const char* data) {
    int size = *((int*)data);
    data += sizeof(int);
    name = QString::fromUtf8(data,size);
    data += size;
    ::memmove(&color, data, sizeof(QColor));
}
