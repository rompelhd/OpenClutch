#pragma once

#include <QListWidgetItem>
#include <QDialog>
#include <string>
#include <map>

class QListWidget;
class QLabel;
class QComboBox;
class ACManager;

class CarSelectorDialog : public QDialog
{
    Q_OBJECT

public:

    CarSelectorDialog(ACManager* ac, QWidget* parent=nullptr);

    std::string getSelectedCar();
    std::string getSelectedSkin();

private:

    ACManager* acManager;

    QListWidget* carsList;

    QLabel* carPreview;
    QLabel* carInfo;

    QComboBox* brandFilter;

    std::string selectedCar;
    std::string selectedSkin;

    std::map<QListWidgetItem*,std::string> brandMap;

    void loadCars();
    void updateInfo();
};