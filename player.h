#ifndef PLAYER_H_
#define PLAYER_H_

#include "clientinfo.h"
#include "coloritem.h"

class Player : public QObject, public QGraphicsItem
{
    Q_INTERFACES(QGraphicsItem);
    Q_OBJECT

protected:
    bool _active;
    ClientInfo _info;
    virtual void activateAll();
    virtual void deactivateAll();
    friend class Game;

private:
    bool _lastactive;
    bool _surrendered;
    int _height;
    int _width;
    int _score;
    int _tilesleft;
    QList<ColorItem *> _items;

public:
    Player(const ClientInfo &info,
           int width = 200,
           int height = 200,
           QGraphicsItem *parent = 0,
           QGraphicsScene *scene = 0);
    QRectF boundingRect() const { return QRectF(0, 0, _width, _height); }
    QColor color() const { return _info.color(); }
    const ClientInfo &info() const { return _info; }
    bool isSurrendered() const { return _surrendered; }
    int leftTilesCount() const { return _tilesleft; }
    QString name() const { return _info.name(); }
    int score() const { return _score; }
    ColorItem *tile(int i) const { return _items[i]; }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

public slots:
    virtual void completeTurn(QColor color, QString tile, int item, int x, int y);
    virtual void startTurn();
    virtual void surrender();

signals:
    void scoreChanged(int score);
};

#endif
