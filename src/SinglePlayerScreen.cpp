#include "SinglePlayerScreen.h"
#include "CarSelectorDialog.h"
#include "ACManager.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QComboBox>
#include <QPixmap>
#include <QCheckBox>

SinglePlayerScreen::SinglePlayerScreen(
    ACManager* ac,
    QWidget* parent
)
: QWidget(parent), acManager(ac)
{
    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addSpacing(1);

    QHBoxLayout* layout = new QHBoxLayout;
    layout->setSpacing(15);

    // ---------------- CAR BUTTON ----------------
    QVBoxLayout* carButtonLayout = new QVBoxLayout;

    carButton = new QPushButton;
    carButton->setFixedSize(220,140);

    carImage = new QLabel;
    carImage->setFixedSize(220,110);
    carImage->setScaledContents(true);

    carName = new QLabel("Select Car");
    carName->setAlignment(Qt::AlignCenter);

    carButtonLayout->addWidget(carImage);
    carButtonLayout->addWidget(carName);

    carButton->setLayout(carButtonLayout);

    QVBoxLayout* carLayout = new QVBoxLayout;

    QLabel* bestLapLabel = new QLabel("Best lap: No info");

    QLabel* bestLapTrackTitle = new QLabel("Best lap on this track:");
    QLabel* bestLapTrack = new QLabel("Ferrari gt30       01:53:141");

    QLabel* drivenCar = new QLabel("Driven in car: 100.2 km");
    QLabel* drivenTrack = new QLabel("Driven on track: 1203.2 km");

    carLayout->addWidget(carButton);
    carLayout->addSpacing(6);
    carLayout->addWidget(bestLapLabel);

    carLayout->addSpacing(8);
    carLayout->addWidget(bestLapTrackTitle);
    carLayout->addWidget(bestLapTrack);

    carLayout->addSpacing(8);
    carLayout->addWidget(drivenCar);
    carLayout->addWidget(drivenTrack);

    // -------- Profile --------
    carLayout->addSpacing(10);

    QLabel* profileLabel = new QLabel("Profile:");
    QComboBox* profileCombo = new QComboBox;
    profileCombo->setMaximumWidth(220);
    profileCombo->addItems({"Pro","Amateur","Casual"});
    carLayout->addWidget(profileLabel);
    carLayout->addWidget(profileCombo);

    carLayout->addSpacing(6);

    QCheckBox* tyreBlanketsCheck = new QCheckBox("Tyre blankets");
    QCheckBox* carSpecificControlsCheck = new QCheckBox("Car-specific controls");
    carLayout->addWidget(tyreBlanketsCheck);
    carLayout->addWidget(carSpecificControlsCheck);

    carLayout->addStretch();

    // ---------------- TRACK ----------------
    QVBoxLayout* trackLayout = new QVBoxLayout;
    trackButton = new QPushButton;
    trackButton->setFixedSize(220,140);

    trackImage = new QLabel;
    trackImage->setFixedSize(220,140);
    trackImage->setScaledContents(true);

    trackName = new QLabel("Select Track");
    trackName->setAlignment(Qt::AlignCenter);

    trackLayout->addWidget(trackImage);
    trackLayout->addStretch();
    trackLayout->addWidget(trackName);
    trackButton->setLayout(trackLayout);

    // ---------------- TIME ----------------
    QVBoxLayout* timeLayout = new QVBoxLayout;
    timeLabel = new QLabel("Time 12:00");

    timeSlider = new QSlider(Qt::Horizontal);
    timeSlider->setRange(0,24);
    timeSlider->setValue(12);
    timeSlider->setMaximumWidth(220);

    connect(timeSlider, &QSlider::valueChanged, [this](int v){
        QString t = QString("Time %1:00").arg(v,2,10,QChar('0'));
        timeLabel->setText(t);
    });

    // ---------------- CHECKBOXES ----------------
    realConditionsCheck = new QCheckBox("Real conditions");
    idealConditionsCheck = new QCheckBox("Ideal conditions");

    idealConditionsCheck->setMaximumWidth(220);
    realConditionsCheck->setMaximumWidth(220);

    connect(realConditionsCheck, &QCheckBox::checkStateChanged, [this](Qt::CheckState state){
        if(state == Qt::Checked)
            idealConditionsCheck->setChecked(false);
    });

    connect(idealConditionsCheck, &QCheckBox::checkStateChanged, [this](Qt::CheckState state){
        if(state == Qt::Checked)
            realConditionsCheck->setChecked(false);
    });

    QVBoxLayout* checkLayout = new QVBoxLayout;
    checkLayout->addWidget(realConditionsCheck);
    checkLayout->addWidget(idealConditionsCheck);

    timeLayout->setSpacing(13);

    timeLayout->addWidget(timeLabel);
    timeLayout->addWidget(timeSlider);
    timeLayout->addLayout(checkLayout);
    timeLayout->addStretch();
    timeLayout->setAlignment(Qt::AlignTop);
    timeLayout->setContentsMargins(0,0,0,0);

    // ---------------- TEMPERATURE ----------------
    QVBoxLayout* tempLayout = new QVBoxLayout;

    tempLabel = new QLabel("Temp 20°C");

    tempSlider = new QSlider(Qt::Horizontal);
    tempSlider->setRange(-10,35);
    tempSlider->setValue(20);
    tempSlider->setMaximumWidth(220);

    connect(tempSlider, &QSlider::valueChanged, [this](int v){
        tempLabel->setText(QString("Temp %1°C").arg(v));
    });

    tempLayout->addWidget(tempLabel);
    tempLayout->addWidget(tempSlider);

    // ---------------- WEATHER ----------------
    QVBoxLayout* weatherLayout = new QVBoxLayout;
    weatherLabel = new QLabel("Weather");

    weatherCombo = new QComboBox;
    weatherCombo->setMaximumWidth(220);
    weatherCombo->setMinimumWidth(220);
    weatherCombo->addItems({"Clear","Cloudy","Light Clouds","Heavy Clouds","Fog"});

    weatherLayout->addWidget(weatherLabel);
    weatherLayout->addWidget(weatherCombo);
    weatherLayout->addLayout(tempLayout);

    weatherLayout->addStretch();
    weatherLayout->setAlignment(Qt::AlignTop);
    weatherLayout->setContentsMargins(0,0,0,0);

    // ---------------- TRACK CONDITIONS ----------------
    QVBoxLayout* trackCondLayout = new QVBoxLayout;

    QLabel* trackCondLabel = new QLabel("Track");

    QComboBox* trackCondCombo = new QComboBox;
    trackCondCombo->setMaximumWidth(220);
    trackCondCombo->addItems({"Green","Fast","Optimum","Greasy","Dusty"});

    trackCondLayout->addWidget(trackCondLabel);
    trackCondLayout->addWidget(trackCondCombo);

    QLabel* windLabel = new QLabel("Wind 16.6 km/h");

    QSlider* windSlider = new QSlider(Qt::Horizontal);
    windSlider->setRange(0,40);
    windSlider->setValue(16);
    windSlider->setMaximumWidth(220);

    connect(windSlider,&QSlider::valueChanged,[windLabel](int v){
        windLabel->setText(QString("Wind %1 km/h").arg(v));
    });

    trackCondLayout->addWidget(windLabel);
    trackCondLayout->addWidget(windSlider);

    trackCondLayout->addStretch();
    trackCondLayout->setAlignment(Qt::AlignTop);

    // ---------------- MAIN LAYOUT ----------------
    layout->addLayout(carLayout);
    layout->addWidget(trackButton, 0, Qt::AlignTop);
    layout->addLayout(timeLayout);
    layout->addLayout(weatherLayout);
    layout->addLayout(trackCondLayout);
    layout->addStretch();

    mainLayout->addLayout(layout);
    mainLayout->addStretch();

    setLayout(mainLayout);

    connect(carButton, &QPushButton::clicked, this, &SinglePlayerScreen::openCarSelector);
}

void SinglePlayerScreen::openCarSelector()
{
    CarSelectorDialog dlg(acManager,this);

    if(dlg.exec() == QDialog::Accepted)
    {
        selectedCar = dlg.getSelectedCar();
        selectedSkin = dlg.getSelectedSkin();

        std::string preview =
            acManager->getACPath() + "/content/cars/" +
            selectedCar + "/skins/" + selectedSkin + "/preview.jpg";

        QPixmap pix(QString::fromStdString(preview));
        carImage->setPixmap(pix);
        carName->setText(QString::fromStdString(selectedCar));
    }
}