#pragma once

#include <QWidget>
#include <string>
#include <QCheckBox>

class QPushButton;
class QLabel;
class QSlider;
class QComboBox;
class ACManager;

class SinglePlayerScreen : public QWidget
{
    Q_OBJECT

public:

    SinglePlayerScreen(ACManager* ac, QWidget* parent=nullptr);

private:

    ACManager* acManager;

    // ---------------- CAR ----------------
    QPushButton* carButton;
    QLabel* carImage;
    QLabel* carName;

    // ---------------- TRACK ----------------
    QPushButton* trackButton;
    QLabel* trackImage;
    QLabel* trackName;

    // ---------------- TIME ----------------
    QLabel* timeLabel;
    QSlider* timeSlider;

    QCheckBox* realConditionsCheck;
    QCheckBox* idealConditionsCheck;

    // ---------------- WEATHER ----------------
    QLabel* weatherLabel;
    QComboBox* weatherCombo;
    QLabel* tempLabel;
    QSlider* tempSlider;

    // ---------------- SELECTION DATA ----------------
    std::string selectedCar;
    std::string selectedSkin;

    // ---------------- METHODS ----------------
    void openCarSelector();
};