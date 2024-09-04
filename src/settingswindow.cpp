#include "settingswindow.h"
#include "constants.h"
#include "ui_settingswindow.h"
#include <QFileDialog>

SettingsWindow::SettingsWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);
    ui->qBitSettings->setVisible(false);
    ui->uTorrSettings->setVisible(false);
    ui->qBitSettings->hide();
    ui->uTorrSettings->hide();

    this->resize(1, 1);
    this->setMaximumSize(this->size());

    connect(ui->autoLogin, &QCheckBox::stateChanged, this, &SettingsWindow::enableLoginBox);
    connect(ui->autoDl, &QCheckBox::stateChanged, this, &SettingsWindow::enableClientBox);
    connect(ui->saveUploads, &QCheckBox::stateChanged, this, &SettingsWindow::enableDownloadSetting);
    connect(ui->savePathBrowse, &QPushButton::pressed, this, &SettingsWindow::selectSavePath);
    connect(ui->uTorBrowse, &QPushButton::pressed, this, &SettingsWindow::selectuTorPath);
    connect(ui->torrentClient,
            &QComboBox::currentIndexChanged,
            this,
            &SettingsWindow::updateClientSettings);
}

SettingsWindow::~SettingsWindow() { delete ui; }

void SettingsWindow::setData(TorrentClient *client, Settings *cfg)
{
    Client = client;
    Cfg = cfg;
}

QPushButton *SettingsWindow::getButton() { return ui->saveSetBtn; }

void SettingsWindow::enableLoginBox() {
  bool enable = ui->autoLogin->isChecked();
  ui->gtUsername->setEnabled(enable);
  ui->gtPassword->setEnabled(enable);
}

void SettingsWindow::enableClientBox() {
  bool enable = ui->autoDl->isChecked();
  ui->torrentClient->setEnabled(enable);
  ui->qTorHost->setEnabled(enable);
  ui->qTorPort->setEnabled(enable);
  ui->qTorUser->setEnabled(enable);
  ui->qTorPass->setEnabled(enable);
}

void SettingsWindow::enableDownloadSetting() {
  bool enable = ui->saveUploads->isChecked();
  ui->savePath->setEnabled(enable);
  ui->savePathBrowse->setEnabled(enable);
}

void SettingsWindow::updateClientSettings()
{
    if (Cfg == nullptr || Client == nullptr) {
        return;
    }

    ui->qBitSettings->setVisible(false);
    ui->uTorrSettings->setVisible(false);
    ui->qBitSettings->hide();
    ui->uTorrSettings->hide();

    std::string name = Cfg->Clients[ui->torrentClient->currentIndex()];

    if (name == "qBitTorrent") {
        ui->qBitSettings->setVisible(true);
        ui->qBitSettings->show();
    }

#ifdef _WIN32
    if (name == "uTorrent") {
        ui->uTorrSettings->setVisible(true);
        ui->uTorrSettings->show();
    }
#endif

    this->resize(1, 1);
    this->setMaximumSize(this->size());
}

void SettingsWindow::updateBoxes()
{
    if (Cfg != nullptr)
        for (auto i : Cfg->Clients)
            ui->torrentClient->addItem(QString::fromStdString(i));
}

void SettingsWindow::selectSavePath() {
  auto path = QFileDialog::getExistingDirectory(this, tr("Select Folder"), "");
  ui->savePath->clear();
  ui->savePath->setText(path);
}

void SettingsWindow::selectuTorPath()
{
    auto path = QFileDialog::getOpenFileName(this,
                                             tr("Select uTorrent Path"),
                                             "",
                                             tr("uTorrent Executable (uTorrent.exe)"));
    ui->uTorPath->clear();
    ui->uTorPath->setText(path);
}

void SettingsWindow::applySettings()
{
    if (Cfg != nullptr) {
        {
            auto it = std::find(Cfg->Clients.begin(), Cfg->Clients.end(), Cfg->client);
            if (it != Cfg->Clients.end()) {
                int index = it - Cfg->Clients.begin();
                ui->torrentClient->setCurrentIndex(index);
            }
        }

        ui->checkUpdates->setChecked(Cfg->updateCheck);
        ui->autoLogin->setChecked(Cfg->saveLogin);
        ui->gtUsername->setText(QString::fromStdString(Cfg->gtUsername));
        ui->gtPassword->setText(QString::fromStdString(Cfg->gtPassword));
        ui->autoDl->setChecked(Cfg->autoDl);
        ui->qTorHost->setText(QString::fromStdString(Cfg->qBitHost));
        ui->qTorPort->setText(QString::fromStdString(Cfg->qBitPort));
        ui->qTorUser->setText(QString::fromStdString(Cfg->qBitUsername));
        ui->qTorPass->setText(QString::fromStdString(Cfg->qBitPassword));
        ui->saveUploads->setChecked(Cfg->saveUploads);
        ui->savePath->setText(QString::fromStdString(Cfg->savePath));
#ifdef _WIN32
        ui->uTorPath
            ->setText(QString::fromStdString(Cfg->uTorrentPath));
#endif

        this->refresh();
    }
}

void SettingsWindow::saveSettings()
{
    if (Cfg != nullptr) {
        Cfg->updateCheck = ui->checkUpdates->isChecked();
        Cfg->saveLogin = ui->autoLogin->isChecked();
        Cfg->gtUsername = ui->gtUsername->text().toStdString();
        Cfg->gtPassword = ui->gtPassword->text().toStdString();
        Cfg->autoDl = ui->autoDl->isChecked();
        Cfg->qBitHost = ui->qTorHost->text().toStdString();
        Cfg->qBitPort = ui->qTorPort->text().toStdString();
        Cfg->qBitUsername = ui->qTorUser->text().toStdString();
        Cfg->qBitPassword = ui->qTorPass->text().toStdString();
        Cfg->saveUploads = ui->saveUploads->isChecked();
        Cfg->savePath = ui->savePath->text().toStdString();

        Cfg->client = Cfg->Clients[ui->torrentClient->currentIndex()];

#ifdef _WIN32
        Cfg->uTorrentPath = ui->uTorPath->text().toStdString();
#endif

        Cfg->save();

        this->hide();
    }
}
