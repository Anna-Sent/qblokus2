#include "networkplayer.h"

NetworkPlayer::NetworkPlayer(const ClientInfo &info,
                             Table *table,
                             int width,
                             int height,
                             QGraphicsItem *parent,
                             QGraphicsScene *scene)
    : Player(info, width, height, parent, scene), _table(table)
{
}

void NetworkPlayer::activateAll()
{
    _active = true;
    update();
}

void NetworkPlayer::deactivateAll()
{
    _active = false;
    update();
}

void NetworkPlayer::completeTurn(QColor color, QString tile, int item, int x, int y)
{
    if (color != _info.color())
    {
        return;
    }

    Player::completeTurn(color, tile, item, x, y);
    Tile t(tile.toStdString());
    _table->accept(x, y, t, item, true, false, color);
}
