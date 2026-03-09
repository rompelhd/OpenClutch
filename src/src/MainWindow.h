#pragma once

#include <QMainWindow>
#include <QStackedWidget>

class SinglePlayerScreen;
class ACManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private:

    ACManager* acManager;

    QStackedWidget* stacked;

    SinglePlayerScreen* singleScreen;
    QWidget* onlineScreen;
};