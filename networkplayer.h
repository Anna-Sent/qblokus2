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
    Table *tbl;

public:
    NetworkPlayer(const QColor &color,
                  const QString &name,
                  Table *table,
                  int width = 200,
                  int height = 200,
                  QGraphicsItem *parent = 0,
                  QGraphicsScene *scene = 0);

public slots:
    void turnComplete(QColor color, QString tile, int item, int x, int y);
};

#endif
