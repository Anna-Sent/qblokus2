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
    connect(table, SIGNAL(turnComplete(QColor,QString,int,int,int)), this, SLOT(turnDone(QColor,QString,int,int,int)));
    QPushButton *surrender = widget->findChild<QPushButton *>(QString("pbSurrender"));
    connect(surrender, SIGNAL(clicked()), this, SLOT(playerRetired()));
    surrender->setEnabled(false);

    this->widget = widget;
    clear();
}

void Game::addPlayer(QString name, QColor color, PlayerType type)
{
    Player *player;
    switch (type)
    {
    case(ptLocal):
        player = new LocalPlayer(color);
        break;
    case(ptNetwork):
        player = new NetworkPlayer(color, table);
        break;
    default:
        return;
    }

    int i = playersleft;
    QString playerwidget("gvPlayer");
    QString playerscore("score");
    player->setPos(0, 0);
    player->setName(name);
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

    lcd->setPalette(QPalette(color));
    connect(player, SIGNAL(scoreChanged(int)), lcd, SLOT(display(int)));
    connect(player, SIGNAL(iWin(Player *)), this, SLOT(winner(Player *)));
    lcd->display(0);
    ++playersleft;
}

void Game::turnDone(QColor color, QString tile, int id, int x, int y)
{
    if (color != players[currplayer]->getColor())
    {
        return;
    }

    if (dynamic_cast<Table *>(sender()))
    {
        emit turnDone(players[currplayer]->getName(), color, tile, id, x, y);
    }

    players[currplayer]->turnComplete(color, tile, id, x, y);
    do
    {
        currplayer = (currplayer + 1) % players.size();
    }
    while(players[currplayer]->getSurrendered());
    QPushButton *surrender = widget->findChild<QPushButton *>(QString("pbSurrender"));
    if (dynamic_cast<LocalPlayer *>(players[currplayer]))
    {
        surrender->setEnabled(true);
    }
    else
    {
        surrender->setEnabled(false);
    }

    players[currplayer]->startTurn();
}

void Game::remotePlayerRetired(QString name, QColor color)
{
    if (name == players[currplayer]->getName() && color == players[currplayer]->getColor())
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
        msgBox.setInformativeText(QString::fromUtf8("Вы действительно хотите сдаться и закончить игру?"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        msgBox.setIcon(QMessageBox::Warning);
        int ret = msgBox.exec();
        if (ret == QMessageBox::No)
        {
            return;
        }

        emit playerRetired(players[currplayer]->getName(), players[currplayer]->getColor());
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
        while(players[currplayer]->getSurrendered());
        if (playersleft == 1)
        {
            int msp = 0;
            for(int p = 1; p < players.size(); ++p)
            {
                if ((players[p]->getScore() > players[msp]->getScore()))
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

    QPushButton *surrender = widget->findChild<QPushButton *>(QString("pbSurrender"));
    if (dynamic_cast<LocalPlayer *>(players[currplayer]))
    {
        surrender->setEnabled(true);
    }
    else
    {
        surrender->setEnabled(false);
    }
}

Game::~Game()
{
    clear();
    delete tablescene;
}

void Game::clear()
{
    running = false;
    for (int i = 0; i < scenes.size(); ++i)
    {
        delete scenes[i];
    }

    playersleft = 0;
    currplayer = 0;
    table->clear();
    scenes.clear();
    players.clear();
    QList<QLCDNumber *> lcds = widget->findChildren<QLCDNumber *>();
    for(int i = 0;i < lcds.size(); ++i)
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

    QPushButton *surrender = widget->findChild<QPushButton *>(QString("pbSurrender"));
    if (dynamic_cast<LocalPlayer *>(players[currplayer]))
    {
        surrender->setEnabled(true);
    }
    else
    {
        surrender->setEnabled(false);
    }

    if (players.size() > 0)
    {
        players[0]->startTurn();
    }

    running = true;
}

void Game::winner(Player *winner)
{
    QString winners;
    int count = 0;
    for(int i = 0; i < players.size(); ++i)
    {
        if (players[i]->getScore() == winner->getScore())
        {
            winners.append(players[i]->getName() + " ");
            ++count;
        }
    }

    QMessageBox msgBox;
    if (count == 1)
    {
        msgBox.setText(QString::fromUtf8("Победитель: ") + winners);
    }
    else
    {
        msgBox.setText(QString::fromUtf8("Победители: ")+winners);
    }

    msgBox.exec();
    running = false;
    emit gameOver(winner->getName(), winner->getScore(), winner->getColor());
}

bool operator==(const ClientInfo &a1,const ClientInfo &a2)
{
    return a1.name() == a2.name() && a1.color() == a2.color();
}

void Game::retirePlayer(int i)
{
    if (players[i]->getSurrendered())
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
        for(int p = 1; p < players.size(); ++p)
        {
            if (players[p]->getScore() > players[msp]->getScore())
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
        for(int i = 0; i < clients.size(); ++i)
        {
            addPlayer(clients[i].name(), clients[i].color(), local[i] ? ptLocal : ptNetwork);
        }
    }
    else
    {
        //merge
        int pl = 0, cl = 0;
        while(pl < players.size() || cl < clients.size())
        {
            if (pl == players.size() || cl == clients.size())
            {
                if (pl == players.size())
                {
                    addPlayer(clients[cl].name(), clients[cl].color(), local[cl] ? ptLocal : ptNetwork);
                }

                if (cl == clients.size())
                {
                    for(int i = pl; i < players.size(); ++i)
                    {
                        if (!players[i]->getSurrendered())
                        {
                            retirePlayer(i);
                        }
                    }

                    pl = players.size();
                }
            }
            else
            {
                if (players[pl]->getName() == clients[cl].name() && players[pl]->getColor() == clients[cl].color())
                {
                    ++pl;
                    ++cl;
                }
                else
                {
                    if (!players[pl]->getSurrendered())
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
