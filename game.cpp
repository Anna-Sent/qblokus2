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
    QPushButton *surrender = widget->findChild<QPushButton *>(QString::fromUtf8("pbSurrender"));
    connect(surrender, SIGNAL(clicked()), this, SLOT(retirePlayer()));
    surrender->setEnabled(false);

    _widget = widget;
    clear();

    connect(this, SIGNAL(destroyed()), this, SLOT(clear()));
    connect(this, SIGNAL(destroyed()), _tablescene, SLOT(deleteLater()));
    connect(this, SIGNAL(destroyed()), _table, SLOT(deleteLater()));
}

void Game::retirePlayer(int i)
{
    if (_players[i]->isSurrendered())
    {
        return;
    }

    if (_currplayer == i)
    {
        retirePlayer();
        return;
    }

    Player *player = _players[i];
    if (!player->_surrendered)
    {
        player->_surrendered = true;
        --_playersleft;
    }

    if (_playersleft == 1)
    {
        int msp = 0;
        for (int p = 1; p < _players.size(); ++p)
        {
            if (_players[p]->score() > _players[msp]->score())
            {
                msp = p;
            }
        }

        winner(_players[msp]);
    }

    player->update();
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
    QPushButton *surrender = _widget->findChild<QPushButton *>(QString::fromUtf8("pbSurrender"));
    surrender->setEnabled(false);
    emit gameOver(clients, winner->score());
}

void Game::retireRemotePlayer(QString name, QColor color)
{
    if (name == _players[_currplayer]->name() && color == _players[_currplayer]->color())
    {
        retirePlayer();
    }
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
    _ui->gridLayout->invalidate();
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

void Game::retirePlayer()
{
    if (!_running)
    {
        return;
    }

    if (sender() && dynamic_cast<QPushButton *>(sender()))
    {
        LocalPlayer *player = dynamic_cast<LocalPlayer *>(_players[_currplayer]);
        if (!player)
        {
            return;
        }

        QMessageBox msgBox;
        msgBox.setInformativeText(QString::fromUtf8("Do you really want to give up and finish the game?"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        msgBox.setIcon(QMessageBox::Warning);
        int ret = msgBox.exec();
        if (ret == QMessageBox::No)
        {
            return;
        }

        emit playerRetired(_players[_currplayer]->name(), _players[_currplayer]->color());
    }

    Player *player = _players[_currplayer];
    player->surrender();
    --_playersleft;
    if (_playersleft > 0)
    {
        do
        {
            _currplayer = (_currplayer + 1) % _players.size();
        }
        while(_players[_currplayer]->isSurrendered());
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
            _players[_currplayer]->startTurn();
            QPushButton *surrender = _widget->findChild<QPushButton *>(QString::fromUtf8("pbSurrender"));
            if (dynamic_cast<LocalPlayer *>(_players[_currplayer]))
            {
                surrender->setEnabled(true);
            }
            else
            {
                surrender->setEnabled(false);
            }
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
        _players[0]->startTurn();
        QPushButton *surrender = _widget->findChild<QPushButton *>(QString::fromUtf8("pbSurrender"));
        if (dynamic_cast<LocalPlayer *>(_players[_currplayer]))
        {
            surrender->setEnabled(true);
        }
        else
        {
            surrender->setEnabled(false);
        }
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
    _players[_currplayer]->startTurn();

    QPushButton *surrender = _widget->findChild<QPushButton *>(QString::fromUtf8("pbSurrender"));
    if (dynamic_cast<LocalPlayer *>(_players[_currplayer]))
    {
        surrender->setEnabled(true);
    }
    else
    {
        surrender->setEnabled(false);
    }
}
