#ifndef NETWORKPLAYER_H
#define NETWORKPLAYER_H

#include "player.h"
#include "table.h"

class NetworkPlayer : public Player
{
    Q_OBJECT

private:
    Table *tbl;

protected:
    void activateAll() { active = true; update(); }
    void deactivateAll() { active = false; update(); }

public:
    NetworkPlayer(QColor clr, Table *table, int wid = 200, int hei = 200, QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);

public slots:
    void turnComplete(QColor color, QString tile, int item, int x, int y);
};

#endif
