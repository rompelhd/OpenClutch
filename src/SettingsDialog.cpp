#include "SettingsDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFileDialog>

SettingsDialog::SettingsDialog(QWidget* parent) : QDialog(parent)
{
    setWindowTitle("Settings - OpenAC Launcher");
    setModal(true);

    QVBoxLayout* mainLayout = new QVBoxLayout();

    // Assetto Corsa path
    QHBoxLayout* acLayout = new QHBoxLayout();
    acLayout->addWidget(new QLabel("Route Assetto Corsa:"));
    acPathEdit = new QLineEdit(this);
    acLayout->addWidget(acPathEdit);
    browseACButton = new QPushButton("Find...", this);
    connect(browseACButton, &QPushButton::clicked, this, &SettingsDialog::onBrowseAC);
    acLayout->addWidget(browseACButton);
    mainLayout->addLayout(acLayout);

    // Steam path
    QHBoxLayout* steamLayout = new QHBoxLayout();
    steamLayout->addWidget(new QLabel("Route Steam:"));
    steamPathEdit = new QLineEdit(this);
    steamLayout->addWidget(steamPathEdit);
    browseSteamButton = new QPushButton("Find...", this);
    connect(browseSteamButton, &QPushButton::clicked, this, &SettingsDialog::onBrowseSteam);
    steamLayout->addWidget(browseSteamButton);
    mainLayout->addLayout(steamLayout);

    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    okButton = new QPushButton("OK", this);
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    cancelButton = new QPushButton("Cancel", this);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    buttonsLayout->addWidget(okButton);
    buttonsLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonsLayout);

    setLayout(mainLayout);
}

void SettingsDialog::onBrowseAC()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select Assetto Corsa folder");
    if (!dir.isEmpty())
        acPathEdit->setText(dir);
}

void SettingsDialog::onBrowseSteam()
{
    QString file = QFileDialog::getOpenFileName(this, "Select Steam executable");
    if (!file.isEmpty())
        steamPathEdit->setText(file);
}

QString SettingsDialog::getACPath() const
{
    return acPathEdit->text();
}

QString SettingsDialog::getSteamPath() const
{
    return steamPathEdit->text();
}