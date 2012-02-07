#include "game.h"
#include "localplayer.h"
#include "networkplayer.h"
#include <iostream>
#include <QMessageBox>

using namespace std;

Game::Game(QWidget* widget)
{
    ui = dynamic_cast<Ui::MainWindow *>(widget);
    table = new Table(20, 20);
    tablescene = new QGraphicsScene;
    tablescene->addItem(table);
    ui->gvTable->setScene(tablescene);
    connect(table, SIGNAL(turnCompleted(QColor,QString,int,int,int)), this, SLOT(turnComplete(QColor,QString,int,int,int)));
    QPushButton *surrender = widget->findChild<QPushButton *>(QString::fromUtf8("pbSurrender"));
    connect(surrender, SIGNAL(clicked()), this, SLOT(playerRetired()));
    surrender->setEnabled(false);

    this->widget = widget;
    clear();

    connect(this, SIGNAL(destroyed()), this, SLOT(clear()));
    connect(this, SIGNAL(destroyed()), tablescene, SLOT(deleteLater()));
    connect(this, SIGNAL(destroyed()), table, SLOT(deleteLater()));
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
        player = new NetworkPlayer(info, table);
        break;
    default:
        return;
    }

    int i = playersleft;
    QString playerwidget(QString::fromUtf8("gvPlayer"));
    QString playerscore(QString::fromUtf8("score"));
    player->setPos(0, 0);
    players.append(player);
    QGraphicsScene *playerscene = new QGraphicsScene;
    scenes.append(playerscene);
    playerscene->addItem(player);
    QGraphicsView *gv = widget->findChild<QGraphicsView *>(playerwidget + QString::number(i + 1));
    QLCDNumber *lcd = widget->findChild<QLCDNumber *>(playerscore + QString::number(i + 1));
    gv->setScene(playerscene);
    ui->gridLayout->invalidate();
    gv->setMinimumSize(playerscene->sceneRect().size().toSize());
    gv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    gv->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    lcd->setPalette(QPalette(info.color()));
    connect(player, SIGNAL(scoreChanged(int)), lcd, SLOT(display(int)));
    connect(player, SIGNAL(won(Player *)), this, SLOT(winner(Player *)));
    lcd->display(0);
    ++playersleft;
}

void Game::turnComplete(QColor color, QString tile, int id, int x, int y)
{
    if (color != players[currplayer]->color())
    {
        return;
    }

    if (dynamic_cast<Table *>(sender()))
    {
        emit turnCompleted(players[currplayer]->name(), color, tile, id, x, y);
    }

    players[currplayer]->turnComplete(color, tile, id, x, y);
    do
    {
        currplayer = (currplayer + 1) % players.size();
    }
    while(players[currplayer]->isSurrendered());
    players[currplayer]->startTurn();

    QPushButton *surrender = widget->findChild<QPushButton *>(QString::fromUtf8("pbSurrender"));
    if (dynamic_cast<LocalPlayer *>(players[currplayer]))
    {
        surrender->setEnabled(true);
    }
    else
    {
        surrender->setEnabled(false);
    }
}

void Game::remotePlayerRetired(QString name, QColor color)
{
    if (name == players[currplayer]->name() && color == players[currplayer]->color())
    {
        playerRetired();
    }
}

void Game::playerRetired()
{
    if (!running)
    {
        return;
    }

    if (sender() && dynamic_cast<QPushButton *>(sender()))
    {
        LocalPlayer *player = dynamic_cast<LocalPlayer *>(players[currplayer]);
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

        emit playerRetired(players[currplayer]->name(), players[currplayer]->color());
    }

    Player *player = players[currplayer];
    player->surrender();
    --playersleft;
    if (playersleft > 0)
    {
        do
        {
            currplayer = (currplayer + 1) % players.size();
        }
        while(players[currplayer]->isSurrendered());
        if (playersleft == 1)
        {
            int msp = 0;
            for (int p = 1; p < players.size(); ++p)
            {
                if ((players[p]->score() > players[msp]->score()))
                {
                    msp = p;
                }
            }

            winner(players[msp]);
        }
        else
        {
            players[currplayer]->startTurn();
        }
    }

    QPushButton *surrender = widget->findChild<QPushButton *>(QString::fromUtf8("pbSurrender"));
    if (dynamic_cast<LocalPlayer *>(players[currplayer]))
    {
        surrender->setEnabled(true);
    }
    else
    {
        surrender->setEnabled(false);
    }
}

void Game::clear()
{
    running = false;
    playersleft = 0;
    currplayer = 0;
    table->clear();

    for (int i = 0; i < scenes.size(); ++i)
    {
        scenes[i]->deleteLater();
    }

    scenes.clear();

    for (int i = 0; i < scenes.size(); ++i)
    {
        players[i]->deleteLater();
    }

    players.clear();

    QList<QLCDNumber *> lcds = widget->findChildren<QLCDNumber *>();
    for (int i = 0;i < lcds.size(); ++i)
    {
        lcds[i]->setPalette(QPalette());
        lcds[i]->display(0);
    }
}

void Game::start()
{
    if (running)
    {
        return;
    }

    if (players.size() == 0)
    {
        return;
    }

    if (players.size() > 0)
    {
        players[0]->startTurn();
    }

    running = true;
    QPushButton *surrender = widget->findChild<QPushButton *>(QString::fromUtf8("pbSurrender"));
    if (dynamic_cast<LocalPlayer *>(players[currplayer]))
    {
        surrender->setEnabled(true);
    }
    else
    {
        surrender->setEnabled(false);
    }
}

void Game::winner(Player *winner)
{
    QList<ClientInfo> clients;
    for (int i = 0; i < players.size(); ++i)
    {
        if (players[i]->score() == winner->score())
        {
            clients.append(players[i]->info());
        }
    }

    running = false;
    emit gameOver(clients, winner->score());
}

void Game::retirePlayer(int i)
{
    if (players[i]->isSurrendered())
    {
        return;
    }

    if (currplayer == i)
    {
        playerRetired();
        return;
    }

    Player *player = players[i];
    if (!player->surrendered)
    {
        player->surrendered = true;
        --playersleft;
    }

    if (playersleft == 1)
    {
        int msp = 0;
        for (int p = 1; p < players.size(); ++p)
        {
            if (players[p]->score() > players[msp]->score())
            {
                msp = p;
            }
        }

        winner(players[msp]);
    }

    player->update();
}

void Game::updatePlayers(QList<ClientInfo> clients, QList<bool> local)
{
    if (!running)
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
        while(pl < players.size() || cl < clients.size())
        {
            if (pl == players.size() || cl == clients.size())
            {
                if (pl == players.size())
                {
                    addPlayer(clients[cl], local[cl] ? ptLocal : ptNetwork);
                }

                if (cl == clients.size())
                {
                    for (int i = pl; i < players.size(); ++i)
                    {
                        if (!players[i]->isSurrendered())
                        {
                            retirePlayer(i);
                        }
                    }

                    pl = players.size();
                }
            }
            else
            {
                if (players[pl]->name() == clients[cl].name() && players[pl]->color() == clients[cl].color())
                {
                    ++pl;
                    ++cl;
                }
                else
                {
                    if (!players[pl]->isSurrendered())
                    {
                        retirePlayer(pl);
                    }

                    ++pl;
                }
            }
        }
    }
}

bool Game::isStarted() const
{
    return running;
}
