#include "CarSelectorDialog.h"
#include "ACManager.h"
#include "SkinSelectorDialog.h"

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
#include <regex>
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

    // remplace \n \t
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

CarSelectorDialog::CarSelectorDialog(
    ACManager* ac,
    QWidget* parent
)
: QDialog(parent), acManager(ac)
{
    setWindowTitle("Select Car");
    resize(1200,700);

    QVBoxLayout* mainLayout = new QVBoxLayout;

    brandFilter = new QComboBox;
    brandFilter->addItem("All Brands");

    mainLayout->addWidget(brandFilter);

    QHBoxLayout* layout = new QHBoxLayout;

    carsList = new QListWidget;
    carsList->setIconSize(QSize(220,110));
    carsList->setResizeMode(QListWidget::Adjust);
    carsList->setViewMode(QListWidget::IconMode);

    layout->addWidget(carsList,2);

    QVBoxLayout* rightPanel = new QVBoxLayout;

    carPreview = new QLabel;
    carPreview->setMinimumSize(500,250);
    carPreview->setScaledContents(true);

    carInfo = new QLabel;
    carInfo->setWordWrap(true);

    rightPanel->addWidget(carPreview);
    rightPanel->addWidget(carInfo);

    layout->addLayout(rightPanel,1);

    mainLayout->addLayout(layout);

    QPushButton* selectBtn = new QPushButton("Select");
    mainLayout->addWidget(selectBtn);

    setLayout(mainLayout);

    loadCars();

    connect(carsList,&QListWidget::currentItemChanged,
        [this](){ updateInfo(); });

    connect(brandFilter,&QComboBox::currentTextChanged,
    [this](const QString& brand){
        for(int i=0;i<carsList->count();i++)
        {
            QListWidgetItem* item = carsList->item(i);

            QString itemBrand = item->data(Qt::UserRole + 1).toString();

            if(brand=="All Brands")
                item->setHidden(false);
            else
                item->setHidden(itemBrand != brand);
        }
    });

    connect(selectBtn,&QPushButton::clicked,[this](){

        QListWidgetItem* item = carsList->currentItem();
        if(!item) return;

        selectedCar =
            item->data(Qt::UserRole).toString().toStdString();

        SkinSelectorDialog dlg(acManager,selectedCar,this);

        if(dlg.exec()==QDialog::Accepted)
        {
            selectedSkin = dlg.getSelectedSkin();
            accept();
        }

    });
}

void CarSelectorDialog::loadCars()
{
    std::set<std::string> brands;

    for(const auto& car : acManager->listCars())
    {
        std::string jsonPath =
            acManager->getACPath()
            + "/content/cars/"
            + car
            + "/ui/ui_car.json";

        std::string brand="Unknown";
        std::string name=car;

        if(std::filesystem::exists(jsonPath))
        {
            json j = readJsonSafe(jsonPath);

            if(j.contains("brand") && !j["brand"].get<std::string>().empty())
                brand=j["brand"];
            if(j.contains("name") && !j["name"].get<std::string>().empty())
                name=j["name"];
        }

        brands.insert(brand);

        std::string skinsPath =
            acManager->getACPath()
            + "/content/cars/"
            + car
            + "/skins";

        if(!std::filesystem::exists(skinsPath))
            continue;

        std::string preview;

        for(const auto& skin :
            std::filesystem::directory_iterator(skinsPath))
        {
            std::string p =
                skin.path().string()+"/preview.jpg";

            if(std::filesystem::exists(p))
            {
                preview=p;
                break;
            }
        }

        if(preview.empty())
            continue;

        QListWidgetItem* item =
            new QListWidgetItem;

        item->setData(
            Qt::UserRole,
            QString::fromStdString(car)
        );

        item->setData(
            Qt::UserRole + 1,
            QString::fromStdString(brand)
        );

        item->setIcon(
            QIcon(QString::fromStdString(preview))
        );

        item->setText(
            QString::fromStdString(name)
        );

        carsList->addItem(item);
    }

    std::vector<std::string> sortedBrands(brands.begin(), brands.end());
    std::sort(sortedBrands.begin(), sortedBrands.end());

    for(auto& b : sortedBrands)
        brandFilter->addItem(QString::fromStdString(b));
}

void CarSelectorDialog::updateInfo()
{
    QListWidgetItem* item=carsList->currentItem();
    if(!item) return;

    std::string car =
        item->data(Qt::UserRole).toString().toStdString();

    std::string jsonPath =
        acManager->getACPath()
        + "/content/cars/"
        + car
        + "/ui/ui_car.json";

    if(!std::filesystem::exists(jsonPath))
        return;

    json j = readJsonSafe(jsonPath);

    std::cout << "Reading car JSON: " << jsonPath << std::endl;
    std::cout << j.dump(4) << std::endl;

    std::string info;

    if(j.contains("name") && !j["name"].is_null())
        info+="Name: "+j["name"].get<std::string>()+"\n";

    if(j.contains("brand") && !j["brand"].is_null())
        info+="Brand: "+j["brand"].get<std::string>()+"\n";

    if(j.contains("class") && !j["class"].is_null())
        info+="Class: "+j["class"].get<std::string>()+"\n";

    if(j.contains("description") && !j["description"].is_null())
        info+="\n"+j["description"].get<std::string>();

    carInfo->setText(
        QString::fromStdString(info)
    );

    std::string skinsPath =
        acManager->getACPath()
        + "/content/cars/"
        + car
        + "/skins";

    for(const auto& skin :
        std::filesystem::directory_iterator(skinsPath))
    {
        std::string p =
            skin.path().string()+"/preview.jpg";

        if(std::filesystem::exists(p))
        {
            carPreview->setPixmap(
                QPixmap(QString::fromStdString(p))
            );
            break;
        }
    }
}

std::string CarSelectorDialog::getSelectedCar()
{
    return selectedCar;
}

std::string CarSelectorDialog::getSelectedSkin()
{
    return selectedSkin;
}