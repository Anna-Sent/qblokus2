#include "player.h"
#include <QGraphicsScene>
#include <QPainter>

Player::Player(const ClientInfo &info,
               int width,
               int height,
               QGraphicsItem *parent,
               QGraphicsScene *scene)
    : QGraphicsItem(parent, scene),
    _active(false),
    _lastactive(false),
    _surrendered(false),
    _height(height),
    _width(width),
    _score(0)
{
    _info = info;
    char const *cTiles[] = { "1", "11", "11|01", "111", "11|11", "010|111", "1111", "001|111", "011|110", "1000|1111", "010|010|111", "100|100|111", "0111|1100", "001|111|100", "1|1|1|1|1", "10|11|11", "011|110|100", "11|10|11", "011|110|010", "010|111|010", "0100|1111" };

    vector<string> tiles(cTiles, cTiles + 21);
    _tilesleft = tiles.size();

    int xs = 10;
    int ys = 10;
    int maxheight = 0;
    double dscale = 0.5;
    int realwidth = 0;
    int realheight = 0;

    for (size_t i = 0; i < tiles.size(); ++i)
    {
        ColorItem *item = new ColorItem(tiles[i], _info.color(), i);
        _items.append(item);
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

        if (xs > _width)
        {
            ys += maxheight;
            xs = 10;
            realheight += maxheight;
            maxheight = 0;
        }
    }

    realheight += maxheight;
    _height = realheight;
    _width = realwidth;
}

void Player::activateAll()
{
    for (int i = 0; i < _items.size(); ++i)
    {
        _items[i]->activate();
    }

    _active = true;
    update();
}

void Player::deactivateAll()
{
    for (int i = 0; i < _items.size(); ++i)
    {
        _items[i]->deactivate();
    }

    _active = false;
    update();
}

void Player::paint(QPainter *painter, const QStyleOptionGraphicsItem *style, QWidget *widget)
{
    Q_UNUSED(style);
    Q_UNUSED(widget);

    if (_active != _lastactive)
    {
        if (_active)
        {
            scene()->setBackgroundBrush(QBrush(_info.color().light(180)));
        }
        else
        {
            scene()->setBackgroundBrush(Qt::NoBrush);
        }

        _lastactive = _active;
    }

    if (_surrendered)
    {
        painter->drawText(boundingRect(), Qt::AlignCenter, QString::fromUtf8("I gave up"));
    }

    if (_tilesleft == 0)
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

    --_tilesleft;
    _items[item]->hide();
    Tile *tile = _items[item];
    _score += tile->score();
    emit scoreChanged(_score);
    deactivateAll();
}

void Player::startTurn()
{
    if (_surrendered || _tilesleft == 0)
    {
        return;
    }

    activateAll();
}

void Player::surrender()
{
    //if (_active)
    {
        _surrendered = true;
        deactivateAll();
    }
}
