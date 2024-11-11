#ifndef UPLOADWORKER_H
#define UPLOADWORKER_H

#include "api.h"
#include "clients.h"
#include "settings.h"
#include <QObject>

class UploadWorker : public QObject {
  Q_OBJECT
public:
  explicit UploadWorker(QObject *parent = nullptr);

  struct WorkerInputData {
    API *api = nullptr;
    Path path = "";
    bool forceV1 = false;
    String categ = "";
    String sCateg1 = "";
    String sCateg2 = "";
    String sCateg3 = "";
    String sCateg4 = "";
    String title = "";
    String description = "";
    std::vector<Path> images = {};
  };

  void configure(Settings *cfg, TorrentClient *client);
  void run(WorkerInputData data);

  int Stages = 1;

private:
  WorkerInputData Data;
  Settings *Cfg = nullptr;
  TorrentClient *Client = nullptr;

signals:
  void workRequested();
  void textChanged(const QString &value);
  void valueChanged(const int &value);
  void warningRaised(const QString &value);
  void errorRaised(const QString &value);
  void finished();

public slots:
  void doWork();
};

#endif // UPLOADWORKER_H
