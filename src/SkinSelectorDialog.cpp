#include "SkinSelectorDialog.h"
#include "ACManager.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QListWidget>
#include <QListWidgetItem>
#include <QIcon>

#include <filesystem>

SkinSelectorDialog::SkinSelectorDialog(
    ACManager* ac,
    const std::string& car,
    QWidget* parent
)
: QDialog(parent), acManager(ac), carName(car)
{
    setWindowTitle("Select Skin");
    resize(800,400);

    QVBoxLayout* layout = new QVBoxLayout;

    skinsList = new QListWidget;
    skinsList->setIconSize(QSize(220,110));
    skinsList->setViewMode(QListWidget::IconMode);

    layout->addWidget(skinsList);

    QPushButton* selectBtn = new QPushButton("Select");
    layout->addWidget(selectBtn);

    setLayout(layout);

    loadSkins();

    connect(selectBtn,&QPushButton::clicked,[this](){

        QListWidgetItem* item = skinsList->currentItem();
        if(!item) return;

        selectedSkin =
            item->data(Qt::UserRole).toString().toStdString();

        accept();

    });
}

void SkinSelectorDialog::loadSkins()
{
    std::string skinsPath =
        acManager->getACPath()
        + "/content/cars/"
        + carName
        + "/skins";

    for(const auto& skin :
        std::filesystem::directory_iterator(skinsPath))
    {
        std::string preview =
            skin.path().string()+"/preview.jpg";

        if(!std::filesystem::exists(preview))
            continue;

        QListWidgetItem* item =
            new QListWidgetItem;

        item->setIcon(
            QIcon(QString::fromStdString(preview)));

        item->setText(
            QString::fromStdString(
                skin.path().filename().string()
            )
        );

        item->setData(
            Qt::UserRole,
            QString::fromStdString(
                skin.path().filename().string()
            )
        );

        skinsList->addItem(item);
    }
}

std::string SkinSelectorDialog::getSelectedSkin()
{
    return selectedSkin;
}