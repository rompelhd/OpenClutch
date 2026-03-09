#pragma once

#include <QDialog>
#include <QListWidget>
#include <string>

class ACManager;

class SkinSelectorDialog : public QDialog
{
    Q_OBJECT

public:

    SkinSelectorDialog(
        ACManager* ac,
        const std::string& car,
        QWidget* parent = nullptr
    );

    std::string getSelectedSkin();

private:

    void loadSkins();

    ACManager* acManager;
    std::string carName;

    QListWidget* skinsList;

    std::string selectedSkin;
};