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
    connect(&_localClient, SIGNAL(errorOccurred(QString)), this, SLOT(perror(QString)));
    connect(&_localClient, SIGNAL(errorOccurred()), &_serversSearcher, SLOT(start()));
    connect(&_localClient, SIGNAL(connectionAccepted()), lwPlayersList, SLOT(clear()));
    connect(&_localClient, SIGNAL(connectionAccepted()), lwServersList, SLOT(clear()));
    connect(&_localClient, SIGNAL(connectionAccepted()), &_serversSearcher, SLOT(stop()));
    connect(&_localClient, SIGNAL(connectionAccepted()), this, SLOT(connectionAccepted()));
    connect(&_localClient, SIGNAL(connectionRejected(const QString &)), this, SLOT(perror(const QString &)));
    connect(&_localClient, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));
    connect(&_localClient, SIGNAL(disconnected()), lwServersList, SLOT(clear()));
    connect(&_localClient, SIGNAL(disconnected()), lwPlayersList, SLOT(clear()));
    connect(&_localClient, SIGNAL(disconnected()), &_serversSearcher, SLOT(start()));
    connect(&_localClient, SIGNAL(chatMessageReceived(QString, QColor, QString)), this, SLOT(chatMessageReceived(QString, QColor, QString)));
    connect(&_localClient, SIGNAL(clientConnectMessageReceived(QString, QColor)), this, SLOT(clientConnectMessageReceived(QString, QColor)));
    connect(&_localClient, SIGNAL(clientDisconnectMessageReceived(QString, QColor)), this, SLOT(clientDisconnectMessageReceived(QString, QColor)));
    connect(&_localClient, SIGNAL(turnMessageReceived(QColor, int, int, int, QString)), this, SLOT(turnMessageReceived(QColor, int, int, int, QString)));
    connect(&_localClient, SIGNAL(playersListMessageReceived(QList<ClientInfo>)), this, SLOT(playersListMessageReceived(QList<ClientInfo>)));
    connect(&_localClient, SIGNAL(startGameMessageReceived(QList<ClientInfo>)), this, SLOT(startGameMessageReceived(QList<ClientInfo>)));
    connect(&_localClient, SIGNAL(surrenderMessageReceived(QString, QColor)), this, SLOT(surrenderMessageReceived(QString, QColor)));

    // from user
    connect(actionStartGame, SIGNAL(activated()), this, SLOT(userStartGame()));
    connect(actionQuit, SIGNAL(activated()), this, SLOT(userQuit()));
    connect(actionDisconnectFromServer, SIGNAL(activated()), this, SLOT(userDisconnectFromServer()));
    connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(userSendMessage()));
    connect(lineEdit, SIGNAL(returnPressed()), lineEdit, SLOT(clear()));
    connect(leNickname, SIGNAL(returnPressed()), pbConnect, SLOT(animateClick()));
    connect(leNickname, SIGNAL(returnPressed()), pbConnect, SLOT(setFocus()));
    connect(pbConnect, SIGNAL(clicked()), this, SLOT(userTryToConnect()));
    connect(lwServersList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(guiServersListItemClicked(QListWidgetItem*)));
    connect(lwServersList, SIGNAL(currentTextChanged(QString)), this, SLOT(guiServersListCurrentTextChanged(QString)));
    connect(cbCreateServer, SIGNAL(toggled(bool)), this, SLOT(guiCreateServerToggled(bool)));
    connect(sbPort, SIGNAL(valueChanged(int)), this, SLOT(guiPortValueChanged(int)));

    // from servers searcher
    connect(&_serversSearcher, SIGNAL(serverInfoMessageReceived(const QHostAddress &,QList<ClientInfo>)), this, SLOT(serverInfoMessageReceive(const QHostAddress &,QList<ClientInfo>)));

    _serversSearcher.setPort(sbPort->value());
    _serversSearcher.start();

    _game = new Game(this);
    connect(_game, SIGNAL(turnDone(QString,QColor,QString,int,int,int)),
            &_localClient, SLOT(doTurn(QString,QColor,QString,int,int,int)));
    connect(_game, SIGNAL(playerRetired(QString,QColor)),
            &_localClient, SLOT(surrender(QString,QColor)));
    connect(_game, SIGNAL(gameOver(QList<ClientInfo>, int)), this, SLOT(finishGame(QList<ClientInfo>, int)));

    connect(this, SIGNAL(destroyed()), _game, SLOT(deleteLater()));
    connect(this, SIGNAL(destroyed()), &_localClient, SLOT(stop()));
    connect(this, SIGNAL(destroyed()), &_serversSearcher, SLOT(stop()));
    connect(this, SIGNAL(destroyed()), &_server, SLOT(stop()));
}

