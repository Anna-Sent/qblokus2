#ifndef TABLE_H
#define TABLE_H

#include <QGraphicsItem>
using namespace std;

class TableCell : public QGraphicsItem
{
public:
    TableCell(QGraphicsItem *parent = 0,int x =0 ,int y=0);

protected:
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);
    void dropEvent(QGraphicsSceneDragDropEvent *event);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;
    QColor color;
    bool dragOver;
    bool highlight;
    friend class Table;
private:
    int xpos,ypos;
};

class Tile;

class Table : public QObject, public QGraphicsItem
{
    Q_INTERFACES(QGraphicsItem);
    Q_OBJECT
public:
    Table(int w,int h);
    bool Accept(int x,int y,const Tile& what,int id,bool really,bool local,QColor color);
    //    bool Accept(int x,int y,Tile what,bool really,QColor color);
    void clearDrags();
    QRectF boundingRect() const;
protected:
    vector<vector<TableCell*> > cells;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option, QWidget *widget);
signals:
    void turnComplete(QColor color,QString mask,int id,int x,int y);
private:
    int width,height;
};

#endif
