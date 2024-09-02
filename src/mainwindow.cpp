#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "nlohmann/json.hpp"
#include "utils.h"
#include <QFileDialog>
#include <QMessageBox>
#include <filesystem>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

MainWindow::MainWindow(QWidget *parent, Settings *s, API *a)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  Cfg = s;
  Api = a;

  ui->setupUi(this);
  this->uiSetup();
}

MainWindow::~MainWindow() {
  delete Client;
  delete ui;
}
void MainWindow::uiSetup() {
  if (Cfg->autoDl)
    this->loadTorrentClient();
  SettingsWin.updateBoxes(Cfg);

  // Menu bar
  connect(ui->actionExit, &QAction::triggered, this, []() { exit(0); });
  connect(ui->actionNew, &QAction::triggered, this,
          &MainWindow::clearAllFields);
  connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openProject);
  connect(ui->actionSave, &QAction::triggered, this, &MainWindow::saveProject);
  connect(ui->actionSave_As, &QAction::triggered, this,
          &MainWindow::saveProjectAs);
  connect(ui->actionAbout, &QAction::triggered, this,
          &MainWindow::openAboutWindow);
  connect(ui->actionPreferences, &QAction::triggered, this,
          &MainWindow::openSettingsWindow);
  connect(ui->actionReload_categories, &QAction::triggered, this,
          &MainWindow::reloadCategories);
  connect(ui->actionRefresh_status, &QAction::triggered, this,
          &MainWindow::refreshInfo);
  connect(ui->actionCheckupdates, &QAction::triggered, nullptr,
          utils::checkForUpdates);

  // File
  connect(ui->fileSelBtn, &QPushButton::pressed, this, &MainWindow::selectFile);
  connect(ui->folderSelBtn, &QPushButton::pressed, this,
          &MainWindow::selectFolder);

  // Category selectors
  connect(ui->category, &QComboBox::currentIndexChanged, this,
          &MainWindow::enableItemsAuto);
  connect(ui->subcategory1, &QComboBox::currentIndexChanged, this,
          &MainWindow::enableItemsAuto);
  connect(ui->subcategory2, &QComboBox::currentIndexChanged, this,
          &MainWindow::enableItemsAuto);
  connect(ui->subcategory3, &QComboBox::currentIndexChanged, this,
          &MainWindow::enableItemsAuto);
  connect(ui->subcategory4, &QComboBox::currentIndexChanged, this,
          &MainWindow::enableItemsAuto);

  // Picture table
  connect(ui->addPicBtn, &QPushButton::pressed, this,
          &MainWindow::selectPictures);
  connect(ui->rmPicBtn, &QPushButton::pressed, this,
          &MainWindow::removePictures);

  // Info
  connect(ui->uploadBtn, &QPushButton::pressed, this,
          &MainWindow::uploadChecks);

  // Login window
  connect(LoginWin.getButton(), &QPushButton::clicked, this,
          &MainWindow::login);

  // Settings window
  connect(SettingsWin.getButton(), &QPushButton::clicked, this,
          &MainWindow::applySettings);

  this->refreshInfo();
}

void MainWindow::enableItemsAll(bool enable) {
  ui->path->setEnabled(enable);
  ui->fileSelBtn->setEnabled(enable);
  ui->folderSelBtn->setEnabled(enable);
  ui->category->setEnabled(enable);
  ui->subcategory1->setEnabled(enable);
  ui->subcategory2->setEnabled(enable);
  ui->subcategory3->setEnabled(enable);
  ui->subcategory4->setEnabled(enable);
  ui->addPicBtn->setEnabled(enable);
  ui->rmPicBtn->setEnabled(enable);
  ui->title->setEnabled(enable);
  ui->description->setEnabled(enable);
  ui->uploadBtn->setEnabled(enable);
  ui->picTable->setEnabled(enable);
  ui->loginBtn->setEnabled(enable);

  if (enable)
    this->enableItemsAuto();
}

void MainWindow::enableItemsAuto() {
  bool enable = ui->category->currentIndex() != 0;

  ui->subcategory1->setEnabled(enable);
  ui->subcategory2->setEnabled((ui->subcategory1->currentIndex() != 0) &&
                               ui->subcategory1->isEnabled());
  ui->subcategory3->setEnabled((ui->subcategory2->currentIndex() != 0) &&
                               ui->subcategory1->isEnabled() &&
                               ui->subcategory2->isEnabled());
  ui->subcategory4->setEnabled((ui->subcategory3->currentIndex() != 0) &&
                               ui->subcategory1->isEnabled() &&
                               ui->subcategory2->isEnabled() &&
                               ui->subcategory3->isEnabled());

  ui->addPicBtn->setEnabled(enable);
  ui->rmPicBtn->setEnabled(enable);
  ui->title->setEnabled(enable);
  ui->description->setEnabled(enable);
  ui->uploadBtn->setEnabled(enable);
  ui->picTable->setEnabled(enable);
}