void App::perror(const QString &text)
{
    textEdit->setTextColor(Qt::darkRed);
    textEdit->append(text);
}

void App::pinfo(const QString &text)
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
    QWidget::setTabOrder(cbCreateServer, sbPlayersCount);
    QWidget::setTabOrder(sbPlayersCount, sbPort);
    QWidget::setTabOrder(sbPort, leNickname);
    QWidget::setTabOrder(leNickname, cbColor);
    QWidget::setTabOrder(cbColor, pbConnect);
    QWidget::setTabOrder(pbConnect, textEdit);
    QWidget::setTabOrder(textEdit, lineEdit);
    QWidget::setTabOrder(lineEdit, lwServersList);
}

void App::userDisconnectFromServer()
{
    if (_localClient.isStarted())
    {
        QMessageBox msgBox;
        msgBox.setText(QString::fromUtf8("Disconnection"));
        msgBox.setInformativeText(QString::fromUtf8("Disconnect from server?"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        msgBox.setIcon(QMessageBox::Warning);
        int ret = msgBox.exec();
        if (ret == QMessageBox::No)
        {
            return;
        }

        _localClient.stop();
        _server.stop();
    }
}

void App::userQuit()
{
    exit(0);
}

void App::userSendMessage()
{
    QString text = lineEdit->text();
    if (text != "")
    {
        _localClient.sendMessage(text);
    }
}

void App::userStartGame()
{
    if (_server.isRunning())
    {
        if (_server.playersCount() == _server.maxClientsCount())
        {
            if (_game->isStarted())
            {
                QMessageBox msgBox;
                msgBox.setText(QString::fromUtf8("New game"));
                msgBox.setInformativeText(QString::fromUtf8(
                        "Are you sure you want to start a new game?"));
                msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                msgBox.setDefaultButton(QMessageBox::No);
                msgBox.setIcon(QMessageBox::Warning);
                int ret = msgBox.exec();
                if (ret == QMessageBox::No)
                {
                    return;
                }
            }

            _server.startGame(_server.clients());
        }
        else
        {
            QMessageBox::warning(this, QString::fromUtf8("Warning"), QString::fromUtf8("Wait for ")
                                 + QString::number(_server.maxClientsCount())
                                 + QString::fromUtf8(" players"));
        }
    }
    else
    {
        QMessageBox::warning(this, QString::fromUtf8("Warning"), QString::fromUtf8(
                "Only the server can start a game"));
    }
}

void App::userTryToConnect()
{
    if (_localClient.isStarted())
    {
        userDisconnectFromServer();
    }
    else
    {
        switch (cbColor->currentIndex())
        {
        case 0:     _localClient.setColor(Qt::red); break;
        case 1:     _localClient.setColor(Qt::darkYellow); break;
        case 2:     _localClient.setColor(Qt::green); break;
        case 3:     _localClient.setColor(Qt::blue); break;
        default:    QMessageBox::warning(this, QString::fromUtf8("Error"), QString::fromUtf8(
                    "Incorrect color")); return;
        }

        if (leNickname->text() == "")
        {
            QMessageBox::warning(this, QString::fromUtf8("Error"), QString::fromUtf8(
                    "Enter the nickname"));
            dockWidget->activateWindow();
            leNickname->setFocus();
            return;
        }
        else if (leNickname->text().toUtf8().size() > 100)
        {
            QMessageBox::warning(this, QString::fromUtf8("Error"), QString::fromUtf8(
                    "Your nickname is too long"));
            dockWidget->activateWindow();
            leNickname->setFocus();
            return;
        }

        int port = sbPort->value();
        QString hostname = cbCreateServer->checkState() ? "localhost" : leServerAddress->text();
        if (hostname == "")
        {
            QMessageBox::warning(this, QString::fromUtf8("Error"), QString::fromUtf8(
                    "Enter the server"));
            dockWidget->activateWindow();
            leServerAddress->setFocus();
            return;
        }

        if (cbCreateServer->checkState())
        {
            // create server
            int maxClientsCount = sbPlayersCount->value();
            _server.start(maxClientsCount, port);
            if (!_server.isRunning())
            {
                QMessageBox::critical(this, QString::fromUtf8("Error"), _server.errorString());
                return;
            }
        }

        _localClient.setNickname(leNickname->text());
        _localClient.start(hostname, port);
    }
}

void App::chatMessageReceived(QString name, QColor color, QString text)
{
    textEdit->setTextColor(color);
    textEdit->append("(" + QTime::currentTime().toString("hh:mm:ss") + ") " + name + ":");
    textEdit->setTextColor(Qt::black);
    textEdit->append(text);
}

void App::clientConnectMessageReceived(QString name, QColor color)
{
    textEdit->setTextColor(color);
    textEdit->append(name + QString::fromUtf8(" connected"));
}

void App::clientDisconnected()
{
    pinfo(QString::fromUtf8("Disconnected"));
    lServersList->setDisabled(cbCreateServer->isChecked());
    lwServersList->setDisabled(cbCreateServer->isChecked());
    lServerAddress->setDisabled(cbCreateServer->isChecked());
    leServerAddress->setDisabled(cbCreateServer->isChecked());
    cbCreateServer->setDisabled(false);
    lPlayersCount->setEnabled(cbCreateServer->isChecked());
    sbPlayersCount->setEnabled(cbCreateServer->isChecked());
    lPort->setDisabled(false);
    sbPort->setDisabled(false);
    lNickname->setDisabled(false);
    leNickname->setDisabled(false);
    lColor->setDisabled(false);
    cbColor->setDisabled(false);
    pbConnect->setText(QString::fromUtf8("Connect to the server"));
    _game->clear();
}

void App::clientDisconnectMessageReceived(QString name, QColor color)
{
    textEdit->setTextColor(color);
    textEdit->append(name + QString::fromUtf8(" disconnected"));
}

void App::connectionAccepted()
{
    pinfo(QString::fromUtf8("Connected"));
    lServersList->setDisabled(true);
    lwServersList->setDisabled(true);
    lServerAddress->setDisabled(true);
    leServerAddress->setDisabled(true);
    cbCreateServer->setDisabled(true);
    lPlayersCount->setDisabled(true);
    sbPlayersCount->setDisabled(true);
    lPort->setDisabled(true);
    sbPort->setDisabled(true);
    lNickname->setDisabled(true);
    leNickname->setDisabled(true);
    lColor->setDisabled(true);
    cbColor->setDisabled(true);
    pbConnect->setText(QString::fromUtf8("Disconnect from the server"));
}

void App::playersListMessageReceived(QList<ClientInfo> list)
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
        isLocal.append(list[i].name() == _localClient.nickname()
                       && list[i].color() == _localClient.color());
    }

    lwPlayersList->sortItems();
    _game->updatePlayers(list, isLocal);
}

