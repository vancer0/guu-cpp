#include "picturelist.h"
#include <QMessageBox>
#include <filesystem>
#include <string>

PictureList::PictureList(QWidget *parent) : QListWidget(parent) {
  this->setAcceptDrops(true);
  this->setIconSize(QSize(150, 150));
  this->setSelectionMode(QAbstractItemView::MultiSelection);
  this->setDragEnabled(true);
  this->setDefaultDropAction(Qt::MoveAction);
  this->setAcceptDrops(true);
  this->setDropIndicatorShown(true);
}

void PictureList::dropEvent(QDropEvent *event) {
  if (event->mimeData()->hasFormat("application/x-item")) {
    event->accept();
    event->setDropAction(Qt::MoveAction);
    QString name = event->mimeData()->data("application/x-item");
    this->addPicture(name);
  } else
    event->ignore();
}

void PictureList::dragEnterEvent(QDragEnterEvent *event) {
  if (event->mimeData()->hasFormat("application/x-item"))
    event->accept();
  else
    event->ignore();
}

void PictureList::removeInvalid() {
  for (int i = 0; i < this->count(); i++) {
    auto item = this->item(i);
    auto path = item->text();
    if (!std::filesystem::exists(path.toStdString())) {
      this->removeItemWidget(item);
      delete item;
    }
  }
}

void PictureList::addPicture(QString path) {
  if (std::filesystem::exists(path.toStdString())) {
    auto icon = QIcon(path);
    if (icon.isNull()) {
      QMessageBox::warning(this, "GUU - Error", path + ": Error reading image");
    } else {
      QListWidgetItem *item = new QListWidgetItem(icon, path);
      this->addItem(item);
    }
  } else {
    QMessageBox::warning(this, "GUU - Error", path + ": File not found");
  }
}
