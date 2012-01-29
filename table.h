#ifndef TABLE_H
#define TABLE_H

#include <QGraphicsItem>

using namespace std;

class TableCell : public QGraphicsItem
{
private:
    int xpos, ypos;

protected:
    QColor color;
    bool dragOver;
    bool highlight;
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);
    void dropEvent(QGraphicsSceneDragDropEvent *event);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;
    friend class Table;

public:
    TableCell(QGraphicsItem *parent = 0, int x = 0, int y = 0);
};

class Tile;

class Table : public QObject, public QGraphicsItem
{
    Q_INTERFACES(QGraphicsItem);
    Q_OBJECT

private:
    int width, height;

protected:
    vector<vector<TableCell *> > cells;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

public:
    Table(int w, int h);
    bool Accept(int x, int y, const Tile &what, int id, bool really, bool local, QColor color);
    void clearDrags();
    QRectF boundingRect() const;

signals:
    void turnComplete(QColor color, QString mask, int id, int x, int y);
};

#endif
