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
    void retirePlayer(int); // ?

private slots:
    void winner(Player*); // ?

public:
    Game(QWidget *widget);
    bool isStarted() const;
    void remotePlayerRetired(QString name, QColor color); // ?
    void updatePlayers(QList<ClientInfo>, QList<bool>);

public slots:
    void addPlayer(ClientInfo info, PlayerType type);
    void clear();
    void playerRetired(); // ?
    void start();
    void turnComplete(QColor color, QString tile, int id, int x, int y);

signals:
    void gameOver(QList<ClientInfo>, int);
    void playerRetired(QString name, QColor color);
    void turnCompleted(QString name, QColor color, QString mask, int id, int x, int y);
};

#endif
