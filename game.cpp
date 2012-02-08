#include "game.h"
#include "localplayer.h"
#include "networkplayer.h"
#include <iostream>
#include <QMessageBox>

using namespace std;

Game::Game(QWidget* widget)
{
    _ui = dynamic_cast<Ui::MainWindow *>(widget);
    _table = new Table(20, 20);
    _tablescene = new QGraphicsScene;
    _tablescene->addItem(_table);
    _ui->gvTable->setScene(_tablescene);
    connect(_table, SIGNAL(turnCompleted(QColor,QString,int,int,int)), this, SLOT(turnComplete(QColor,QString,int,int,int)));
    _widget = widget;
    clear();
    connect(this, SIGNAL(destroyed()), this, SLOT(clear()));
    connect(this, SIGNAL(destroyed()), _tablescene, SLOT(deleteLater()));
    connect(this, SIGNAL(destroyed()), _table, SLOT(deleteLater()));
}

void Game::retirePlayer(int i)
{
    Player *player = _players[i];
    player->surrender();
    --_playersleft;
    if (_playersleft > 0)
    {
        if (_running)
        {
            do
            {
                _currplayer = (_currplayer + 1) % _players.size();
            }
            while(_players[_currplayer]->isSurrendered());
        }

        if (_playersleft == 1)
        {
            int msp = 0;
            for (int p = 1; p < _players.size(); ++p)
            {
                if ((_players[p]->score() > _players[msp]->score()))
                {
                    msp = p;
                }
            }

            winner(_players[msp]);
        }
        else if (_running)
        {
            emit turnStarted(_players[_currplayer]->info());
            _players[_currplayer]->startTurn();
        }
    }
}

void Game::winner(Player *winner)
{
    QList<ClientInfo> clients;
    for (int i = 0; i < _players.size(); ++i)
    {
        if (_players[i]->score() == winner->score())
        {
            clients.append(_players[i]->info());
        }
    }

    _running = false;
    emit gameOver(clients, winner->score());
}

void Game::updatePlayers(QList<ClientInfo> clients, QList<bool> local)
{
    if (!_running)
    {
        clear();
        for (int i = 0; i < clients.size(); ++i)
        {
            addPlayer(clients[i], local[i] ? ptLocal : ptNetwork);
        }
    }
    else
    {
        // merge
        int pl = 0, cl = 0;
        while(pl < _players.size() || cl < clients.size())
        {
            if (pl == _players.size() || cl == clients.size())
            {
                if (pl == _players.size())
                {
                    addPlayer(clients[cl], local[cl] ? ptLocal : ptNetwork);
                }

                if (cl == clients.size())
                {
                    for (int i = pl; i < _players.size(); ++i)
                    {
                        if (!_players[i]->isSurrendered())
                        {
                            retirePlayer(i);
                        }
                    }

                    pl = _players.size();
                }
            }
            else
            {
                if (_players[pl]->name() == clients[cl].name() && _players[pl]->color() == clients[cl].color())
                {
                    ++pl;
                    ++cl;
                }
                else
                {
                    if (!_players[pl]->isSurrendered())
                    {
                        retirePlayer(pl);
                    }

                    ++pl;
                }
            }
        }
    }
}

void Game::addPlayer(ClientInfo info, PlayerType type)
{
    Player *player;
    switch (type)
    {
    case(ptLocal):
        player = new LocalPlayer(info);
        break;
    case(ptNetwork):
        player = new NetworkPlayer(info, _table);
        break;
    default:
        return;
    }

    int i = _playersleft;
    QString playerwidget(QString::fromUtf8("gvPlayer"));
    QString playerscore(QString::fromUtf8("score"));
    player->setPos(0, 0);
    _players.append(player);
    QGraphicsScene *playerscene = new QGraphicsScene;
    _scenes.append(playerscene);
    playerscene->addItem(player);
    QGraphicsView *gv = _widget->findChild<QGraphicsView *>(playerwidget + QString::number(i + 1));
    QLCDNumber *lcd = _widget->findChild<QLCDNumber *>(playerscore + QString::number(i + 1));
    gv->setScene(playerscene);
    gv->setMinimumSize(playerscene->sceneRect().size().toSize());
    gv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    gv->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    lcd->setPalette(QPalette(info.color()));
    connect(player, SIGNAL(scoreChanged(int)), lcd, SLOT(display(int)));
    lcd->display(0);
    ++_playersleft;
}

void Game::clear()
{
    _running = false;
    _playersleft = 0;
    _currplayer = 0;
    _table->clear();

    for (int i = 0; i < _scenes.size(); ++i)
    {
        _scenes[i]->deleteLater();
    }

    _scenes.clear();

    for (int i = 0; i < _scenes.size(); ++i)
    {
        _players[i]->deleteLater();
    }

    _players.clear();

    QList<QLCDNumber *> lcds = _widget->findChildren<QLCDNumber *>();
    for (int i = 0; i < lcds.size(); ++i)
    {
        lcds[i]->setPalette(QPalette());
        lcds[i]->display(0);
    }
}

void Game::retirePlayer(QString name, QColor color)
{
    for (int i = 0; i < _players.size(); ++i)
    {
        if (_players[i]->name() == name && _players[i]->color() == color)
        {
            retirePlayer(i);
            return;
        }
    }
}

void Game::start()
{
    if (_running)
    {
        return;
    }

    if (_players.size() == 0)
    {
        return;
    }

    if (_players.size() > 0)
    {
        _running = true;
        emit turnStarted(_players[0]->info());
        _players[0]->startTurn();
    }
}

void Game::turnComplete(QColor color, QString tile, int id, int x, int y)
{
    if (color != _players[_currplayer]->color())
    {
        return;
    }

    if (dynamic_cast<Table *>(sender()))
    {
        emit turnCompleted(_players[_currplayer]->name(), color, tile, id, x, y);
    }

    _players[_currplayer]->completeTurn(color, tile, id, x, y);
    if (_players[_currplayer]->leftTilesCount() == 0)
    {
        winner(_players[_currplayer]);
    }

    do
    {
        _currplayer = (_currplayer + 1) % _players.size();
    }
    while(_players[_currplayer]->isSurrendered());
    emit turnStarted(_players[_currplayer]->info());
    _players[_currplayer]->startTurn();
}