void MainWindow::reloadCategories() {
  if (!Api->isLoggedIn()) {
    QMessageBox::warning(this, "GUU - Error",
                         "You must be logged in to do that.");
  } else {
    Api->downloadCategories();
    this->loadCategories();
  }
}

void MainWindow::loadCategories() {
  if (!Api->hasCategories()) {
    ui->category->clear();
    ui->category->addItem("Log in to load categories");
    return;
  }

  auto categories = Api->getCategories();

  ui->category->clear();
  ui->category->addItem("Select Category");
  ui->subcategory1->addItem("(Optional)");
  ui->subcategory2->addItem("(Optional)");
  ui->subcategory3->addItem("(Optional)");
  ui->subcategory4->addItem("(Optional)");

  for (auto const &c : categories) {
    QString name = QString::fromStdString(c.first);
    ui->category->addItem(name);
    ui->subcategory1->addItem(name);
    ui->subcategory2->addItem(name);
    ui->subcategory3->addItem(name);
    ui->subcategory4->addItem(name);
  }
}

void MainWindow::loadTorrentClient() {
  if (Client != nullptr)
    delete Client;
  Client = nullptr;

  if (Cfg->autoDl) {
    if (Cfg->client == "qBitTorrent")
      Client = new qBitTorrent();

    if (Client != nullptr)
      try {
        Client->configure(Cfg);
      } catch (const std::exception &e) {
        QMessageBox::warning(nullptr, "GUU - Error",
                             QString::fromStdString(e.what()));
      }
  }
}

void MainWindow::login() {
  LoginWin.hide();

  std::string username = LoginWin.getUsername();
  std::string password = LoginWin.getPassword();
  bool shouldSave = LoginWin.shouldSave();
  Api->login(username, password);
  if (Api->isLoggedIn()) {
    if (shouldSave) {
      Cfg->saveLogin = true;
      Cfg->gtUsername = username;
      Cfg->gtPassword = password;
      Cfg->save();
    }
  } else {
    QMessageBox::warning(
        this, "GUU - Error",
        "Could not login. Please check your credentials and try again.");
  }

  this->refreshInfo();
  this->enableItemsAll(true);
}

void MainWindow::logout() {
  this->enableItemsAll(false);
  Api->logout();
  this->refreshInfo();
  this->enableItemsAll(true);
}

void MainWindow::updateStatus() {
  // Client check
  ui->clientStatus->setText("N/A");
  if (Client != nullptr)
    if (Client->isConnected()) {
      QString s = QString::fromStdString(Client->name() + " (OK)");
      ui->clientStatus->setText(s);
    }

  // Server check
  if (Api->isServerOnline())
    ui->serverStatus->setText("Online");
  else
    ui->serverStatus->setText("Unreachable");

  // User check
  if (Api->isLoggedIn()) {
    QString username = QString::fromStdString(Api->fetchUsername());
    ui->loginBtn->setText("Log Out");
    ui->userStatus->setText(username);
    ui->loginBtn->disconnect();
    connect(ui->loginBtn, &QPushButton::pressed, this, &MainWindow::logout);
  } else {
    ui->loginBtn->setText("Log In");
    ui->userStatus->setText("-");
    ui->loginBtn->disconnect();
    connect(ui->loginBtn, &QPushButton::pressed, this,
            &MainWindow::openLoginWindow);
  }
}

void MainWindow::selectFile() {
  auto path = QFileDialog::getOpenFileName(this, tr("Open File"), "",
                                           tr("All Files (*.*)"));

  ui->path->clear();
  ui->path->insert(path);
}

void MainWindow::selectFolder() {
  auto path = QFileDialog::getExistingDirectory(this, tr("Open Folder"), "");

  ui->path->clear();
  ui->path->insert(path);
}

void MainWindow::selectPictures() {
  auto paths = QFileDialog::getOpenFileNames(
      this, tr("Open File"), "",
      tr("Images (*.png *.jpg *.jpeg *.bmp *.tif *.psd)"));

  for (auto pic : paths) {
    ui->picTable->addPicture(pic);
  }
}

