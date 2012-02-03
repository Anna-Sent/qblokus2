#include "localplayer.h"

LocalPlayer::LocalPlayer(const QColor &color,
                         const QString &name,
                         int wid,
                         int hei,
                         QGraphicsItem *parent,
                         QGraphicsScene *scene)
    : Player(color, name, wid, hei, parent, scene)
{
}
