#include "picturelist.h"
#include <QMessageBox>
#include <QPainter>
#include <filesystem>
#include <string>

PictureList::PictureList(QWidget *parent) : QListWidget(parent) {
  this->setAcceptDrops(true);
  this->setIconSize(QSize(100, 100));
  this->setSelectionMode(QAbstractItemView::ExtendedSelection);
  this->setViewMode(QListView::IconMode);
  this->setResizeMode(QListView::Adjust);
  this->setMovement(Movement::Free);
}

void PictureList::dropEvent(QDropEvent *event) {
  if (_allowDrops)
    foreach (const QUrl &url, event->mimeData()->urls()) {
      QString fileName = url.toLocalFile();
      this->addPicture(fileName);
    }
}

void PictureList::dragEnterEvent(QDragEnterEvent *event) {
  if (event->mimeData()->hasUrls() && _allowDrops) {
    event->acceptProposedAction();
  }
}

void PictureList::dragMoveEvent(QDragMoveEvent *event) {
  if (event->mimeData()->hasUrls() && _allowDrops) {
    event->acceptProposedAction();
  }
}

void PictureList::dragLeaveEvent(QDragLeaveEvent *event) {
  if (_allowDrops)
    event->accept();
}

void PictureList::paintEvent(QPaintEvent *e) {
  QListView::paintEvent(e);
  if (model() && model()->rowCount(rootIndex()) > 0)
    return;

  QPainter p(this->viewport());
  p.setOpacity(0.5);
  QString text;
  if (_allowDrops)
    text = "Click + to select or\nDrag & Drop pictures";
  else
    text = "No Pictures";
  p.drawText(rect(), Qt::AlignCenter, text);
}

void PictureList::allowDrops(bool allow) { _allowDrops = allow; }

void PictureList::removeInvalid() {
  for (int i = 0; i < this->count(); i++) {
    auto item = this->item(i);
    auto path = item->data(Qt::UserRole).toString();
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
      QListWidgetItem *item = new QListWidgetItem();
      item->setIcon(icon);
      item->setData(Qt::UserRole, path);
      this->addItem(item);
    }
  } else {
    QMessageBox::warning(this, "GUU - Error", path + ": File not found");
  }
}

void PictureList::moveDown() {
  auto selected = this->selectedItems();
  if (selected.empty()) {
    return;
  } else {
    QListWidgetItem *current = selected[0];
    if (current == nullptr)
      return;
    int currIndex = this->row(current);

    QListWidgetItem *next = this->item(this->row(current) + 1);
    int nextIndex = this->row(next);

    QListWidgetItem *temp = this->takeItem(nextIndex);
    this->insertItem(currIndex, temp);
    this->insertItem(nextIndex, current);
  }
}

void PictureList::moveUp() {
  auto selected = this->selectedItems();
  if (selected.empty()) {
    return;
  } else {
    QListWidgetItem *current = selected[0];
    if (current == nullptr)
      return;
    int currIndex = this->row(current);

    QListWidgetItem *prev = this->item(this->row(current) - 1);
    int prevIndex = this->row(prev);

    QListWidgetItem *temp = this->takeItem(prevIndex);
    this->insertItem(prevIndex, current);
    this->insertItem(currIndex, temp);
  }
}
