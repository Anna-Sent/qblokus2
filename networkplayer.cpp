#include "networkplayer.h"

NetworkPlayer::NetworkPlayer(const QColor &color,
                             const QString &name,
                             Table *table,
                             int width,
                             int height,
                             QGraphicsItem *parent,
                             QGraphicsScene *scene)
    : Player(color, name, width, height, parent, scene), _table(table)
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

    turnComplete(color, tile, item, x, y);
    Tile t(tile.toStdString());
    _table->accept(x, y, t, item, true, false, color);
}
