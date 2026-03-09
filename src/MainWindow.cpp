#include "MainWindow.h"
#include "SinglePlayerScreen.h"
#include "ACManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    acManager =
        new ACManager(
            std::string(getenv("HOME"))
            + "/.steam/steam/steamapps/common/assettocorsa"
        );

    acManager->loadConfig();

    QWidget* central = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout;

    // TOP MENU

    QHBoxLayout* menu = new QHBoxLayout;

    QPushButton* singleBtn = new QPushButton("Single");
    QPushButton* onlineBtn = new QPushButton("Online");

    menu->addWidget(singleBtn);
    menu->addWidget(onlineBtn);
    menu->addStretch();

    layout->addLayout(menu);

    // SCREENS

    stacked = new QStackedWidget;

    singleScreen = new SinglePlayerScreen(acManager);
    onlineScreen = new QWidget;

    stacked->addWidget(singleScreen);
    stacked->addWidget(onlineScreen);

    layout->addWidget(stacked);

    connect(singleBtn,&QPushButton::clicked,[=]{
        stacked->setCurrentWidget(singleScreen);
    });

    connect(onlineBtn,&QPushButton::clicked,[=]{
        stacked->setCurrentWidget(onlineScreen);
    });

    central->setLayout(layout);
    setCentralWidget(central);

    setWindowTitle("OpenClutch");
}