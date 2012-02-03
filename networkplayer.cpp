#include "networkplayer.h"

NetworkPlayer::NetworkPlayer(const QColor &color,
                             const QString &name,
                             Table *table,
                             int wid,
                             int hei,
                             QGraphicsItem *parent,
                             QGraphicsScene *scene)
    : Player(color, name, wid, hei, parent, scene), tbl(table)
{
}

void NetworkPlayer::activateAll()
{
    active = true;
    update();
}

void NetworkPlayer::deactivateAll()
{
    active = false;
    update();
}

void NetworkPlayer::turnComplete(QColor color, QString tile, int item, int x, int y)
{
    if (color != _info.color())
    {
        return;
    }

    Tile til(tile.toStdString());
    Player::turnComplete(color, tile, item, x, y);
    tbl->accept(x, y, til, item, true, false, color);
}
