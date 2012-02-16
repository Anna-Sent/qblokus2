#include "app.h"
#include <cstdlib>
#include <QMessageBox>

using namespace std;

App::App(QWidget *parent)
{
    Q_UNUSED(parent);
    setupUi(this);
    setTabOrder();

    // gui
    connect(actionStartGame, SIGNAL(activated()), this, SLOT(userStartGame()));
    connect(actionQuit, SIGNAL(activated()), qApp, SLOT(quit()));
    connect(actionConnect, SIGNAL(activated()), this, SLOT(userTryToConnect()));
    connect(cbCreateServer, SIGNAL(toggled(bool)), this, SLOT(guiToggleCreateServer(bool)));
    connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(userSendMessage()));
    connect(lineEdit, SIGNAL(returnPressed()), lineEdit, SLOT(clear()));
    connect(lwServersList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(guiClickServersListItem(QListWidgetItem*)));
    connect(lwServersList, SIGNAL(currentTextChanged(QString)), this, SLOT(guiChangeServersListCurrentText(QString)));
    connect(pbConnect, SIGNAL(clicked()), this, SLOT(userTryToConnect()));
    connect(pbSurrender, SIGNAL(clicked()), this, SLOT(guiClickRetirePlayer()));
    connect(sbPort, SIGNAL(valueChanged(int)), this, SLOT(guiChangePortValue(int)));

    // server searcher
    connect(&_serverSearcher, SIGNAL(serverInfoMessageReceived(const QHostAddress &,QList<ClientInfo>)), this, SLOT(receiveServerInfoMessage(const QHostAddress &,QList<ClientInfo>)));
    _serverSearcher.setPort(sbPort->value());
    _serverSearcher.start();

    // local client
    _localClient.moveToThread(&_localClientThread);
    connect(&_localClient, SIGNAL(connectionAccepted()), lwPlayersList, SLOT(clear()));
    connect(&_localClient, SIGNAL(connectionAccepted()), lwServersList, SLOT(clear()));
    connect(&_localClient, SIGNAL(connectionAccepted()), &_serverSearcher, SLOT(stop()));
    connect(&_localClient, SIGNAL(connectionAccepted()), this, SLOT(acceptConnection()));
    connect(&_localClient, SIGNAL(disconnected()), this, SLOT(processClientDisconnected()));
    connect(&_localClient, SIGNAL(disconnected()), lwPlayersList, SLOT(clear()));
    connect(&_localClient, SIGNAL(disconnected()), lwServersList, SLOT(clear()));
    connect(&_localClient, SIGNAL(disconnected()), &_serverSearcher, SLOT(start()));
    connect(&_localClient, SIGNAL(errorOccurred(QString)), this, SLOT(perror(QString)));
    connect(&_localClient, SIGNAL(chatMessageReceived(ClientInfo, QString)), this, SLOT(receiveChatMessage(ClientInfo, QString)));
    connect(&_localClient, SIGNAL(clientConnectMessageReceived(ClientInfo)), this, SLOT(receiveClientConnectMessage(ClientInfo)));
    connect(&_localClient, SIGNAL(clientDisconnectMessageReceived(ClientInfo)), this, SLOT(receiveClientDisconnectMessage(ClientInfo)));
    connect(&_localClient, SIGNAL(playersListMessageReceived(QList<ClientInfo>)), this, SLOT(receivePlayersListMessage(QList<ClientInfo>)));
    connect(&_localClient, SIGNAL(startGameMessageReceived(QList<ClientInfo>)), this, SLOT(receiveStartGameMessage(QList<ClientInfo>)));
    connect(&_localClient, SIGNAL(surrenderMessageReceived(ClientInfo)), this, SLOT(receiveSurrenderMessage(ClientInfo)));
    connect(this, SIGNAL(chatMessagePosted(ClientInfo, QString)), &_localClient, SLOT(sendChatMessage(ClientInfo, QString)));
    connect(this, SIGNAL(gameStarted()), &_localClient, SLOT(sendStartGameMessage()));
    connect(this, SIGNAL(gameStopped()), &_localClient, SLOT(sendStopGameMessage()));
    connect(this, SIGNAL(readyToStartLocalClient(QString, quint16)), &_localClient, SLOT(start(QString, quint16)));
    connect(this, SIGNAL(readyToStopLocalClient()), &_localClient, SLOT(stop()));
    connect(this, SIGNAL(surrendered(ClientInfo)), &_localClient, SLOT(sendSurrenderMessage(ClientInfo)));
    _localClientThread.start();

    // server
    _server.moveToThread(&_serverThread);
    connect(this, SIGNAL(readyToStartServer(int, quint16)), &_server, SLOT(start(int, quint16)));
    connect(this, SIGNAL(readyToStopServer()), &_server, SLOT(stop()));
    connect(&_server, SIGNAL(started()), this, SLOT(processServerStarted()));
    _serverThread.start();

    // game
    QGraphicsView *gvs[4] = { gvPlayer1, gvPlayer2, gvPlayer3, gvPlayer4 };
    QLCDNumber *lcds[4] = { score1, score2, score3, score4 };
    _game = new Game(gvTable, gvs, lcds);
    connect(_game, SIGNAL(gameOver(QList<ClientInfo>, int)),
            this, SLOT(finishGame(QList<ClientInfo>, int)));
    connect(_game, SIGNAL(turnCompleted(ClientInfo, QString, int, int, int)),
            this, SLOT(completeTurn(ClientInfo, QString, int, int, int)));
    connect(_game, SIGNAL(turnCompleted(ClientInfo, QString, int, int, int)),
            &_localClient, SLOT(sendTurnMessage(ClientInfo, QString, int, int, int)));
    connect(&_localClient, SIGNAL(turnMessageReceived(QColor, QString, int, int, int)),
            _game, SLOT(turnComplete(QColor, QString, int, int, int)));
    connect(_game, SIGNAL(turnStarted(ClientInfo)),
            this, SLOT(startTurn(ClientInfo)));

    connect(this, SIGNAL(destroyed()), _game, SLOT(deleteLater()));
    connect(this, SIGNAL(destroyed()), &_localClientThread, SLOT(quit()));
    connect(this, SIGNAL(destroyed()), &_serverThread, SLOT(quit()));
}

