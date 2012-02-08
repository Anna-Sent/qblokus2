#include "game.h"
#include "localplayer.h"
#include "networkplayer.h"
#include <iostream>
#include <QMessageBox>

using namespace std;

Game::Game(QGraphicsView *gv,
           QGraphicsView *gvs[MAX_PLAYERS_COUNT], QLCDNumber *lcds[MAX_PLAYERS_COUNT])
{
    for (int i = 0; i < MAX_PLAYERS_COUNT; ++i)
    {
        _gvs[i] = gvs[i];
        _lcds[i] = lcds[i];
    }

    _table = new Table(20, 20);
    _tablescene = new QGraphicsScene;
    _tablescene->addItem(_table);
    gv->setScene(_tablescene);
    connect(_table, SIGNAL(turnCompleted(QColor, QString, int, int, int)),
            this, SLOT(turnComplete(QColor, QString, int, int, int)));
    clear();
    connect(this, SIGNAL(destroyed()), this, SLOT(clear()));
    connect(this, SIGNAL(destroyed()), _tablescene, SLOT(deleteLater()));
    connect(this, SIGNAL(destroyed()), _table, SLOT(deleteLater()));
}

void Game::clear()
{
    _running = false;
    _currplayer = -1;
    _playersleft = 0;
    _table->clear();

    for (int i = 0; i < _scenes.size(); ++i)
    {
        _scenes[i]->deleteLater();
    }

    _scenes.clear();

    for (int i = 0; i < _players.size(); ++i)
    {
        _players[i]->deleteLater();
    }

    _players.clear();

    for (int i = 0; i < MAX_PLAYERS_COUNT; ++i)
    {
        _lcds[i]->setPalette(QPalette());
        _lcds[i]->display(0);
    }
}

void Game::countNextActivePlayerNumber()
{
    if (_currplayer == -1)
    {
        _currplayer = 0;
    }
    else
    {
        do
        {
            _currplayer = (_currplayer + 1) % _players.size();
        }
        while(_players[_currplayer]->isSurrendered());
    }
}

void Game::startTurn()
{
    countNextActivePlayerNumber();
    Player *player = _players[_currplayer];
    emit turnStarted(player->info());
    player->startTurn();
}

void Game::start()
{
    if (!_running && _players.size() > 0)
    {
        _running = true;
        startTurn();
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
    _currplayer = -1;
    emit gameOver(clients, winner->score());
}

void Game::turnComplete(QColor color, QString tile, int id, int x, int y)
{
    if (_currplayer != -1)
    {
        Player *player = _players[_currplayer];
        if (player->color() == color)
        {
            player->completeTurn(color, tile, id, x, y);
            if (dynamic_cast<Table *>(sender()))
            {
                emit turnCompleted(player->name(), color, tile, id, x, y);
            }

            if (player->leftTilesCount() == 0)
            {
                winner(_players[_currplayer]);
            }
            else
            {
                startTurn();
            }
        }
    }
}

void Game::retirePlayer(int i)
{
    if (_running)
    {
        Player *player = _players[i];
        player->surrender();
        --_playersleft;

        if (_playersleft > 0)
        {
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
            else
            {
                startTurn();
            }
        }
        else
        {
            _running = false;
            _currplayer = -1;
        }
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
    }

    player->setPos(0, 0);
    _players.append(player);
    QGraphicsScene *playerscene = new QGraphicsScene;
    _scenes.append(playerscene);
    playerscene->addItem(player);

    QGraphicsView *gv = _gvs[_playersleft];
    gv->setScene(playerscene);
    gv->setMinimumSize(playerscene->sceneRect().size().toSize());
    gv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    gv->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QLCDNumber *lcd = _lcds[_playersleft];
    connect(player, SIGNAL(scoreChanged(int)), lcd, SLOT(display(int)));
    lcd->setPalette(QPalette(info.color()));
    lcd->display(0);

    ++_playersleft;
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
                if (_players[pl]->info() == clients[cl])
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
