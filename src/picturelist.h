#ifndef PICTURELIST_H
#define PICTURELIST_H

#include <QDragMoveEvent>
#include <QDropEvent>
#include <QListWidget>
#include <QMimeData>
#include <string>

class PictureList : public QListWidget {
  Q_OBJECT
public:
  PictureList(QWidget *parent);
  void addPicture(QString path);
  void removeInvalid();

  void moveDown();
  void moveUp();

  void allowDrops(bool allow);

protected:
  void dragEnterEvent(QDragEnterEvent *event);
  void dragMoveEvent(QDragMoveEvent *event);
  void dragLeaveEvent(QDragLeaveEvent *event);
  void dropEvent(QDropEvent *event);
  void paintEvent(QPaintEvent *e);
signals:
  void itemDroped();

private:
  bool _allowDrops = true;
};

#endif // PICTURELIST_H
