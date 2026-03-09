#pragma once

#include <QListWidgetItem>
#include <QDialog>
#include <string>
#include <map>

class QListWidget;
class QLabel;
class QComboBox;
class ACManager;

class CircuitSelectorDialog : public QDialog
{
    Q_OBJECT

public:

    CircuitSelectorDialog(ACManager* ac, QWidget* parent=nullptr);

    // Devuelve el track seleccionado
    std::string getSelectedCircuit();

private:

    ACManager* acManager;

    QListWidget* circuitsList;

    QLabel* circuitPreview;
    QLabel* circuitInfo;

    QComboBox* locationFilter;

    std::string selectedCircuit;

    std::map<QListWidgetItem*, std::string> locationMap;

    void loadCircuits();
    void updateInfo();
};