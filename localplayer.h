#ifndef LOCALPLAYER_H
#define LOCALPLAYER_H

#include "player.h"

class LocalPlayer : public Player
{
    Q_OBJECT

public:
    LocalPlayer(const ClientInfo &info,
                int wid = 200,
                int hei = 200,
                QGraphicsItem *parent = 0,
                QGraphicsScene *scene = 0);
};

#endif
