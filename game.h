#ifndef GAME_H
#define GAME_H

#include "ui_racingForm.h"
#include "clientinfo.h"
#include "table.h"
#include "player.h"
#include <QObject>
#include <QColor>
#include <QGraphicsScene>

enum PlayerType { ptLocal, ptNetwork };

class Game : public QObject
{
    Q_OBJECT

private:
    QList<Player *> players;
    QList<QGraphicsScene *> scenes;
    Table *table;
    QGraphicsScene *tablescene;
    Ui::MainWindow *ui;
    QWidget *widget;
    int currplayer;
    bool isserver;
    int playersleft;
    bool running;
    void retirePlayer(int);

public:
    Game(QWidget *widget);
    ~Game();
    void clear();
    void remotePlayerRetired(QString name, QColor color);
    void updatePlayers(QList<ClientInfo>, QList<bool>);
    bool isStarted() const;

public slots:
    void addPlayer(QString name, QColor color, PlayerType type);
    void playerRetired();
    void start();
    void turnDone(QColor color, QString tile, int id, int x, int y);

private slots:
    void winner(Player*);

signals:
    void gameOver(QString winner, int score, QColor color);
    void playerRetired(QString name, QColor color);
    void turnDone(QString name, QColor color, QString mask, int id, int x, int y);
};

#endif