bool App::confirm(const QString &question) const
{
    QMessageBox msgBox;
    msgBox.setText(question);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    msgBox.setIcon(QMessageBox::Question);
    int ret = msgBox.exec();
    return ret == QMessageBox::Yes;
}

void App::setTabOrder() const
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
    QWidget::setTabOrder(sbPort, lePlayerName);
    QWidget::setTabOrder(lePlayerName, cbColor);
    QWidget::setTabOrder(cbColor, pbConnect);
    QWidget::setTabOrder(pbConnect, textEdit);
    QWidget::setTabOrder(textEdit, lineEdit);
    QWidget::setTabOrder(lineEdit, lwServersList);
}

void App::showCriticalMessage(const QString &text)
{
    QMessageBox::critical(this, tr("Error"), text);
}

void App::showInformationMessage(const QString &text)
{
    QMessageBox::information(this, tr("Information"), text);
}

void App::showWarningMessage(const QString &text)
{
    QMessageBox::warning(this, tr("Warning"), text);
}

void App::perror(const QString &text)
{
    pinfo(text, Qt::darkRed);
    if (!dockWidget->isVisible())
    {
        showCriticalMessage(text);
    }
}

void App::pinfo(const QString &text, const QColor &color)
{
    textEdit->setTextColor(color);
    textEdit->append(text);
}

void App::userDisconnectFromServer()
{
    if (_localClient.isStarted())
    {
        if (confirm(tr("Disconnect from the server?")))
        {
            emit readyToStopLocalClient();
            emit readyToStopServer();
        }
    }
}

void App::userSendMessage()
{
    QString text = lineEdit->text();
    if (text != "")
    {
        emit chatMessagePosted(_localClient.info(), text);
    }
}

void App::userStartGame()
{
    if (_localClient.isStarted())
    {
        if (_game->isStarted())
        {
            if (!confirm(tr("Are you sure you want to start a new game?")))
            {
                return;
            }
        }

        emit gameStarted();
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
        QColor color;
        switch (cbColor->currentIndex())
        {
        case 0:     color = Qt::red; break;
        case 1:     color = Qt::darkYellow; break;
        case 2:     color = Qt::green; break;
        case 3:     color = Qt::blue; break;
        default:    showCriticalMessage(tr("Incorrect color")); return;
        }

        _localClient.setColor(color);

        QString name = lePlayerName->text();
        if (name == "")
        {
            showWarningMessage(tr("Enter the nickname"));
            if (dockWidget->isVisible())
            {
                dockWidget->activateWindow();
                lePlayerName->setFocus();
            }

            return;
        }

        if (name.toUtf8().size() > 100)
        {
            showWarningMessage(tr("Your nickname is too long"));
            if (dockWidget->isVisible())
            {
                dockWidget->activateWindow();
                lePlayerName->setFocus();
            }

            return;
        }

        _localClient.setName(name);

        QString hostname = leServerAddress->text();
        if (hostname == "")
        {
            showWarningMessage(tr("Enter the server"));
            if (dockWidget->isVisible())
            {
                dockWidget->activateWindow();
                leServerAddress->setFocus();
            }

            return;
        }

        int port = sbPort->value();
        if (cbCreateServer->checkState())
        {
            // create server
            emit readyToStartServer(sbPlayersCount->value(), port);
        }
        else
        {
            emit readyToStartLocalClient(hostname, port);
        }
    }
}

void App::processServerStarted()
{
    if (_server.isListening())
    {
        emit readyToStartLocalClient(leServerAddress->text(), sbPort->value());
    }
    else
    {
        showCriticalMessage(_server.errorString());
    }
}

