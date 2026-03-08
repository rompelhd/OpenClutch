#include "MainWindow.h"
#include "SettingsDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QIcon>
#include <QComboBox>
#include <QListWidgetItem>
#include <QDialog>
#include <QLabel>
#include <QScrollArea>

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

#include <filesystem>
#include <algorithm>
#include <set>

using json = nlohmann::json;

// -------------------------------------------------------------

class CarInfoDialog : public QDialog {
public:
    CarInfoDialog(const json& j, QWidget* parent = nullptr) : QDialog(parent) {
        setWindowTitle(QString::fromStdString(j.value("name", "Car Info")));
        resize(600, 400);

        QVBoxLayout* layout = new QVBoxLayout(this);
        QScrollArea* scroll = new QScrollArea(this);
        QWidget* content = new QWidget();
        QVBoxLayout* contentLayout = new QVBoxLayout(content);

        auto addLine = [&](const std::string& key, const std::string& value){
            QLabel* label = new QLabel(QString::fromStdString("<b>" + key + ":</b> " + value));
            label->setWordWrap(true);
            contentLayout->addWidget(label);
        };

        if (j.contains("name")) addLine("Name", j["name"]);
        if (j.contains("brand")) addLine("Brand", j["brand"]);
        if (j.contains("class")) addLine("Class", j["class"]);
        if (j.contains("description")) addLine("Description", j["description"]);

        if (j.contains("tags") && j["tags"].is_array()) {
            QStringList tags;
            for (auto& t : j["tags"])
                tags << QString::fromStdString(t.get<std::string>());
            addLine("Tags", tags.join(", ").toStdString());
        }

        if (j.contains("specs") && j["specs"].is_object()) {
            for (auto& [key, value] : j["specs"].items()) {
                std::string valStr;
                if (value.is_string()) valStr = value.get<std::string>();
                else if (value.is_number()) valStr = std::to_string(value.get<double>());
                else valStr = value.dump();
                addLine(key, valStr);
            }
        }

        scroll->setWidgetResizable(true);
        scroll->setWidget(content);
        layout->addWidget(scroll);

        QPushButton* closeBtn = new QPushButton("Close", this);
        connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
        layout->addWidget(closeBtn);
    }
};

