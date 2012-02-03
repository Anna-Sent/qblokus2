#ifndef COLORITEM_H
#define COLORITEM_H

#include "tile.h"
#include <QGraphicsItem>

class ColorItem : public QGraphicsItem, public Tile
{
private:
    bool _active;
    QColor _color;
    int _id;

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

public:
    ColorItem(string mask, QColor clr, int id);
    QRectF boundingRect() const;
    void activate();
    void deactivate();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

#endif
