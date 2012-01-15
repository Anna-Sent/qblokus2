#include "app.h"
#include <cstdlib>
#include <QMessageBox>
using namespace std;

App::App(QWidget *parent)
{
    Q_UNUSED(parent);
    setupUi(this);
    setTabOrder();

    // from local client
    connect(&localClient, SIGNAL(lcError(QString)), this, SLOT(perror(QString)));
    connect(&localClient, SIGNAL(lcError()), &serversSearcher, SLOT(start()));
    connect(&localClient, SIGNAL(lcError()), &server, SLOT(quit()));
    connect(&localClient, SIGNAL(lcConnected()), this, SLOT(localConnected()));
    connect(&localClient, SIGNAL(lcConnected()), lwPlayersList, SLOT(clear()));
    connect(&localClient, SIGNAL(lcConnected()), lwServersList, SLOT(clear()));
    connect(&localClient, SIGNAL(lcConnected()), &serversSearcher, SLOT(stop()));
    connect(&localClient, SIGNAL(lcDisconnected()), this, SLOT(localDisconnected()));
    connect(&localClient, SIGNAL(lcDisconnected()), lwPlayersList, SLOT(clear()));
    connect(&localClient, SIGNAL(lcDisconnected()), lwServersList, SLOT(clear()));
    connect(&localClient, SIGNAL(lcDisconnected()), &server, SLOT(quit()));
    connect(&localClient, SIGNAL(lcDisconnected()), &serversSearcher, SLOT(start()));
    connect(&localClient, SIGNAL(lcChatMessageReceive(QString, QColor, QString)), this, SLOT(localChatMessageReceive(QString, QColor, QString)));
    connect(&localClient, SIGNAL(lcPlayersListMessageReceive(QList<ClientInfo>)), this, SLOT(localPlayersListMessageReceive(QList<ClientInfo>)));
    connect(&localClient, SIGNAL(lcClientConnectMessageReceive(QString, QColor)), this, SLOT(localClientConnectMessageReceive(QString, QColor)));
    connect(&localClient, SIGNAL(lcClientDisconnectMessageReceive(QString, QColor)), this, SLOT(localClientDisconnectMessageReceive(QString, QColor)));
    connect(&localClient, SIGNAL(lcConnectionAcceptedMessageReceive(int)), this, SLOT(localConnectionAcceptedMessageReceive(int)));

    // game signals
    connect(&localClient, SIGNAL(lcTurnMessageReceive(QColor, int, int, int, QString)), this, SLOT(localTurnMessageReceive(QColor, int, int, int, QString)));
    connect(&localClient, SIGNAL(lcStartGameMessageReceive()), this, SLOT(localStartGameMessageReceive()));
    connect(&localClient, SIGNAL(lcRestartGameMessageReceive(QList<ClientInfo>)), this, SLOT(localRestartGameMessageReceive(QList<ClientInfo>)));
    connect(&localClient, SIGNAL(lcSurrenderMessageReceive(QString, QColor)), this, SLOT(localSurrenderMessageReceive(QString, QColor)));

    // from graphics
    connect(actionStartGame, SIGNAL(activated()), this, SLOT(userStartGame()));
    connect(actionQuit, SIGNAL(activated()), this, SLOT(userQuit()));
    connect(actionDisconnectFromServer, SIGNAL(activated()), this, SLOT(userDisconnectFromServer()));
    connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(userSendMessage()));
    connect(lineEdit, SIGNAL(returnPressed()), lineEdit, SLOT(clear()));
    connect(leNickname, SIGNAL(returnPressed()), pbConnect, SLOT(animateClick()));
    connect(leNickname, SIGNAL(returnPressed()), pbConnect, SLOT(setFocus()));

    connect(this, SIGNAL(startGame()), &server, SLOT(startGame()));
    connect(this, SIGNAL(restartGame(QList<ClientInfo>)), &server, SLOT(restartGame(QList<ClientInfo>)));
    connect(this, SIGNAL(sendMessage(QString)), &localClient, SLOT(sendMessage(QString)));

    connect(pbConnect, SIGNAL(clicked()), this, SLOT(connectClicked()));
    connect(lwServersList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(serversListItemClicked(QListWidgetItem*)));
    connect(lwServersList, SIGNAL(currentTextChanged(QString)), this, SLOT(serversListCurrentTextChanged(QString)));
    connect(cbCreateServer, SIGNAL(toggled(bool)), this, SLOT(createServerToggled(bool)));
    connect(sbPort, SIGNAL(valueChanged(int)), this, SLOT(portValueChanged(int)));

    // from servers searcher
    connect(&serversSearcher, SIGNAL(serverInfoMessageReceive(const QHostAddress &,QList<ClientInfo>)), this, SLOT(serverInfoMessageReceive(const QHostAddress &,QList<ClientInfo>)));

    serversSearcher.setPort(sbPort->value());
    serversSearcher.start();

    game = new Game(this);
}

