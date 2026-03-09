#include "CircuitSelectorDialog.h"
#include "ACManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QListWidget>
#include <QListWidgetItem>
#include <QIcon>
#include <QLabel>
#include <QComboBox>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <set>
#include <vector>
#include <algorithm>
#include <iostream>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

static json readJsonSafe(const std::string& path)
{
    std::ifstream f(path);
    if(!f.is_open()) return json{};

    std::stringstream buffer;
    buffer << f.rdbuf();
    std::string content = buffer.str();

    // CR (\r)
    content.erase(std::remove(content.begin(), content.end(), '\r'), content.end());

    // Escape \n y \t
    std::string clean;
    bool inString = false;
    for(size_t i=0;i<content.size();i++)
    {
        char c = content[i];
        if(c=='\"' && (i==0 || content[i-1]!='\\'))
            inString = !inString;

        if(inString)
        {
            if(c=='\n' || c=='\r')
                clean += "\\n";
            else if(c=='\t')
                clean += "\\t";
            else
                clean += c;
        }
        else
        {
            clean += c;
        }
    }

    try {
        return json::parse(clean);
    } catch(const std::exception& e) {
        std::cerr << "Failed to parse JSON: " << path << " | " << e.what() << "\n";
        return json{};
    }
}

CircuitSelectorDialog::CircuitSelectorDialog(
    ACManager* ac,
    QWidget* parent
)
: QDialog(parent), acManager(ac)
{
    setWindowTitle("Select Circuit");
    resize(1200,700);

    QVBoxLayout* mainLayout = new QVBoxLayout;

    locationFilter = new QComboBox;
    locationFilter->addItem("All Locations");
    mainLayout->addWidget(locationFilter);

    QHBoxLayout* layout = new QHBoxLayout;

    circuitsList = new QListWidget;
    circuitsList->setIconSize(QSize(220,110));
    circuitsList->setResizeMode(QListWidget::Adjust);
    circuitsList->setViewMode(QListWidget::IconMode);
    layout->addWidget(circuitsList,2);

    QVBoxLayout* rightPanel = new QVBoxLayout;

    circuitPreview = new QLabel;
    circuitPreview->setMinimumSize(500,250);
    circuitPreview->setScaledContents(true);

    circuitInfo = new QLabel;
    circuitInfo->setWordWrap(true);

    rightPanel->addWidget(circuitPreview);
    rightPanel->addWidget(circuitInfo);

    layout->addLayout(rightPanel,1);

    mainLayout->addLayout(layout);

    QPushButton* selectBtn = new QPushButton("Select");
    mainLayout->addWidget(selectBtn);

    setLayout(mainLayout);

    loadCircuits();

    connect(circuitsList,&QListWidget::currentItemChanged,
        [this](){ updateInfo(); });

    connect(locationFilter,&QComboBox::currentTextChanged,
        [this](const QString& location){
            for(int i=0;i<circuitsList->count();i++)
            {
                QListWidgetItem* item = circuitsList->item(i);
                QString itemLoc = item->data(Qt::UserRole + 1).toString();

                if(location=="All Locations")
                    item->setHidden(false);
                else
                    item->setHidden(itemLoc != location);
            }
        });

    connect(selectBtn,&QPushButton::clicked,[this](){
        QListWidgetItem* item = circuitsList->currentItem();
        if(!item) return;

        selectedCircuit = item->data(Qt::UserRole).toString().toStdString();
        accept();
    });
}

void CircuitSelectorDialog::loadCircuits()
{
    std::set<std::string> locations;

    for(const auto& track : acManager->listTracks())
    {
        std::string trackPath =
            acManager->getACPath()
            + "/content/tracks/"
            + track;

        std::string previewPath = trackPath + "/ui/preview.png";

        if(!std::filesystem::exists(previewPath))
            continue;

        std::string name = track;
        std::string location = "Unknown";

        // Read info ui_track.json
        std::string jsonPath = trackPath + "/ui/ui_track.json";
        if(std::filesystem::exists(jsonPath))
        {
            json j = readJsonSafe(jsonPath);

            if(j.contains("name") && !j["name"].get<std::string>().empty())
                name = j["name"];
            if(j.contains("location") && !j["location"].get<std::string>().empty())
                location = j["location"];
        }

        locations.insert(location);

        QListWidgetItem* item = new QListWidgetItem;

        item->setData(Qt::UserRole, QString::fromStdString(track));
        item->setData(Qt::UserRole + 1, QString::fromStdString(location));
        item->setIcon(QIcon(QString::fromStdString(previewPath)));
        item->setText(QString::fromStdString(name));

        circuitsList->addItem(item);
    }

    std::vector<std::string> sortedLocations(locations.begin(), locations.end());
    std::sort(sortedLocations.begin(), sortedLocations.end());

    for(auto& loc : sortedLocations)
        locationFilter->addItem(QString::fromStdString(loc));
}

void CircuitSelectorDialog::updateInfo()
{
    QListWidgetItem* item=circuitsList->currentItem();
    if(!item) return;

    std::string track = item->data(Qt::UserRole).toString().toStdString();
    std::string jsonPath =
        acManager->getACPath()
        + "/content/tracks/"
        + track
        + "/ui/ui_track.json";

    if(!std::filesystem::exists(jsonPath))
        return;

    json j = readJsonSafe(jsonPath);

    std::string info;

    if(j.contains("name") && !j["name"].is_null())
        info+="Name: "+j["name"].get<std::string>()+"\n";
    if(j.contains("location") && !j["location"].is_null())
        info+="Location: "+j["location"].get<std::string>()+"\n";
    if(j.contains("description") && !j["description"].is_null())
        info+="\n"+j["description"].get<std::string>();

    circuitInfo->setText(QString::fromStdString(info));

    std::string previewPath =
        acManager->getACPath()
        + "/content/tracks/"
        + track
        + "/ui/preview.png";

    if(std::filesystem::exists(previewPath))
        circuitPreview->setPixmap(QPixmap(QString::fromStdString(previewPath)));
}

std::string CircuitSelectorDialog::getSelectedCircuit()
{
    return selectedCircuit;
}