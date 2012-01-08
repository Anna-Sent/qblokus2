/********************************************************************************
** Form generated from reading UI file 'racingForm.ui'
**
** Created: Sun Jan 8 21:09:50 2012
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
#include <QtGui/QFrame>
#include <QtGui/QGraphicsView>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
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
    QAction *actionExit;
    QAction *actionDisconnectFromServer;
    QAction *actionReconnectToServer;
    QAction *actionStartGame;
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
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
    QPushButton *btnSurrender;
    QMenuBar *menubar;
    QMenu *menu;
    QStatusBar *statusbar;
    QDockWidget *dockWidget;
    QWidget *dockWidgetContents;
    QGridLayout *gridLayout_9;
    QLineEdit *lineEdit;
    QListWidget *listWidget;
    QFrame *line;
    QTextEdit *textEdit;
    QFrame *line_2;
    QDockWidget *dockWidget_3;
    QWidget *dockWidgetContents_3;
    QGridLayout *gridLayout_10;
    QGridLayout *gridLayout_7;
    QGridLayout *gridLayout_4;
    QGridLayout *gridLayout_2;
    QListWidget *lwServersList;
    QGridLayout *gridLayout_3;
    QCheckBox *cbCreateServer;
    QSpinBox *sbClientsCount;
    QSpacerItem *horizontalSpacer;
    QLineEdit *leServerIP;
    QGridLayout *gridLayout_6;
    QGridLayout *gridLayout_5;
    QPushButton *pbSearch;
    QPushButton *pbConnect;
    QGridLayout *gridLayout_8;
    QLabel *lPort;
    QLabel *lNickname;
    QLineEdit *leNickname;
    QSpinBox *sbPort;
    QSpacerItem *verticalSpacer;
    QComboBox *comboBox;
    QTextEdit *textEdit_2;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1182, 590);
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        actionDisconnectFromServer = new QAction(MainWindow);
        actionDisconnectFromServer->setObjectName(QString::fromUtf8("actionDisconnectFromServer"));
        actionReconnectToServer = new QAction(MainWindow);
        actionReconnectToServer->setObjectName(QString::fromUtf8("actionReconnectToServer"));
        actionStartGame = new QAction(MainWindow);
        actionStartGame->setObjectName(QString::fromUtf8("actionStartGame"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setSizeConstraint(QLayout::SetFixedSize);
        score1 = new QLCDNumber(centralwidget);
        score1->setObjectName(QString::fromUtf8("score1"));

        gridLayout->addWidget(score1, 0, 0, 1, 1);

        score3 = new QLCDNumber(centralwidget);
        score3->setObjectName(QString::fromUtf8("score3"));

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

        gridLayout->addWidget(score2, 2, 0, 1, 1);

        score4 = new QLCDNumber(centralwidget);
        score4->setObjectName(QString::fromUtf8("score4"));

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

        btnSurrender = new QPushButton(centralwidget);
        btnSurrender->setObjectName(QString::fromUtf8("btnSurrender"));

        gridLayout->addWidget(btnSurrender, 4, 1, 1, 1);


        horizontalLayout->addLayout(gridLayout);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1182, 23));
        menu = new QMenu(menubar);
        menu->setObjectName(QString::fromUtf8("menu"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);
        dockWidget = new QDockWidget(MainWindow);
        dockWidget->setObjectName(QString::fromUtf8("dockWidget"));
        dockWidget->setFloating(false);
        dockWidget->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));
        gridLayout_9 = new QGridLayout(dockWidgetContents);
        gridLayout_9->setObjectName(QString::fromUtf8("gridLayout_9"));
        lineEdit = new QLineEdit(dockWidgetContents);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));

        gridLayout_9->addWidget(lineEdit, 4, 0, 1, 1);

        listWidget = new QListWidget(dockWidgetContents);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));

        gridLayout_9->addWidget(listWidget, 0, 0, 1, 1);

        line = new QFrame(dockWidgetContents);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        gridLayout_9->addWidget(line, 1, 0, 1, 1);

        textEdit = new QTextEdit(dockWidgetContents);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));
        textEdit->setReadOnly(true);

        gridLayout_9->addWidget(textEdit, 2, 0, 1, 1);

        line_2 = new QFrame(dockWidgetContents);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);

        gridLayout_9->addWidget(line_2, 3, 0, 1, 1);

        dockWidget->setWidget(dockWidgetContents);
        MainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(2), dockWidget);
        dockWidget_3 = new QDockWidget(MainWindow);
        dockWidget_3->setObjectName(QString::fromUtf8("dockWidget_3"));
        dockWidgetContents_3 = new QWidget();
        dockWidgetContents_3->setObjectName(QString::fromUtf8("dockWidgetContents_3"));
        gridLayout_10 = new QGridLayout(dockWidgetContents_3);
        gridLayout_10->setObjectName(QString::fromUtf8("gridLayout_10"));
        gridLayout_7 = new QGridLayout();
        gridLayout_7->setObjectName(QString::fromUtf8("gridLayout_7"));
        gridLayout_7->setSizeConstraint(QLayout::SetMaximumSize);
        gridLayout_4 = new QGridLayout();
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        lwServersList = new QListWidget(dockWidgetContents_3);
        lwServersList->setObjectName(QString::fromUtf8("lwServersList"));

        gridLayout_2->addWidget(lwServersList, 0, 0, 1, 1);

        gridLayout_3 = new QGridLayout();
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        cbCreateServer = new QCheckBox(dockWidgetContents_3);
        cbCreateServer->setObjectName(QString::fromUtf8("cbCreateServer"));

        gridLayout_3->addWidget(cbCreateServer, 0, 0, 1, 1);

        sbClientsCount = new QSpinBox(dockWidgetContents_3);
        sbClientsCount->setObjectName(QString::fromUtf8("sbClientsCount"));
        sbClientsCount->setEnabled(false);
        sbClientsCount->setMinimum(3);
        sbClientsCount->setMaximum(4);
        sbClientsCount->setValue(4);

        gridLayout_3->addWidget(sbClientsCount, 1, 0, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_3->addItem(horizontalSpacer, 1, 1, 1, 1);


        gridLayout_2->addLayout(gridLayout_3, 2, 0, 1, 1);

        leServerIP = new QLineEdit(dockWidgetContents_3);
        leServerIP->setObjectName(QString::fromUtf8("leServerIP"));

        gridLayout_2->addWidget(leServerIP, 1, 0, 1, 1);


        gridLayout_4->addLayout(gridLayout_2, 0, 0, 1, 1);


        gridLayout_7->addLayout(gridLayout_4, 0, 0, 1, 1);

        gridLayout_6 = new QGridLayout();
        gridLayout_6->setObjectName(QString::fromUtf8("gridLayout_6"));
        gridLayout_5 = new QGridLayout();
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        pbSearch = new QPushButton(dockWidgetContents_3);
        pbSearch->setObjectName(QString::fromUtf8("pbSearch"));

        gridLayout_5->addWidget(pbSearch, 0, 0, 1, 1);

        pbConnect = new QPushButton(dockWidgetContents_3);
        pbConnect->setObjectName(QString::fromUtf8("pbConnect"));

        gridLayout_5->addWidget(pbConnect, 1, 0, 1, 1);


        gridLayout_6->addLayout(gridLayout_5, 0, 0, 1, 1);

        gridLayout_8 = new QGridLayout();
        gridLayout_8->setObjectName(QString::fromUtf8("gridLayout_8"));
        lPort = new QLabel(dockWidgetContents_3);
        lPort->setObjectName(QString::fromUtf8("lPort"));

        gridLayout_8->addWidget(lPort, 0, 0, 1, 1);

        lNickname = new QLabel(dockWidgetContents_3);
        lNickname->setObjectName(QString::fromUtf8("lNickname"));

        gridLayout_8->addWidget(lNickname, 2, 0, 1, 1);

        leNickname = new QLineEdit(dockWidgetContents_3);
        leNickname->setObjectName(QString::fromUtf8("leNickname"));

        gridLayout_8->addWidget(leNickname, 3, 0, 1, 1);

        sbPort = new QSpinBox(dockWidgetContents_3);
        sbPort->setObjectName(QString::fromUtf8("sbPort"));
        sbPort->setMinimum(1024);
        sbPort->setMaximum(65535);
        sbPort->setValue(1500);

        gridLayout_8->addWidget(sbPort, 1, 0, 1, 1);


        gridLayout_6->addLayout(gridLayout_8, 4, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_6->addItem(verticalSpacer, 2, 0, 1, 1);

        comboBox = new QComboBox(dockWidgetContents_3);
        comboBox->setObjectName(QString::fromUtf8("comboBox"));

        gridLayout_6->addWidget(comboBox, 3, 0, 1, 1);

        textEdit_2 = new QTextEdit(dockWidgetContents_3);
        textEdit_2->setObjectName(QString::fromUtf8("textEdit_2"));
        textEdit_2->setReadOnly(true);

        gridLayout_6->addWidget(textEdit_2, 1, 0, 1, 1);


        gridLayout_7->addLayout(gridLayout_6, 0, 1, 1, 1);


        gridLayout_10->addLayout(gridLayout_7, 0, 0, 1, 1);

        dockWidget_3->setWidget(dockWidgetContents_3);
        MainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(2), dockWidget_3);

        menubar->addAction(menu->menuAction());
        menu->addAction(actionStartGame);
        menu->addAction(actionDisconnectFromServer);
        menu->addSeparator();
        menu->addAction(actionExit);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "QBLOKUS", 0, QApplication::UnicodeUTF8));
        actionExit->setText(QApplication::translate("MainWindow", "\320\222\321\213\321\205\320\276\320\264", 0, QApplication::UnicodeUTF8));
        actionDisconnectFromServer->setText(QApplication::translate("MainWindow", "\320\236\321\202\320\272\320\273\321\216\321\207\320\270\321\202\321\201\321\217 \320\276\321\202 \321\201\320\265\321\200\320\262\320\265\321\200\320\260", 0, QApplication::UnicodeUTF8));
        actionReconnectToServer->setText(QApplication::translate("MainWindow", "\320\237\320\265\321\200\320\265\320\277\320\276\320\264\320\272\320\273\321\216\321\207\320\270\321\202\321\214\321\201\321\217 \320\272 \321\201\320\265\321\200\320\262\320\265\321\200\321\203", 0, QApplication::UnicodeUTF8));
        actionStartGame->setText(QApplication::translate("MainWindow", "\320\235\320\260\321\207\320\260\321\202\321\214 \320\270\320\263\321\200\321\203", 0, QApplication::UnicodeUTF8));
        btnSurrender->setText(QApplication::translate("MainWindow", "\320\241\320\264\320\260\321\202\321\214\321\201\321\217!", 0, QApplication::UnicodeUTF8));
        menu->setTitle(QApplication::translate("MainWindow", "\320\234\320\265\320\275\321\216", 0, QApplication::UnicodeUTF8));
        cbCreateServer->setText(QApplication::translate("MainWindow", "Create server", 0, QApplication::UnicodeUTF8));
        leServerIP->setText(QApplication::translate("MainWindow", "localhost", 0, QApplication::UnicodeUTF8));
        pbSearch->setText(QApplication::translate("MainWindow", "Start search", 0, QApplication::UnicodeUTF8));
        pbConnect->setText(QApplication::translate("MainWindow", "Connect to server", 0, QApplication::UnicodeUTF8));
        lPort->setText(QApplication::translate("MainWindow", "Port", 0, QApplication::UnicodeUTF8));
        lNickname->setText(QApplication::translate("MainWindow", "Nickname", 0, QApplication::UnicodeUTF8));
        comboBox->clear();
        comboBox->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "\320\232\321\200\320\260\321\201\320\275\321\213\320\271", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "\320\226\320\265\320\273\321\202\321\213\320\271", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "\320\227\320\265\320\273\320\265\320\275\321\213\320\271", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "\320\241\320\270\320\275\320\270\320\271", 0, QApplication::UnicodeUTF8)
        );
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RACINGFORM_H