App::~App()
{
    localClient.quit();
    server.quit();
    serversSearcher.stop();
}

void App::perror(QString text)
{
    textEdit->setTextColor(Qt::darkRed);
    textEdit->append(text);
}

void App::pinfo(QString text)
{
    textEdit->setTextColor(Qt::darkBlue);
    textEdit->append(text);
}

void App::setTabOrder()
{
    QWidget::setTabOrder(gvPlayer1, gvPlayer2);
    QWidget::setTabOrder(gvPlayer2, gvPlayer3);
    QWidget::setTabOrder(gvPlayer3, gvPlayer4);
    QWidget::setTabOrder(gvPlayer4, gvTable);
    QWidget::setTabOrder(gvTable, pbSurrender);
    QWidget::setTabOrder(pbSurrender, gvPlayer1);

    QWidget::setTabOrder(lwServersList, lwPlayersList);
    QWidget::setTabOrder(lwPlayersList, leServerAddress);
    QWidget::setTabOrder(leServerAddress, cbCreateServer);
    QWidget::setTabOrder(cbCreateServer, sbClientsCount);
    QWidget::setTabOrder(sbClientsCount, sbPort);
    QWidget::setTabOrder(sbPort, leNickname);
    QWidget::setTabOrder(leNickname, cbColor);
    QWidget::setTabOrder(cbColor, pbConnect);
    QWidget::setTabOrder(pbConnect, textEdit);
    QWidget::setTabOrder(textEdit, lineEdit);
    QWidget::setTabOrder(lineEdit, lwServersList);
}

void App::userStartGame()
{
    if (server.isRunning())
    {
        if (server.getPlayersCount() == server.getMaxClientsCount())
        {
            if (game && game->isStarted())
            {
                QMessageBox msgBox;
                msgBox.setText("New game");
                msgBox.setInformativeText("Start new game?");
                msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                msgBox.setDefaultButton(QMessageBox::No);
                msgBox.setIcon(QMessageBox::Warning);
                int ret = msgBox.exec();
                switch(ret)
                {
                case(QMessageBox::No): return;
                case(QMessageBox::Yes): break;
                }

                delete game;
                game = new Game(this);
                QList<ClientInfo> list = server.getClients();
                QList<bool> isLocal;
                for (int i = 0; i < list.size(); ++i)
                {
                    isLocal.append(list[i].name() == localClient.getNickname()
                                   && list[i].color() == localClient.getColor());
                }

                game->updatePlayers(list, isLocal);
                game->start();
                emit restartGame(list);
            }
            else
            {
                game->start();
                emit startGame();
            }
        }
        else
        {
            QMessageBox::warning(this, "Warning", "Wait for "
                                 + QString::number(server.getMaxClientsCount()) + " players");
        }
    }
    else
    {
        QMessageBox::warning(this, "Warning", "Only server can start the game");
    }
}

void App::userQuit()
{
    exit(0);
}

