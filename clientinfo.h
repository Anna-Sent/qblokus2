#ifndef CLIENTINFO_H
#define CLIENTINFO_H

#include <QColor>
#include <QString>

class ClientInfo {
    public:
        QString name;
        QColor color;
        QByteArray serialize() const;
        void fill(const char*);
        int size() const;
        ClientInfo():name(""),color(Qt::black){}
};

#endif
