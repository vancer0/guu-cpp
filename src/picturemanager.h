#ifndef PICTUREMANAGER_H
#define PICTUREMANAGER_H

#include <QListWidgetItem>
#include <QWidget>

namespace Ui {
class PictureManager;
}

class PictureManager : public QWidget {
  Q_OBJECT

public:
  explicit PictureManager(QWidget *parent = nullptr);
  QList<QListWidgetItem *> getList();
  void addPicture(QString path);
  void removeInvalid();

  void selectPictures();
  void removePictures();
  ~PictureManager();

private:
  Ui::PictureManager *ui;

signals:
  void modified();
};

#endif // PICTUREMANAGER_H
