#include "picturemanager.h"
#include "ui_picturemanager.h"

#include <QFileDialog>

PictureManager::PictureManager(QWidget *parent)
    : QWidget(parent), ui(new Ui::PictureManager) {
  ui->setupUi(this);
  ui->picTable->setIconSize(QSize(200, 200));
  connect(ui->addPics, &QPushButton::pressed, this,
          &PictureManager::selectPictures);
  connect(ui->rmPics, &QPushButton::pressed, this,
          &PictureManager::removePictures);
  connect(ui->mvUp, &QPushButton::pressed, ui->picTable, &PictureList::moveUp);
  connect(ui->mvDown, &QPushButton::pressed, ui->picTable,
          &PictureList::moveDown);

  connect(ui->picTable, &PictureList::modified, this,
          &PictureManager::modified);

  this->setAcceptDrops(true);
}

QList<QListWidgetItem *> PictureManager::getList() {
  QList<QListWidgetItem *> list;
  for (int i = 0; i < ui->picTable->count(); i++) {
    list.push_back(ui->picTable->item(i));
  }
  return list;
}

void PictureManager::addPicture(QString path) {
  ui->picTable->addPicture(path);
}

void PictureManager::removeInvalid() {
  ui->picTable->removeInvalid();
  emit modified();
}

void PictureManager::clear() {
  ui->picTable->clear();
  emit modified();
}

void PictureManager::selectPictures() {
  auto paths = QFileDialog::getOpenFileNames(
      this, tr("Open File"), "", tr("Images (*.png *.jpg *.jpeg *.bmp)"));

  for (auto pic : paths) {
    ui->picTable->addPicture(pic);
  }
}

void PictureManager::removePictures() { ui->picTable->removeSelected(); }

PictureManager::~PictureManager() { delete ui; }
