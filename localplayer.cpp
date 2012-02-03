#include "localplayer.h"

LocalPlayer::LocalPlayer(const ClientInfo &info,
                         int width,
                         int height,
                         QGraphicsItem *parent,
                         QGraphicsScene *scene)
    : Player(info, width, height, parent, scene)
{
}
