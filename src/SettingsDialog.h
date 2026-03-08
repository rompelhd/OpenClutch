#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    SettingsDialog(QWidget* parent = nullptr);

    QString getACPath() const;
    QString getSteamPath() const;

    void setACPath(const QString& path) { acPathEdit->setText(path); }
    void setSteamPath(const QString& path) { steamPathEdit->setText(path); }

private slots:
    void onBrowseAC();
    void onBrowseSteam();

private:
    QLineEdit* acPathEdit;
    QLineEdit* steamPathEdit;
    QPushButton* browseACButton;
    QPushButton* browseSteamButton;
    QPushButton* okButton;
    QPushButton* cancelButton;
};

#endif // SETTINGSDIALOG_H