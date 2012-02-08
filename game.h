#ifndef GAME_H
#define GAME_H

#include <QObject>
#include <QColor>
#include <QGraphicsScene>
#include "clientinfo.h"
#include "player.h"
#include "table.h"
#include "ui_racingForm.h"

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
    QWidget *_widget;
    QGraphicsScene *_tablescene;
    QList<Player *> _players;
    QList<QGraphicsScene *> _scenes;
    void countNextActivePlayerNumber();
    void retirePlayer(int);

private slots:
    void winner(Player *);

public:
    Game(QWidget *widget);
    bool isStarted() const { return _running; }
    void updatePlayers(QList<ClientInfo>, QList<bool>);

public slots:
    void addPlayer(ClientInfo info, PlayerType type);
    void clear();
    void retirePlayer(QString name, QColor color);
    void start();
    void turnComplete(QColor color, QString tile, int id, int x, int y);

signals:
    void gameOver(QList<ClientInfo>, int);
    void turnCompleted(QString name, QColor color, QString mask, int id, int x, int y);
    void turnStarted(const ClientInfo &);
};

#endif
