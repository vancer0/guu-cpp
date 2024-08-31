#ifndef PICTURELIST_H
#define PICTURELIST_H

#include <QListWidget>
#include <QDropEvent>
#include <QDragMoveEvent>
#include <QMimeData>
#include <string>

class PictureList : public QListWidget
{
    Q_OBJECT
public:
    PictureList(QWidget* parent);
    void addPicture(QString path);
protected:
    void dropEvent(QDropEvent* event);
    void dragEnterEvent(QDragEnterEvent* event);
signals:
    void itemDroped();
};

#endif // PICTURELIST_H