void MainWindow::removePictures() {
  auto items = ui->picTable->selectedItems();
  for (auto item : items)
    delete ui->picTable->takeItem(ui->picTable->row(item));
}

void MainWindow::clearAllFields() {
  QMessageBox msgBox;
  msgBox.setText(
      "All fields are about to be cleared. Are you sure you want to continue?");
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  msgBox.setDefaultButton(QMessageBox::No);
  msgBox.setIcon(QMessageBox::Question);

  if (msgBox.exec() == QMessageBox::No)
    return;

  ui->path->clear();
  ui->category->setCurrentIndex(0);
  ui->subcategory1->setCurrentIndex(0);
  ui->subcategory2->setCurrentIndex(0);
  ui->subcategory3->setCurrentIndex(0);
  ui->subcategory4->setCurrentIndex(0);
  ui->picTable->clear();
  ui->title->clear();
  ui->description->clear();
}

void MainWindow::openProject() {
  this->clearAllFields();

  QString path = QFileDialog::getOpenFileName(this, tr("Open Project"), "",
                                              tr("GUU Files (*.guu)"));

  if (path.length() == 0)
    return;

  std::ifstream ifs(path.toStdString());
  std::string content((std::istreambuf_iterator<char>(ifs)),
                      (std::istreambuf_iterator<char>()));

  try {
    auto proj = json::parse(content);

    ui->category->setCurrentIndex(proj["Categories"]["Main"]);
    this->enableItemsAuto();
    ui->path->clear();
    ui->path->setText(QString::fromStdString(proj["Info"]["Path"]));
    ui->subcategory1->setCurrentIndex(proj["Categories"]["Secondary1"]);
    ui->subcategory1->setCurrentIndex(proj["Categories"]["Secondary2"]);
    ui->subcategory3->setCurrentIndex(proj["Categories"]["Secondary3"]);
    ui->subcategory4->setCurrentIndex(proj["Categories"]["Secondary4"]);
    ui->title->clear();
    ui->title->setText(QString::fromStdString(proj["Info"]["Title"]));
    ui->description->clear();
    ui->description->insertPlainText(
        QString::fromStdString(proj["Info"]["Description"]));
    ui->picTable->clear();
    std::vector<std::string> pics = proj["Pictures"]["Path(s)"];

    for (auto pic : pics)
      ui->picTable->addPicture(QString::fromStdString(pic));
  } catch (...) {
    QMessageBox::warning(this, "GUU - Error",
                         "Error loading project. Error loading project.");
  }
}

void MainWindow::saveProjectAs() {
  QString outPath = QFileDialog::getSaveFileName(
      this, tr("Save Project"), lastProjectPath, tr("GUU Files (*.guu)"));

  if (outPath.length() != 0) {
    lastProjectPath = outPath;
    this->saveProject();
  }
}

