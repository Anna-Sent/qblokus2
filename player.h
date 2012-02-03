#ifndef PLAYER_H_
#define PLAYER_H_

#include "clientinfo.h"
#include "coloritem.h"

class Player : public QObject, public QGraphicsItem
{
    Q_INTERFACES(QGraphicsItem);
    Q_OBJECT

protected:
    bool active;
    ClientInfo _info;
    virtual void activateAll();
    virtual void deactivateAll();
    friend class Game;

private:
    bool lastactive;
    QList<ColorItem *> items;
    int height, width;
    int _score;
    bool surrendered;
    int tilesleft;

public:
    Player(const QColor &color, const QString &name, int width = 300, int height = 200, QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);
    ~Player();
    QRectF boundingRect() const;
    QColor color() const { return _info.color(); }
    const ClientInfo &info() const { return _info; }
    bool isSurrendered() const;
    QString name() const { return _info.name(); }
    int score() const { return _score; }
    ColorItem *tile(int i) const { return items[i]; }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

public slots:
    virtual void startTurn();
    virtual void surrender();
    virtual void turnComplete(QColor color, QString tile, int item, int x, int y);

signals:
    void scoreChanged(int score);
    void turnCompleted();
    void won(Player *);
};

#endif