void App::startGameMessageReceived(QList<ClientInfo> list)
{
    QList<bool> isLocal;
    for (int i = 0; i < list.size(); ++i)
    {
        isLocal.append(list[i].name() == _localClient.nickname()
                       && list[i].color() == _localClient.color());
    }

    _game->clear();
    _game->updatePlayers(list, isLocal);
    _game->start();
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

void App::finishGame(QList<ClientInfo> winners, int score)
{
    if (_server.isRunning())
    {
        _server.finishGame();
    }

    int count = winners.count();
    QMessageBox msgBox;
    if (count == 1)
    {
        msgBox.setText(QString::fromUtf8("The winner is ") + winners[0].name()
                       + QString::fromUtf8(" with score ") + QString::number(score));
    }
    else if (count > 1)
    {
        QStringList names;
        for (int i = 0; i < count; ++i)
        {
            names << winners[i].name();
        }

        names.sort();
        QString str;
        for (int i = 0; i < count - 1; ++i)
        {
            str += names[i] + ", ";
        }

        str += winners[count - 1];
        msgBox.setText(QString::fromUtf8("The winners are ") + str
                       + QString::fromUtf8(" with score ") + QString::number(score));
    }
    else
    {
        msgBox.setText(QString::fromUtf8("There is no winner"));
    }

    msgBox.exec();
}

void App::surrenderMessageReceived(QString name, QColor color)
{
    _game->remotePlayerRetired(name, color);
}

void App::turnMessageReceived(QColor color, int x, int y, int id, QString mask)
{
    _game->turnDone(color, mask, id, x, y);
}

void App::guiCreateServerToggled(bool value)
{
    if (value)
    {
        leServerAddress->setText("localhost");
        _serversSearcher.stop();
        lwServersList->clear();
        lwPlayersList->clear();
    }
    else
    {
        lwPlayersList->clear();
        lwServersList->clear();
        _serversSearcher.start();
    }
}

void App::guiPortValueChanged(int value)
{
    _serversSearcher.setPort(value);
    lwServersList->clear();
    lwPlayersList->clear();
}

void App::guiServersListCurrentTextChanged(QString text)
{
    if (text != "")
    {
        leServerAddress->setText(text);
    }
}

void App::guiServersListItemClicked(QListWidgetItem *item)
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
