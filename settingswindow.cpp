#include "settingswindow.h"
#include "ui_settingswindow.h"
#include "constants.h"
#include <QFileDialog>

SettingsWindow::SettingsWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);
    this->resize(1, 1);
    this->setMaximumSize(this->size());

    connect(ui->autoLogin, &QCheckBox::stateChanged, this, &SettingsWindow::enableLoginBox);
    connect(ui->autoDl, &QCheckBox::stateChanged, this, &SettingsWindow::enableClientBox);
    connect(ui->saveUploads, &QCheckBox::stateChanged, this, &SettingsWindow::enableDownloadSetting);
    connect(ui->savePathBrowse, &QPushButton::pressed, this, &SettingsWindow::selectSavePath);
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

QPushButton *SettingsWindow::getButton() {
    return ui->saveSetBtn;
}

void SettingsWindow::enableLoginBox() {
    bool enable = ui->autoLogin->isChecked();
    ui->gtUsername->setEnabled(enable);
    ui->gtPassword->setEnabled(enable);
}

void SettingsWindow::enableClientBox() {
    bool enable = ui->autoDl->isChecked();
    ui->torrentClient->setEnabled(enable);
    ui->webuiHost->setEnabled(enable);
    ui->webuiPort->setEnabled(enable);
    ui->webuiUser->setEnabled(enable);
    ui->webuiPwd->setEnabled(enable);
}

void SettingsWindow::enableDownloadSetting() {
    bool enable = ui->saveUploads->isChecked();
    ui->savePath->setEnabled(enable);
    ui->savePathBrowse->setEnabled(enable);
}

void SettingsWindow::updateBoxes(Settings &settings) {
    for (auto i : settings.Languages)
        ui->language->addItem(QString::fromStdString(i.second));

    for (auto i : settings.Clients)
        ui->torrentClient->addItem(QString::fromStdString(i));
}

void SettingsWindow::selectSavePath() {
    auto path = QFileDialog::getExistingDirectory(this,
                                                  tr("Select Folder"),
                                                  "");
    ui->savePath->clear();
    ui->savePath->setText(path);
}

void SettingsWindow::applySettings(Settings &settings) {
    {
        int i = 0;
        for (auto l : settings.Languages) {
            if (settings.language == l.first) {
                ui->language->setCurrentIndex(i);
                break;
            }
            i++;
        }
    }

    {
        auto it = std::find(settings.Clients.begin(),
                            settings.Clients.end(),
                            settings.client);
        if (it != settings.Clients.end()) {
            int index = it - settings.Clients.begin();
            ui->torrentClient->setCurrentIndex(index);
        }
    }

    ui->autoLogin->setChecked(settings.saveLogin);
    ui->gtUsername->setText(QString::fromStdString(settings.gtUsername));
    ui->gtPassword->setText(QString::fromStdString(settings.gtPassword));
    ui->autoDl->setChecked(settings.autoDl);
    ui->webuiHost->setText(QString::fromStdString(settings.qBitHost));
    ui->webuiPort->setText(QString::fromStdString(settings.qBitPort));
    ui->webuiUser->setText(QString::fromStdString(settings.qBitUsername));
    ui->webuiPwd->setText(QString::fromStdString(settings.qBitPassword));
    ui->saveUploads->setChecked(settings.saveUploads);
    ui->savePath->setText(QString::fromStdString(settings.savePath));

    this->refresh();
}

void SettingsWindow::saveSettings(Settings &settings) {
    settings.saveLogin = ui->autoLogin->isChecked();
    settings.gtUsername = ui->gtUsername->text().toStdString();
    settings.gtPassword = ui->gtPassword->text().toStdString();
    settings.autoDl = ui->autoDl->isChecked();
    settings.qBitHost = ui->webuiHost->text().toStdString();
    settings.qBitPort = ui->webuiPort->text().toStdString();
    settings.qBitUsername = ui->webuiUser->text().toStdString();
    settings.qBitPassword = ui->webuiPwd->text().toStdString();
    settings.saveUploads = ui->saveUploads->isChecked();
    settings.savePath = ui->savePath->text().toStdString();

    settings.client = settings.Clients[ui->torrentClient->currentIndex()];

    {
        int idx = ui->language->currentIndex();
        auto it = settings.Languages.begin();
        std::advance(it, idx);
        settings.language = it->first;
    }

    settings.save();

    this->hide();
}
