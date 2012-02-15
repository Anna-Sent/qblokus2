/********************************************************************************
** Form generated from reading UI file 'racingForm.ui'
**
** Created: Thu Feb 16 01:39:54 2012
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RACINGFORM_H
#define UI_RACINGFORM_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDockWidget>
#include <QtGui/QGraphicsView>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLCDNumber>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QStatusBar>
#include <QtGui/QTextEdit>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionQuit;
    QAction *actionDisconnectFromServer;
    QAction *actionStartGame;
    QAction *actionConnectionWindow;
    QAction *actionConnect;
    QAction *actionEnglish;
    QAction *actionRussian;
    QWidget *centralwidget;
    QGridLayout *gridLayout_11;
    QGridLayout *gridLayout;
    QLCDNumber *score1;
    QLCDNumber *score3;
    QGraphicsView *gvPlayer1;
    QGraphicsView *gvPlayer3;
    QLCDNumber *score2;
    QLCDNumber *score4;
    QGraphicsView *gvPlayer2;
    QGraphicsView *gvTable;
    QGraphicsView *gvPlayer4;
    QPushButton *pbSurrender;
    QMenuBar *menubar;
    QMenu *menu;
    QMenu *menuWindow;
    QMenu *menuView;
    QMenu *menuLanguage;
    QStatusBar *statusbar;
    QDockWidget *dockWidget;
    QWidget *dockWidgetContents;
    QGridLayout *gridLayout_10;
    QGridLayout *gridLayout_7;
    QGridLayout *gridLayout_4;
    QGridLayout *gridLayout_2;
    QListWidget *lwServersList;
    QLabel *lServersList;
    QLabel *lPlayersList;
    QListWidget *lwPlayersList;
    QGridLayout *gridLayout_6;
    QGridLayout *gridLayout_8;
    QLabel *lPort;
    QLabel *lPlayerName;
    QLineEdit *lePlayerName;
    QSpinBox *sbPort;
    QPushButton *pbConnect;
    QLineEdit *leServerAddress;
    QCheckBox *cbCreateServer;
    QSpinBox *sbPlayersCount;
    QSpacerItem *verticalSpacer;
    QLabel *lServerAddress;
    QComboBox *cbColor;
    QLabel *lColor;
    QLabel *lPlayersCount;
    QTextEdit *textEdit;
    QLineEdit *lineEdit;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1182, 670);
        actionQuit = new QAction(MainWindow);
        actionQuit->setObjectName(QString::fromUtf8("actionQuit"));
        actionDisconnectFromServer = new QAction(MainWindow);
        actionDisconnectFromServer->setObjectName(QString::fromUtf8("actionDisconnectFromServer"));
        actionStartGame = new QAction(MainWindow);
        actionStartGame->setObjectName(QString::fromUtf8("actionStartGame"));
        actionConnectionWindow = new QAction(MainWindow);
        actionConnectionWindow->setObjectName(QString::fromUtf8("actionConnectionWindow"));
        actionConnectionWindow->setCheckable(true);
        actionConnectionWindow->setChecked(true);
        actionConnect = new QAction(MainWindow);
        actionConnect->setObjectName(QString::fromUtf8("actionConnect"));
        actionEnglish = new QAction(MainWindow);
        actionEnglish->setObjectName(QString::fromUtf8("actionEnglish"));
        actionRussian = new QAction(MainWindow);
        actionRussian->setObjectName(QString::fromUtf8("actionRussian"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout_11 = new QGridLayout(centralwidget);
        gridLayout_11->setObjectName(QString::fromUtf8("gridLayout_11"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setSizeConstraint(QLayout::SetFixedSize);
        score1 = new QLCDNumber(centralwidget);
        score1->setObjectName(QString::fromUtf8("score1"));
        score1->setFrameShape(QFrame::Box);
        score1->setFrameShadow(QFrame::Raised);
        score1->setSegmentStyle(QLCDNumber::Outline);

        gridLayout->addWidget(score1, 0, 0, 1, 1);

        score3 = new QLCDNumber(centralwidget);
        score3->setObjectName(QString::fromUtf8("score3"));
        score3->setFrameShape(QFrame::Box);
        score3->setFrameShadow(QFrame::Raised);
        score3->setSegmentStyle(QLCDNumber::Outline);

        gridLayout->addWidget(score3, 0, 2, 1, 1);

        gvPlayer1 = new QGraphicsView(centralwidget);
        gvPlayer1->setObjectName(QString::fromUtf8("gvPlayer1"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(gvPlayer1->sizePolicy().hasHeightForWidth());
        gvPlayer1->setSizePolicy(sizePolicy);

        gridLayout->addWidget(gvPlayer1, 1, 0, 1, 1);

        gvPlayer3 = new QGraphicsView(centralwidget);
        gvPlayer3->setObjectName(QString::fromUtf8("gvPlayer3"));
        sizePolicy.setHeightForWidth(gvPlayer3->sizePolicy().hasHeightForWidth());
        gvPlayer3->setSizePolicy(sizePolicy);

        gridLayout->addWidget(gvPlayer3, 1, 2, 1, 1);

        score2 = new QLCDNumber(centralwidget);
        score2->setObjectName(QString::fromUtf8("score2"));
        score2->setFrameShape(QFrame::Box);
        score2->setFrameShadow(QFrame::Raised);
        score2->setSegmentStyle(QLCDNumber::Outline);

        gridLayout->addWidget(score2, 2, 0, 1, 1);

        score4 = new QLCDNumber(centralwidget);
        score4->setObjectName(QString::fromUtf8("score4"));
        score4->setFrameShape(QFrame::Box);
        score4->setFrameShadow(QFrame::Raised);
        score4->setSegmentStyle(QLCDNumber::Outline);

        gridLayout->addWidget(score4, 2, 2, 1, 1);

        gvPlayer2 = new QGraphicsView(centralwidget);
        gvPlayer2->setObjectName(QString::fromUtf8("gvPlayer2"));
        sizePolicy.setHeightForWidth(gvPlayer2->sizePolicy().hasHeightForWidth());
        gvPlayer2->setSizePolicy(sizePolicy);

        gridLayout->addWidget(gvPlayer2, 3, 0, 2, 1);

        gvTable = new QGraphicsView(centralwidget);
        gvTable->setObjectName(QString::fromUtf8("gvTable"));
        sizePolicy.setHeightForWidth(gvTable->sizePolicy().hasHeightForWidth());
        gvTable->setSizePolicy(sizePolicy);

        gridLayout->addWidget(gvTable, 0, 1, 4, 1);

        gvPlayer4 = new QGraphicsView(centralwidget);
        gvPlayer4->setObjectName(QString::fromUtf8("gvPlayer4"));
        sizePolicy.setHeightForWidth(gvPlayer4->sizePolicy().hasHeightForWidth());
        gvPlayer4->setSizePolicy(sizePolicy);

        gridLayout->addWidget(gvPlayer4, 3, 2, 2, 1);

        pbSurrender = new QPushButton(centralwidget);
        pbSurrender->setObjectName(QString::fromUtf8("pbSurrender"));
        pbSurrender->setEnabled(false);

        gridLayout->addWidget(pbSurrender, 4, 1, 1, 1);


        gridLayout_11->addLayout(gridLayout, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1182, 27));
        menu = new QMenu(menubar);
        menu->setObjectName(QString::fromUtf8("menu"));
        menuWindow = new QMenu(menubar);
        menuWindow->setObjectName(QString::fromUtf8("menuWindow"));
        menuView = new QMenu(menubar);
        menuView->setObjectName(QString::fromUtf8("menuView"));
        menuLanguage = new QMenu(menuView);
        menuLanguage->setObjectName(QString::fromUtf8("menuLanguage"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);
        dockWidget = new QDockWidget(MainWindow);
        dockWidget->setObjectName(QString::fromUtf8("dockWidget"));
        dockWidget->setFloating(true);
        dockWidget->setFeatures(QDockWidget::AllDockWidgetFeatures);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));
        gridLayout_10 = new QGridLayout(dockWidgetContents);
        gridLayout_10->setObjectName(QString::fromUtf8("gridLayout_10"));
        gridLayout_7 = new QGridLayout();
        gridLayout_7->setObjectName(QString::fromUtf8("gridLayout_7"));
        gridLayout_7->setSizeConstraint(QLayout::SetMaximumSize);
        gridLayout_4 = new QGridLayout();
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        lwServersList = new QListWidget(dockWidgetContents);
        lwServersList->setObjectName(QString::fromUtf8("lwServersList"));

        gridLayout_2->addWidget(lwServersList, 1, 0, 1, 1);

        lServersList = new QLabel(dockWidgetContents);
        lServersList->setObjectName(QString::fromUtf8("lServersList"));

        gridLayout_2->addWidget(lServersList, 0, 0, 1, 1);

        lPlayersList = new QLabel(dockWidgetContents);
        lPlayersList->setObjectName(QString::fromUtf8("lPlayersList"));

        gridLayout_2->addWidget(lPlayersList, 3, 0, 1, 1);

        lwPlayersList = new QListWidget(dockWidgetContents);
        lwPlayersList->setObjectName(QString::fromUtf8("lwPlayersList"));

        gridLayout_2->addWidget(lwPlayersList, 4, 0, 1, 1);


        gridLayout_4->addLayout(gridLayout_2, 0, 0, 1, 1);


        gridLayout_7->addLayout(gridLayout_4, 0, 0, 1, 1);

        gridLayout_6 = new QGridLayout();
        gridLayout_6->setObjectName(QString::fromUtf8("gridLayout_6"));
        gridLayout_8 = new QGridLayout();
        gridLayout_8->setObjectName(QString::fromUtf8("gridLayout_8"));
        lPort = new QLabel(dockWidgetContents);
        lPort->setObjectName(QString::fromUtf8("lPort"));

        gridLayout_8->addWidget(lPort, 5, 0, 1, 1);

        lPlayerName = new QLabel(dockWidgetContents);
        lPlayerName->setObjectName(QString::fromUtf8("lPlayerName"));

        gridLayout_8->addWidget(lPlayerName, 8, 0, 1, 1);

        lePlayerName = new QLineEdit(dockWidgetContents);
        lePlayerName->setObjectName(QString::fromUtf8("lePlayerName"));

        gridLayout_8->addWidget(lePlayerName, 9, 0, 1, 1);

        sbPort = new QSpinBox(dockWidgetContents);
        sbPort->setObjectName(QString::fromUtf8("sbPort"));
        sbPort->setMinimum(1);
        sbPort->setMaximum(65535);
        sbPort->setValue(1500);

        gridLayout_8->addWidget(sbPort, 6, 0, 1, 1);

        pbConnect = new QPushButton(dockWidgetContents);
        pbConnect->setObjectName(QString::fromUtf8("pbConnect"));

        gridLayout_8->addWidget(pbConnect, 13, 0, 1, 1);

        leServerAddress = new QLineEdit(dockWidgetContents);
        leServerAddress->setObjectName(QString::fromUtf8("leServerAddress"));

        gridLayout_8->addWidget(leServerAddress, 1, 0, 1, 1);

        cbCreateServer = new QCheckBox(dockWidgetContents);
        cbCreateServer->setObjectName(QString::fromUtf8("cbCreateServer"));

        gridLayout_8->addWidget(cbCreateServer, 2, 0, 1, 1);

        sbPlayersCount = new QSpinBox(dockWidgetContents);
        sbPlayersCount->setObjectName(QString::fromUtf8("sbPlayersCount"));
        sbPlayersCount->setEnabled(false);
        sbPlayersCount->setMinimum(3);
        sbPlayersCount->setMaximum(4);
        sbPlayersCount->setValue(4);

        gridLayout_8->addWidget(sbPlayersCount, 4, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_8->addItem(verticalSpacer, 12, 0, 1, 1);

        lServerAddress = new QLabel(dockWidgetContents);
        lServerAddress->setObjectName(QString::fromUtf8("lServerAddress"));

        gridLayout_8->addWidget(lServerAddress, 0, 0, 1, 1);

        cbColor = new QComboBox(dockWidgetContents);
        cbColor->setObjectName(QString::fromUtf8("cbColor"));

        gridLayout_8->addWidget(cbColor, 11, 0, 1, 1);

        lColor = new QLabel(dockWidgetContents);
        lColor->setObjectName(QString::fromUtf8("lColor"));

        gridLayout_8->addWidget(lColor, 10, 0, 1, 1);

        lPlayersCount = new QLabel(dockWidgetContents);
        lPlayersCount->setObjectName(QString::fromUtf8("lPlayersCount"));
        lPlayersCount->setEnabled(false);

        gridLayout_8->addWidget(lPlayersCount, 3, 0, 1, 1);


        gridLayout_6->addLayout(gridLayout_8, 4, 0, 1, 1);


        gridLayout_7->addLayout(gridLayout_6, 0, 1, 1, 1);


        gridLayout_10->addLayout(gridLayout_7, 2, 0, 1, 1);

        textEdit = new QTextEdit(dockWidgetContents);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));
        textEdit->setReadOnly(true);

        gridLayout_10->addWidget(textEdit, 4, 0, 1, 1);

        lineEdit = new QLineEdit(dockWidgetContents);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));

        gridLayout_10->addWidget(lineEdit, 5, 0, 1, 1);

        dockWidget->setWidget(dockWidgetContents);
        MainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(1), dockWidget);

        menubar->addAction(menu->menuAction());
        menubar->addAction(menuView->menuAction());
        menubar->addAction(menuWindow->menuAction());
        menu->addAction(actionStartGame);
        menu->addAction(actionConnect);
        menu->addSeparator();
        menu->addAction(actionQuit);
        menuWindow->addAction(actionConnectionWindow);
        menuView->addAction(menuLanguage->menuAction());
        menuLanguage->addAction(actionEnglish);
        menuLanguage->addAction(actionRussian);

        retranslateUi(MainWindow);
        QObject::connect(actionConnectionWindow, SIGNAL(toggled(bool)), dockWidget, SLOT(setVisible(bool)));
        QObject::connect(cbCreateServer, SIGNAL(toggled(bool)), lServerAddress, SLOT(setDisabled(bool)));
        QObject::connect(cbCreateServer, SIGNAL(toggled(bool)), leServerAddress, SLOT(setDisabled(bool)));
        QObject::connect(cbCreateServer, SIGNAL(toggled(bool)), sbPlayersCount, SLOT(setEnabled(bool)));
        QObject::connect(dockWidget, SIGNAL(visibilityChanged(bool)), actionConnectionWindow, SLOT(setChecked(bool)));
        QObject::connect(cbCreateServer, SIGNAL(toggled(bool)), lServersList, SLOT(setDisabled(bool)));
        QObject::connect(cbCreateServer, SIGNAL(toggled(bool)), lwServersList, SLOT(setDisabled(bool)));
        QObject::connect(cbCreateServer, SIGNAL(toggled(bool)), lPlayersCount, SLOT(setEnabled(bool)));
        QObject::connect(lePlayerName, SIGNAL(returnPressed()), pbConnect, SLOT(animateClick()));
        QObject::connect(lePlayerName, SIGNAL(returnPressed()), pbConnect, SLOT(setFocus()));
        QObject::connect(leServerAddress, SIGNAL(returnPressed()), pbConnect, SLOT(animateClick()));
        QObject::connect(leServerAddress, SIGNAL(returnPressed()), pbConnect, SLOT(setFocus()));

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "QBLOKUS", 0, QApplication::UnicodeUTF8));
        actionQuit->setText(QApplication::translate("MainWindow", "Quit", 0, QApplication::UnicodeUTF8));
        actionDisconnectFromServer->setText(QApplication::translate("MainWindow", "Disconnect from the server", 0, QApplication::UnicodeUTF8));
        actionStartGame->setText(QApplication::translate("MainWindow", "Start a new game", 0, QApplication::UnicodeUTF8));
        actionConnectionWindow->setText(QApplication::translate("MainWindow", "Connection window", 0, QApplication::UnicodeUTF8));
        actionConnect->setText(QApplication::translate("MainWindow", "Connect to the server", 0, QApplication::UnicodeUTF8));
        actionEnglish->setText(QApplication::translate("MainWindow", "English", 0, QApplication::UnicodeUTF8));
        actionRussian->setText(QApplication::translate("MainWindow", "Russian", 0, QApplication::UnicodeUTF8));
        pbSurrender->setText(QApplication::translate("MainWindow", "Give up!", 0, QApplication::UnicodeUTF8));
        menu->setTitle(QApplication::translate("MainWindow", "Menu", 0, QApplication::UnicodeUTF8));
        menuWindow->setTitle(QApplication::translate("MainWindow", "Window", 0, QApplication::UnicodeUTF8));
        menuView->setTitle(QApplication::translate("MainWindow", "View", 0, QApplication::UnicodeUTF8));
        menuLanguage->setTitle(QApplication::translate("MainWindow", "Language", 0, QApplication::UnicodeUTF8));
        lServersList->setText(QApplication::translate("MainWindow", "Local servers list", 0, QApplication::UnicodeUTF8));
        lPlayersList->setText(QApplication::translate("MainWindow", "Players list", 0, QApplication::UnicodeUTF8));
        lPort->setText(QApplication::translate("MainWindow", "Port", 0, QApplication::UnicodeUTF8));
        lPlayerName->setText(QApplication::translate("MainWindow", "Player name", 0, QApplication::UnicodeUTF8));
        pbConnect->setText(QApplication::translate("MainWindow", "Connect to the server", 0, QApplication::UnicodeUTF8));
        leServerAddress->setText(QApplication::translate("MainWindow", "localhost", 0, QApplication::UnicodeUTF8));
        cbCreateServer->setText(QApplication::translate("MainWindow", "Create server", 0, QApplication::UnicodeUTF8));
        lServerAddress->setText(QApplication::translate("MainWindow", "Server address", 0, QApplication::UnicodeUTF8));
        cbColor->clear();
        cbColor->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "Red", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "Yellow", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "Green", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "Blue", 0, QApplication::UnicodeUTF8)
        );
        lColor->setText(QApplication::translate("MainWindow", "Color", 0, QApplication::UnicodeUTF8));
        lPlayersCount->setText(QApplication::translate("MainWindow", "Players count", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RACINGFORM_H
