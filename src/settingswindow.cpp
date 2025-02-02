#include "settingswindow.h"
#include "constants.h"
#include "ui_settingswindow.h"
#include <QFileDialog>

SettingsWindow::SettingsWindow(QWidget *parent)
    : QWidget(parent), ui(new Ui::SettingsWindow) {
  ui->setupUi(this);
  hideClientSettings();

  this->layout()->setSizeConstraint(QLayout::SetFixedSize);

  connect(ui->autoLogin, &QGroupBox::toggled, this,
          &SettingsWindow::enableLoginBox);
  connect(ui->autoDl, &QGroupBox::toggled, this,
          &SettingsWindow::enableClientBox);
  connect(ui->saveUploads, &QGroupBox::toggled, this,
          &SettingsWindow::enableDownloadSetting);
  connect(ui->savePathBrowse, &QPushButton::pressed, this,
          &SettingsWindow::selectSavePath);
  connect(ui->uTorBrowse, &QPushButton::pressed, this,
          &SettingsWindow::selectuTorPath);
  connect(ui->qBitBrowse, &QPushButton::pressed, this,
          &SettingsWindow::selectqBitPath);
  connect(ui->torrentClient, &QComboBox::currentIndexChanged, this,
          &SettingsWindow::updateClientSettings);

  connect(ui->save->button(QDialogButtonBox::Apply), &QPushButton::pressed,
          this, [this]() {
            this->saveSettings();
            this->loadSettings();
            emit saved();
          });
  connect(ui->save->button(QDialogButtonBox::Cancel), &QPushButton::pressed,
          this, [this]() {
            this->loadSettings();
            this->hide();
          });
  connect(ui->save->button(QDialogButtonBox::Ok), &QPushButton::pressed, this, [this]() {
      this->saveSettings();
      emit saved();
      this->hide();
  });
}

SettingsWindow::~SettingsWindow() { delete ui; }

void SettingsWindow::setData(TorrentClient *client, Settings *cfg) {
  Client = client;
  Cfg = cfg;
}

void SettingsWindow::enableLoginBox() {
  bool enable = ui->autoLogin->isChecked();
  ui->credsWidget->setEnabled(enable);
}

void SettingsWindow::enableClientBox() {
  bool enable = ui->autoDl->isChecked();
  ui->torrentClient->setEnabled(enable);
  ui->qBitWebSettings->setEnabled(enable);
  ui->uTorrSettings->setEnabled(enable);
}

void SettingsWindow::enableDownloadSetting() {
  bool enable = ui->saveUploads->isChecked();
  ui->savePath->setEnabled(enable);
  ui->savePathBrowse->setEnabled(enable);
}

void SettingsWindow::hideClientSettings() {
  ui->qBitWebSettings->setVisible(false);
  ui->uTorrSettings->setVisible(false);
  ui->qBitTorSettings->setVisible(false);
  ui->sysDefLabel->setVisible(false);
  ui->qBitWebSettings->hide();
  ui->uTorrSettings->hide();
  ui->qBitTorSettings->hide();
  ui->sysDefLabel->hide();
}

void SettingsWindow::updateClientSettings() {
  if (Cfg == nullptr) {
    return;
  }

  hideClientSettings();

  String name = Cfg->Clients[ui->torrentClient->currentIndex()];

  if (name == "qBitTorrent") {
    ui->qBitTorSettings->setVisible(true);
    ui->qBitTorSettings->show();
  }

  else if (name == "qBitTorrent WebUI") {
    ui->qBitWebSettings->setVisible(true);
    ui->qBitWebSettings->show();
  }

  else if (name == "System Default") {
    ui->sysDefLabel->setVisible(true);
    ui->sysDefLabel->show();
  }

#ifdef _WIN32
  else if (name == "uTorrent") {
    ui->uTorrSettings->setVisible(true);
    ui->uTorrSettings->show();
  }
#endif
}

void SettingsWindow::updateBoxes() {
  if (Cfg != nullptr)
    for (auto i : Cfg->Clients)
      ui->torrentClient->addItem(QString::fromStdString(i));
}

void SettingsWindow::selectSavePath() {
  auto path = QFileDialog::getExistingDirectory(this, tr("Select Folder"), "");
  if (!path.isNull())
    ui->savePath->setText(path);
}

void SettingsWindow::selectuTorPath() {
  auto path =
      QFileDialog::getOpenFileName(this, tr("Select uTorrent Path"), "",
                                   tr("uTorrent Executable (uTorrent.exe)"));
  if (!path.isNull())
    ui->uTorPath->setText(path);
}

void SettingsWindow::selectqBitPath() {
  auto path =
      QFileDialog::getOpenFileName(this, tr("Select qBitTorrent Path"), "",
                                   tr("qBitTorrent Executable (*)"));
  if (!path.isNull())
    ui->qBitPath->setText(path);
}

void SettingsWindow::loadSettings() {
  if (Cfg != nullptr) {
    qDebug() << "Loading settings";
    {
      auto it =
          std::find(Cfg->Clients.begin(), Cfg->Clients.end(), Cfg->client);
      if (it != Cfg->Clients.end()) {
        int index = it - Cfg->Clients.begin();
        ui->torrentClient->setCurrentIndex(index);
      }
    }

    ui->checkUpdates->setChecked(Cfg->updateCheck);
    ui->saveWarn->setChecked(Cfg->saveWarn);
    ui->autoLogin->setChecked(Cfg->saveLogin);
    ui->forceV1->setChecked(Cfg->forceV1);
    ui->domainSelect->setCurrentIndex(Cfg->gtDomain);
    ui->gtUsername->setText(QString::fromStdString(Cfg->gtUsername));
    ui->gtPassword->setText(QString::fromStdString(Cfg->gtPassword));
    ui->autoDl->setChecked(Cfg->autoDl);
    ui->qBitPath->setText(QString::fromStdString(Cfg->qBitPath));
    ui->qTorHost->setText(QString::fromStdString(Cfg->qBitHost));
    ui->qTorPort->setText(QString::fromStdString(Cfg->qBitPort));
    ui->qTorUser->setText(QString::fromStdString(Cfg->qBitUsername));
    ui->qTorPass->setText(QString::fromStdString(Cfg->qBitPassword));
    ui->saveUploads->setChecked(Cfg->saveUploads);
    ui->savePath->setText(QString::fromStdString(Cfg->savePath));
#ifdef _WIN32
    ui->uTorPath->setText(QString::fromStdString(Cfg->uTorrentPath));
#endif

    this->refresh();
  }
}

void SettingsWindow::saveSettings() {
  if (Cfg != nullptr) {
    qDebug() << "Saving settings";

    Cfg->updateCheck = ui->checkUpdates->isChecked();
    Cfg->saveWarn = ui->saveWarn->isChecked();
    Cfg->saveLogin = ui->autoLogin->isChecked();
    Cfg->forceV1 = ui->forceV1->isChecked();
    Cfg->gtDomain = ui->domainSelect->currentIndex();
    Cfg->gtUsername = ui->gtUsername->text().toStdString();
    Cfg->gtPassword = ui->gtPassword->text().toStdString();
    Cfg->autoDl = ui->autoDl->isChecked();
    Cfg->qBitPath = ui->qBitPath->text().toStdString();
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
  }
}
