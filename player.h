#ifndef PLAYER_H_
#define PLAYER_H_

#include "coloritem.h"

class Player : public QObject, public QGraphicsItem
{
    Q_INTERFACES(QGraphicsItem);
    Q_OBJECT

private:
    bool lastactive;

protected:
    QList<ColorItem *> items;
    int height, width;
    bool active;
    QColor color;
    QString name;
    bool surrendered;
    int score;
    int tilesleft;
    virtual void activateAll();
    virtual void deactivateAll();
    friend class Game;

public:
    Player(QColor color, int width = 300, int height = 200, QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);
    ~Player();
    QRectF boundingRect() const;
    QColor getColor() const { return color; }
    const QString &getName() const;
    int getScore() const { return score; }
    bool getSurrendered() const;
    ColorItem *getTile(int i) const { return items[i]; }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setName(const QString &);

public slots:
    virtual void startTurn();
    virtual void turnComplete(QColor color, QString tile, int item, int x, int y);
    virtual void surrender();

signals:
    void turnComplete();
    void scoreChanged(int score);
    void iWin(Player *);
};

#endif