void MainWindow::saveProject() {
  if (lastProjectPath == "Untitled.guu") {
    this->saveProjectAs();
    return;
  }

  int mc = ui->category->currentIndex();
  int sc1 = ui->subcategory1->currentIndex();
  int sc2 = ui->subcategory2->currentIndex();
  int sc3 = ui->subcategory3->currentIndex();
  int sc4 = ui->subcategory4->currentIndex();
  std::string title = ui->title->text().toStdString();
  std::string description = ui->description->toPlainText().toStdString();
  std::string path = ui->path->text().toStdString();
  std::vector<std::string> pics;
  for (int i = 0; i < ui->picTable->count(); i++)
    pics.push_back(ui->picTable->item(i)->text().toStdString());

  if (lastProjectPath.length() == 0)
    return;

  json proj{
      {"Categories",
       {
           {"Main", mc},
           {"Secondary1", sc1},
           {"Secondary2", sc2},
           {"Secondary3", sc3},
           {"Secondary4", sc4},
       }},
      {"Info",
       {{"Title", title}, {"Description", description}, {"Path", path}}},
      {"Pictures", {{"Path(s)", pics}}},
  };

  std::ofstream out(lastProjectPath.toStdString());
  if (!out) {
    QMessageBox::warning(this, "GUU - Error",
                         "An error occured while creating the project file.");
    return;
  }

  out << proj.dump();
  out.close();

  if (out.bad() || out.fail()) {
    QMessageBox::warning(this, "GUU - Error",
                         "An I/O error occured while saving the project.");
    return;
  }
}
void MainWindow::uploadChecks() {
  QMessageBox msgBox;
  msgBox.setText("Are you sure you want to upload?");
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  msgBox.setDefaultButton(QMessageBox::Yes);
  msgBox.setIcon(QMessageBox::Question);

  if (msgBox.exec() == QMessageBox::No)
    return;

  if (!Api->isLoggedIn()) {
    int res = QMessageBox::warning(this, "GUU - Error",
                                   "You need to be logged in to upload.");
    if (res == QMessageBox::Ok)
      this->openLoginWindow();
    return;
  }

  if (Cfg->autoDl && Client != nullptr) {
    if (!Client->isConnected()) {
      QMessageBox::warning(this, "GUU - Error",
                           "Torrent client unavailable. Your torrent cannot be "
                           "seeded automatically. Aborting...");
      return;
    }
  }

  QString provideInfo =
      "These fields are necessary for the torrent to be uploaded:";
  bool needInfo = false;
  if (ui->path->text().length() == 0 ||
      !std::filesystem::exists(ui->path->text().toStdString())) {
    provideInfo += "\n- Valid file/folder path";
    needInfo = true;
  }
  if (ui->category->currentIndex() == 0) {
    provideInfo += "\n- Category";
    needInfo = true;
  }
  if (ui->title->text().length() == 0) {
    provideInfo += "\n- Title";
    needInfo = true;
  }
  if (ui->description->toPlainText().length() == 0) {
    provideInfo += "\n- Description";
    needInfo = true;
  }
  ui->picTable->removeInvalid();
  if (ui->picTable->count() == 0) {
    provideInfo += "\n- At least 1 picture";
    needInfo = true;
  }

  if (needInfo) {
    QMessageBox::warning(this, "GUU - Error", provideInfo);
    return;
  }

  this->beginUpload();
}

void MainWindow::beginUpload() {
  thread = new QThread();
  worker = new UploadWorker();

  worker->configure(Cfg, Client);
  ui->uploadStatus->setMaximum(worker->Stages);
  this->enableItemsAll(false);

  worker->moveToThread(thread);
  connect(worker, &UploadWorker::textChanged, ui->uploadStatus,
          &QProgressBar::setFormat);
  connect(worker, &UploadWorker::valueChanged, ui->uploadStatus,
          &QProgressBar::setValue);
  connect(worker, &UploadWorker::errorRaised, this,
          &MainWindow::showUploadError);
  connect(worker, SIGNAL(workRequested()), thread, SLOT(start()));
  connect(thread, SIGNAL(started()), worker, SLOT(doWork()));
  connect(worker, &UploadWorker::finished, thread, &QThread::quit);
  connect(thread, &QThread::finished, this, &MainWindow::finishUpload);

  UploadWorker::WorkerInputData data;
  data.api = Api;
  data.path = ui->path->text().toStdString();
  data.title = ui->title->text().toStdString();
  data.description = ui->description->toPlainText().toStdString();
  data.images = {};
  for (int i = 0; i < ui->picTable->count(); i++) {
    std::string pth = ui->picTable->item(i)->text().toStdString();
    if (std::filesystem::exists(pth))
      data.images.push_back(pth);
  }

  auto categs = Api->getCategories();
  data.categ = categs[ui->category->currentText().toStdString()];
  if (ui->subcategory1->currentIndex() != 0) {
    data.sCateg1 = categs[ui->subcategory1->currentText().toStdString()];
    if (ui->subcategory2->currentIndex() != 0) {
      data.sCateg2 = categs[ui->subcategory1->currentText().toStdString()];
      if (ui->subcategory3->currentIndex() != 0) {
        data.sCateg3 = categs[ui->subcategory3->currentText().toStdString()];
        if (ui->subcategory4->currentIndex() != 0)
          data.sCateg4 = categs[ui->subcategory4->currentText().toStdString()];
      }
    }
  }

  worker->run(data);
}

void MainWindow::showUploadError(const QString &text) {
  QMessageBox::warning(this, "GUU - Error", text);

  thread->wait();

  this->enableItemsAll(true);
  ui->uploadStatus->setMaximum(1);
  ui->uploadStatus->setValue(0);
  ui->uploadStatus->setFormat("Waiting...");
}

void MainWindow::finishUpload() {
  thread->wait();
  delete thread;
  delete worker;

  this->enableItemsAll(true);
  ui->uploadStatus->setMaximum(1);
  ui->uploadStatus->setValue(0);
  ui->uploadStatus->setFormat("Waiting...");

  QMessageBox::information(this, "GUU - Success",
                           "Your torrent has been uploaded!");
}