// -------------------------------------------------------------

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    acManager = new ACManager(std::string(getenv("HOME")) + "/.steam/steam/steamapps/common/assettocorsa");
    acManager->loadConfig();

    QWidget* central = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout();

    // filters
    QHBoxLayout* filtersLayout = new QHBoxLayout();

    brandFilter = new QComboBox(this);
    brandFilter->addItem("All brands");

    classFilter = new QComboBox(this);
    classFilter->addItem("All classes");

    filtersLayout->addWidget(brandFilter);
    filtersLayout->addWidget(classFilter);

    mainLayout->addLayout(filtersLayout);

    // lists
    QHBoxLayout* listsLayout = new QHBoxLayout();

    carsList = new QListWidget(this);
    carsList->setIconSize(QSize(128,128));

    tracksList = new QListWidget(this);
    tracksList->setIconSize(QSize(128,128));

    listsLayout->addWidget(carsList);
    listsLayout->addWidget(tracksList);

    mainLayout->addLayout(listsLayout);

    QHBoxLayout* buttonsLayout = new QHBoxLayout();

    launchButton = new QPushButton("Launch Assetto Corsa", this);
    buttonsLayout->addWidget(launchButton);

    QPushButton* infoButton = new QPushButton("Car Info", this);
    buttonsLayout->addWidget(infoButton);

    QPushButton* settingsButton = new QPushButton("Settings", this);
    buttonsLayout->addWidget(settingsButton);

    mainLayout->addLayout(buttonsLayout);

    connect(launchButton, &QPushButton::clicked, this, &MainWindow::onLaunchClicked);

    auto applyFilters = [this]()
    {
        QString selectedBrand = brandFilter->currentText();
        QString selectedClass = classFilter->currentText();

        for (int i = 0; i < carsList->count(); i++)
        {
            QListWidgetItem* item = carsList->item(i);

            QString itemBrand = item->data(Qt::UserRole + 1).toString();
            QString itemClass = item->data(Qt::UserRole + 2).toString();

            bool brandMatch =
                (selectedBrand == "All brands") || (itemBrand == selectedBrand);

            bool classMatch =
                (selectedClass == "All classes") || (itemClass == selectedClass);

            item->setHidden(!(brandMatch && classMatch));
        }
    };

    connect(brandFilter, &QComboBox::currentTextChanged, this, applyFilters);
    connect(classFilter, &QComboBox::currentTextChanged, this, applyFilters);

    // ---------------------------------------------------------

    connect(settingsButton, &QPushButton::clicked, [this](){
        SettingsDialog dlg(this);

        dlg.setACPath(QString::fromStdString(acManager->getACPath()));
        dlg.setSteamPath(QString::fromStdString(acManager->getSteamPath()));

        if (dlg.exec() == QDialog::Accepted)
        {
            QString acPath = dlg.getACPath();
            QString steamPath = dlg.getSteamPath();

            if (!acPath.isEmpty())
                acManager->setACPath(acPath.toStdString());

            acManager->setSteamPath(steamPath.toStdString());
            acManager->saveConfig();
        }
    });

    // CAR INFO

    connect(infoButton, &QPushButton::clicked, [this](){

        QListWidgetItem* item = carsList->currentItem();
        if (!item) return;

        std::string car = item->data(Qt::UserRole).toString().toStdString();

        std::string jsonPath =
            acManager->getACPath() +
            "/content/cars/" +
            car +
            "/ui/ui_car.json";

        if (!std::filesystem::exists(jsonPath)) return;

        std::ifstream f(jsonPath);
        if (!f.is_open()) return;

        try {

            json j;
            f >> j;

            CarInfoDialog dlg(j, this);
            dlg.exec();

        } catch(...) {}

    });

    central->setLayout(mainLayout);
    setCentralWidget(central);

    setWindowTitle("OpenClutch Launcher By Rompelhd");
    resize(1000,600);

    std::set<std::string> brands;
    std::set<std::string> classes;

    for (const auto& car : acManager->listCars())
    {
        std::string jsonPath =
            acManager->getACPath() +
            "/content/cars/" +
            car +
            "/ui/ui_car.json";

        std::string displayName = car;
        std::string carBrand = "Unknown";
        std::string carClass = "Unknown";

        if (std::filesystem::exists(jsonPath))
        {
            std::ifstream f(jsonPath);

            if (f.is_open())
            {
                try {

                    json j;
                    f >> j;

                    if (j.contains("name"))
                        displayName = j["name"];

                    if (j.contains("brand"))
                        carBrand = j["brand"];

                    if (j.contains("class"))
                        carClass = j["class"];

                } catch(...) {}
            }
        }

        brands.insert(carBrand);
        classes.insert(carClass);

        QListWidgetItem* item =
            new QListWidgetItem(QString::fromStdString(displayName));

        item->setData(Qt::UserRole, QString::fromStdString(car));
        item->setData(Qt::UserRole + 1, QString::fromStdString(carBrand));
        item->setData(Qt::UserRole + 2, QString::fromStdString(carClass));

        std::string iconPath =
            acManager->getACPath() +
            "/content/cars/" +
            car +
            "/logo.png";

        if (std::filesystem::exists(iconPath))
            item->setIcon(QIcon(QString::fromStdString(iconPath)));

        carsList->addItem(item);
    }

    for (auto& b : brands)
        brandFilter->addItem(QString::fromStdString(b));

    for (auto& c : classes)
        classFilter->addItem(QString::fromStdString(c));

    for (const auto& track : acManager->listTracks())
    {
        QListWidgetItem* item =
            new QListWidgetItem(QString::fromStdString(track));

        std::string iconPath =
            acManager->getACPath() +
            "/content/tracks/" +
            track +
            "/preview.png";

        if (std::filesystem::exists(iconPath))
            item->setIcon(QIcon(QString::fromStdString(iconPath)));

        tracksList->addItem(item);
    }
}

void MainWindow::onLaunchClicked()
{
    acManager->launchGame();
}