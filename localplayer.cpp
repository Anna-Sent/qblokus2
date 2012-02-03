#include "localplayer.h"

LocalPlayer::LocalPlayer(const QColor &color,
                         const QString &name,
                         int width,
                         int height,
                         QGraphicsItem *parent,
                         QGraphicsScene *scene)
    : Player(color, name, width, height, parent, scene)
{
}
