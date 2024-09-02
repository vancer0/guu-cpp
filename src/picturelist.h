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

protected:
  void dropEvent(QDropEvent *event);
  void dragEnterEvent(QDragEnterEvent *event);
signals:
  void itemDroped();
};

#endif // PICTURELIST_H
