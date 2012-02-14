#ifndef GAME_H
#define GAME_H

#include <QObject>
#include <QColor>
#include <QGraphicsScene>
#include "clientinfo.h"
#include "player.h"
#include "table.h"
#include "ui_racingForm.h"

#define MAX_PLAYERS_COUNT 4

enum PlayerType { ptLocal, ptNetwork };

class Game : public QObject
{
    Q_OBJECT

private:
    bool _running;
    int _currplayer;
    int _playersleft;
    Table *_table;
    Ui::MainWindow *_ui;
    QGraphicsView *_gvs[MAX_PLAYERS_COUNT];
    QGraphicsScene *_tablescene;
    QLCDNumber *_lcds[MAX_PLAYERS_COUNT];
    QList<Player *> _players;
    QList<QGraphicsScene *> _scenes;
    void addPlayer(const ClientInfo &info, PlayerType type);
    void countNextActivePlayerNumber();
    void retirePlayer(int);
    void startTurn();
    void winner(Player *);

public:
    Game(QGraphicsView *, QGraphicsView *[MAX_PLAYERS_COUNT], QLCDNumber *[MAX_PLAYERS_COUNT]);
    bool isStarted() const { return _running; }
    void retirePlayer(const ClientInfo &);
    void start();
    void updatePlayers(QList<ClientInfo>, QList<bool>);

public slots:
    void clear();
    void turnComplete(QColor color, QString tile, int id, int x, int y);

signals:
    void gameOver(QList<ClientInfo>, int);
    void turnCompleted(const ClientInfo &, const QString &mask, int id, int x, int y);
    void turnStarted(const ClientInfo &);
};

#endif
