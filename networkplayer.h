#ifndef NETWORKPLAYER_H
#define NETWORKPLAYER_H

#include "player.h"
#include "table.h"

class NetworkPlayer : public Player
{
    Q_OBJECT

protected:
    void activateAll();
    void deactivateAll();

private:
    Table *_table;

public:
    NetworkPlayer(const ClientInfo &info,
                  Table *table,
                  int width = 300,
                  int height = 200,
                  QGraphicsItem *parent = 0,
                  QGraphicsScene *scene = 0);

public slots:
    void completeTurn(QColor color, QString tile, int item, int x, int y);
};

#endif