void App::acceptConnection()
{
    pinfo(tr("Connected"));
    lServersList->setDisabled(true);
    lwServersList->setDisabled(true);
    lServerAddress->setDisabled(true);
    leServerAddress->setDisabled(true);
    cbCreateServer->setDisabled(true);
    lPlayersCount->setDisabled(true);
    sbPlayersCount->setDisabled(true);
    lPort->setDisabled(true);
    sbPort->setDisabled(true);
    lPlayerName->setDisabled(true);
    lePlayerName->setDisabled(true);
    lColor->setDisabled(true);
    cbColor->setDisabled(true);
    pbConnect->setText(tr("Disconnect from the server"));
    actionConnect->setText(tr("Disconnect from the server"));
}

void App::processClientDisconnected()
{
    pinfo(tr("Disconnected"));
    lServersList->setDisabled(cbCreateServer->isChecked());
    lwServersList->setDisabled(cbCreateServer->isChecked());
    lServerAddress->setDisabled(cbCreateServer->isChecked());
    leServerAddress->setDisabled(cbCreateServer->isChecked());
    cbCreateServer->setDisabled(false);
    lPlayersCount->setEnabled(cbCreateServer->isChecked());
    sbPlayersCount->setEnabled(cbCreateServer->isChecked());
    lPort->setDisabled(false);
    sbPort->setDisabled(false);
    lPlayerName->setDisabled(false);
    lePlayerName->setDisabled(false);
    lColor->setDisabled(false);
    cbColor->setDisabled(false);
    pbConnect->setText(tr("Connect to the server"));
    actionConnect->setText(tr("Connect to the server"));
    pbSurrender->setDisabled(true);
    _game->clear();
}

void App::receiveChatMessage(const ClientInfo &info, const QString &text)
{
    pinfo("(" + QTime::currentTime().toString("hh:mm:ss") + ") " + info.name() + ":", info.color());
    pinfo(text, Qt::black);
}

void App::receiveClientConnectMessage(const ClientInfo &info)
{
    pinfo(tr("%1 connected").arg(info.name()), info.color());
}

void App::receiveClientDisconnectMessage(const ClientInfo &info)
{
    pinfo(tr("%1 disconnected").arg(info.name()), info.color());
}

void App::receivePlayersListMessage(QList<ClientInfo> list)
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
        isLocal.append(list[i] == _localClient.info());
    }

    lwPlayersList->sortItems();
    _game->updatePlayers(list, isLocal);
}

void App::receiveServerInfoMessage(const QHostAddress &host, QList<ClientInfo> clients)
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

void App::receiveStartGameMessage(QList<ClientInfo> list)
{
    pbSurrender->setDisabled(true);
    QList<bool> isLocal;
    for (int i = 0; i < list.size(); ++i)
    {
        isLocal.append(list[i] == _localClient.info());
    }

    _game->clear();
    _game->updatePlayers(list, isLocal);
    _game->start();
}

void App::receiveSurrenderMessage(const ClientInfo &info)
{
    pinfo(tr("%1 surrendered").arg(info.name()), info.color());
    _game->retirePlayer(info);
    if (_localClient.info() == info)
    {
        pbSurrender->setDisabled(true);
    }
}

void App::completeTurn(const ClientInfo &info, const QString &, int, int, int)
{
    if (_localClient.info() == info)
    {
        pbSurrender->setDisabled(true);
    }
}

void App::finishGame(QList<ClientInfo> winners, int score)
{
    emit gameStopped();
    int count = winners.count();
    QString msg;
    if (score > 0)
    {
        if (count == 1)
        {
            msg = tr("The winner is %1 with score %2")
                  .arg(winners[0].name(), QString::number(score));
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

            str += names[count - 1];
            msg = tr("The winners are %1 with score %2")
                  .arg(str, QString::number(score));
        }
    }
    else
    {
        msg = tr("There is no winner");
    }

    pbSurrender->setDisabled(true);
    pinfo(msg);
    showInformationMessage(msg);
}

void App::startTurn(const ClientInfo &info)
{
    if (_localClient.info() == info)
    {
        pbSurrender->setEnabled(true);
    }
}

void App::guiChangePortValue(int value)
{
    _serverSearcher.setPort(value);
    lwServersList->clear();
    lwPlayersList->clear();
}

void App::guiChangeServersListCurrentText(const QString &text)
{
    if (text != "")
    {
        leServerAddress->setText(text);
    }
}

void App::guiClickRetirePlayer()
{
    if (confirm(tr("Do you really want to give up and finish the game?")))
    {
        emit surrendered(_localClient.info());
    }
}

void App::guiClickServersListItem(QListWidgetItem *item)
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

void App::guiToggleCreateServer(bool value)
{
    if (value)
    {
        leServerAddress->setText("localhost");
        leServerAddress->setDisabled(true);
        _serverSearcher.stop();
        lwServersList->clear();
        lwPlayersList->clear();
    }
    else
    {
        leServerAddress->setEnabled(true);
        lwServersList->clear();
        lwPlayersList->clear();
        _serverSearcher.start();
    }
}