void App::userDisconnectFromServer()
{
    if (localClient.isStarted())
    {
        QMessageBox msgBox;
        msgBox.setText("Disconnection");
        msgBox.setInformativeText("Disconnect from server?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        msgBox.setIcon(QMessageBox::Warning);
        int ret = msgBox.exec();
        switch(ret)
        {
        case(QMessageBox::No): return;
        case(QMessageBox::Yes): break;
        }

        localClient.quit();
        server.quit();
    }
}

void App::userSendMessage()
{
    QString text = lineEdit->text();
    if (text != "")
    {
        emit sendMessage(text);
    }
}

void App::localConnected()
{
}

void App::localDisconnected()
{
    pinfo("Disconnected");
    lServersList->setDisabled(cbCreateServer->isChecked());
    lwServersList->setDisabled(cbCreateServer->isChecked());
    lServerAddress->setDisabled(cbCreateServer->isChecked());
    leServerAddress->setDisabled(cbCreateServer->isChecked());
    cbCreateServer->setDisabled(false);
    sbClientsCount->setEnabled(cbCreateServer->isChecked());
    lPort->setDisabled(false);
    sbPort->setDisabled(false);
    lNickname->setDisabled(false);
    leNickname->setDisabled(false);
    lColor->setDisabled(false);
    cbColor->setDisabled(false);
    pbConnect->setText("Connect to server");
    if (game)
    {
        delete game;
        game = NULL;
    }
}

void App::localChatMessageReceive(QString name, QColor color, QString text)
{
    textEdit->setTextColor(color);
    textEdit->append("(" + QTime::currentTime().toString("hh:mm:ss") + ") " + name + ":");
    textEdit->setTextColor(Qt::black);
    textEdit->append(text);
}

void App::localPlayersListMessageReceive(QList<ClientInfo> list)
{
    lwPlayersList->clear();
    QList<bool> isLocal;
    for (int i = 0; i < list.size(); ++i)
    {
        QListWidgetItem *item = new QListWidgetItem();
        QBrush brush = item->foreground();
        brush.setColor(list[i].color());
        item->setForeground(brush);
        item->setText(list[i].name());
        lwPlayersList->addItem(item);
        isLocal.append(list[i].name() == localClient.getNickname()
                       && list[i].color() == localClient.getColor());
    }

    lwPlayersList->sortItems();
    game->updatePlayers(list, isLocal);
}

void App::localClientConnectMessageReceive(QString name, QColor color)
{
    textEdit->setTextColor(color);
    textEdit->append(name + " connected");
}

void App::localClientDisconnectMessageReceive(QString name, QColor color)
{
    textEdit->setTextColor(color);
    textEdit->append(name + " disconnected");
}

void App::localConnectionAcceptedMessageReceive(int errorCode)
{
    if (errorCode != 0)
    {
        switch (errorCode)
        {
        case 1: perror("This color is already in use"); break;
        case 2: perror("This nickname is already in use"); break;
        case 4: perror("The maximum allowed number of players has been reached for the game"); break;
        }

        localClient.quit();
    }
    else
    {
        pinfo("Connected");
        lServersList->setDisabled(true);
        lwServersList->setDisabled(true);
        lServerAddress->setDisabled(true);
        leServerAddress->setDisabled(true);
        cbCreateServer->setDisabled(true);
        sbClientsCount->setDisabled(true);
        lPort->setDisabled(true);
        sbPort->setDisabled(true);
        lNickname->setDisabled(true);
        leNickname->setDisabled(true);
        lColor->setDisabled(true);
        cbColor->setDisabled(true);
        pbConnect->setText("Disconnect from server");
    }
}

void App::localTurnMessageReceive(QColor color, int x, int y, int id, QString mask)
{
    game->turnDone(color, mask, id, x, y);
}

void App::localStartGameMessageReceive()
{
    game->start();
}

void App::localRestartGameMessageReceive(QList<ClientInfo> list)
{
    if (game)
    {
        delete game;
        game = new Game(this);
        connect(game, SIGNAL(turnDone(QString,QColor,QString,int,int,int)), &localClient, SLOT(turnDone(QString,QColor,QString,int,int,int)));
        connect(game, SIGNAL(playerRetired(QString,QColor)), &localClient, SLOT(playerSurrendered(QString,QColor)));
        QList<bool> isLocal;
        for (int i = 0; i < list.size(); ++i)
        {
            isLocal.append(list[i].name() == localClient.getNickname()
                           && list[i].color() == localClient.getColor());
        }

        game->updatePlayers(list, isLocal);
        game->start();
    }
}

void App::localSurrenderMessageReceive(QString name, QColor color)
{
    game->remotePlayerRetired(name, color);
}

void App::connectClicked()
{
    if (localClient.isStarted())
    {
        userDisconnectFromServer();
    }
    else
    {
        switch (cbColor->currentIndex())
        {
        case 0:     localClient.setColor(Qt::red); break;
        case 1:     localClient.setColor(Qt::darkYellow); break;
        case 2:     localClient.setColor(Qt::green); break;
        case 3:     localClient.setColor(Qt::blue); break;
        default:    QMessageBox::warning(this, "Error", "Incorrect color"); return;
        }

        if (leNickname->text() == "")
        {
            QMessageBox::warning(this, "Error", "Enter nickname");
            dockWidget->activateWindow();
            leNickname->setFocus();
            return;
        }
        else if (leNickname->text().toUtf8().size() > 100)
        {
            QMessageBox::warning(this, "Error", "Your nickname is too long");
            dockWidget->activateWindow();
            leNickname->setFocus();
            return;
        }

        int port = sbPort->value();
        QString hostname = cbCreateServer->checkState() ? "localhost" : leServerAddress->text();
        if (hostname == "")
        {
            QMessageBox::warning(this, "Error", "Enter server address");
            dockWidget->activateWindow();
            leServerAddress->setFocus();
            return;
        }

        if (cbCreateServer->checkState())
        {
            // create server
            int maxClientsCount = sbClientsCount->value();
            bool listening = server.start(maxClientsCount, port);
            if (!listening)
            {
                QMessageBox::critical(this, "Error", server.getErrorString());
                return;
            }
        }

        if (!game)
        {
            game = new Game(this);
        }

        localClient.setNickname(leNickname->text());
        connect(game, SIGNAL(turnDone(QString,QColor,QString,int,int,int)), &localClient, SLOT(turnDone(QString,QColor,QString,int,int,int)));
        connect(game, SIGNAL(playerRetired(QString,QColor)), &localClient, SLOT(playerSurrendered(QString,QColor)));
        localClient.start(hostname, port);
    }
}

void App::serversListItemClicked(QListWidgetItem *item)
{
    lwPlayersList->clear();
    QList<ClientInfo> clients = item->data(Qt::UserRole).value<QList<ClientInfo> >();
    for (int i = 0; i < clients.size(); ++i)
    {
        QListWidgetItem *item = new QListWidgetItem();
        QBrush brush = item->foreground();
        brush.setColor(clients[i].color());
        item->setForeground(brush);
        item->setText(clients[i].name());
        lwPlayersList->addItem(item);
    }
}

void App::serversListCurrentTextChanged(QString text)
{
    if (text != "")
    {
        leServerAddress->setText(text);
    }
}

void App::createServerToggled(bool value)
{
    if (value)
    {
        leServerAddress->setText("localhost");
        serversSearcher.stop();
        lwServersList->clear();
        lwPlayersList->clear();
    }
    else
    {
        serversSearcher.start();
    }
}

void App::portValueChanged(int value)
{
    serversSearcher.setPort(value);
    lwServersList->clear();
    lwPlayersList->clear();
}

void App::serverInfoMessageReceive(const QHostAddress &host, QList<ClientInfo> clients)
{
    QString address = host.toString();
    QList<QListWidgetItem *> items = lwServersList->findItems(address, Qt::MatchExactly);
    if (items.count() == 0)
    {
        QListWidgetItem *item = new QListWidgetItem();
        item->setText(address);
        item->setData(Qt::UserRole, qVariantFromValue(clients));
        lwServersList->addItem(item);
    }
    else
    {
        // updates clients information
        foreach (QListWidgetItem *item, items)
        {
            item->setData(Qt::UserRole, qVariantFromValue(clients));
        }
    }
}
