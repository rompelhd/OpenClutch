#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QPushButton>
#include <QTableWidget>
#include <QComboBox>
#include "ACManager.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void onLaunchClicked();

private:
    ACManager* acManager;

    QListWidget* carsList;
    QListWidget* tracksList;
    QTableWidget* carInfoTable;
    QPushButton* launchButton;

    QComboBox* brandFilter;
    QComboBox* classFilter;
};

#endif // MAINWINDOW_H