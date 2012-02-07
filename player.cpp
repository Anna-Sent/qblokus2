#include "player.h"
#include <QGraphicsScene>
#include <QPainter>

Player::Player(const ClientInfo &info, int wid, int hei, QGraphicsItem *parent, QGraphicsScene *scene)
    : QGraphicsItem(parent, scene), active(false), lastactive(false), height(wid), width(hei), _score(0), surrendered(false)
{
    _info = info;
    char const *cTiles[] = { "1", "11", "11|01", "111", "11|11", "010|111", "1111", "001|111", "011|110", "1000|1111", "010|010|111", "100|100|111", "0111|1100", "001|111|100", "1|1|1|1|1", "10|11|11", "011|110|100", "11|10|11", "011|110|010", "010|111|010", "0100|1111" };

    vector<string> tiles(cTiles, cTiles + 21);
    tilesleft = tiles.size();

    int xs = 10;
    int ys = 10;
    int maxheight = 0;
    double dscale = 0.5;
    int realwidth = 0;
    int realheight = 0;

    for (size_t i = 0; i < tiles.size(); ++i)
    {
        ColorItem *item = new ColorItem(tiles[i], _info.color(), i);
        items.append(item);
        item->setParentItem(this);
        item->setPos(xs, ys);
        int dim = max(item->height(), item->width());
        xs += dim * 20 * dscale + 10;
        int height = (dim * 20 * dscale + 10);
        item->scale(dscale, dscale);
        if (height > maxheight)
        {
            maxheight = height;
        }

        if (xs > realwidth)
        {
            realwidth = xs;
        }

        if (xs > width)
        {
            ys += maxheight;
            xs = 10;
            realheight += maxheight;
            maxheight = 0;
        }
    }

    realheight += maxheight;
    height = realheight;
    width = realwidth;
}

QRectF Player::boundingRect() const 
{
    return QRectF(0, 0, width, height);
}

bool Player::isSurrendered() const
{
    return surrendered;
}

void Player::startTurn()
{
    if (surrendered || tilesleft == 0)
    {
        return;
    }

    activateAll();
}

void Player::deactivateAll()
{
    for (int i = 0; i < items.size(); ++i)
    {
        items[i]->deactivate();
    }

    active = false;
    update();
}

void Player::activateAll()
{
    for (int i = 0; i < items.size(); ++i)
    {
        items[i]->activate();
    }

    active = true;
    update();
}

void Player::paint(QPainter *painter, const QStyleOptionGraphicsItem *style, QWidget *widget)
{
    Q_UNUSED(style);
    Q_UNUSED(widget);

    if (active != lastactive)
    {
        if (active)
        {
            scene()->setBackgroundBrush(QBrush(_info.color().light(180)));
        }
        else
        {
            scene()->setBackgroundBrush(Qt::NoBrush);
        }

        lastactive = active;
    }

    if (surrendered)
    {
        painter->drawText(boundingRect(), Qt::AlignCenter, QString::fromUtf8("I gave up"));
    }

    if (tilesleft == 0)
    {
        painter->drawText(boundingRect(), Qt::AlignCenter, QString::fromUtf8("I won!"));
    }
}

void Player::completeTurn(QColor color, QString, int item, int x, int y)
{
    Q_UNUSED(x);
    Q_UNUSED(y);

    if (color != _info.color())
    {
        return;
    }

    --tilesleft;
    items[item]->hide();
    Tile *tile = items[item];
    _score += tile->score();
    emit scoreChanged(_score);
    deactivateAll();
}

void Player::surrender()
{
    if (active)
    {
        surrendered = true;
        deactivateAll();
    }
}
