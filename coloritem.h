#ifndef COLORITEM_H
#define COLORITEM_H

#include "tile.h"
#include <QGraphicsItem>

class ColorItem : public QGraphicsItem, public Tile
{
public:
    ColorItem(string mask, QColor clr, int id);
    void deactivate();
    void activate();
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
#ifdef DEBUG_DESTRUCTORS
    ~ColorItem();
#endif
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * event );
private:
    QColor color;
    int Id;
    bool active;
};

#endif
