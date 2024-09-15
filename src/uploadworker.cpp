#include "uploadworker.h"
#include "utils.h"
#include <QDebug>
#include <filesystem>
#include <fstream>
#include <iostream>

UploadWorker::UploadWorker(QObject *parent) : QObject(parent) {}

void UploadWorker::configure(Settings *cfg, TorrentClient *client) {
  Cfg = cfg;
  Client = client;

  // 200: Torrent creation
  // 100: Upload
  // 50: Verify
  Stages = 350;
  if (Cfg != nullptr) {
    if (Cfg->autoDl)
      Stages += 50;
    if (Cfg->saveUploads)
      Stages += 50;
  }
}

void UploadWorker::run(WorkerInputData data) {
  Data = data;

  emit workRequested();
}

void UploadWorker::doWork() {
  int currStage = 0;
  emit valueChanged(currStage);

  qInfo() << "Creating torrent";
  emit textChanged("Creating torrent...");

  byteData torrent;
  try {
    torrent =
        utils::createTorrent(Data.path, [this, currStage](int curr, int total) {
          float p = 200.0 * ((float)curr / (float)total);
          emit valueChanged(currStage + (int)p);
          emit textChanged("Creating torrent... (" + QString::number(curr) +
                           '/' + QString::number(total) + ')');
        });
  } catch (const std::exception &e) {
    qWarning() << "Exception raised while creating the torrent:" << e.what();
    emit errorRaised("An error occured while creating the torrent.");
    return;
  }
  currStage += 200;
  emit valueChanged(currStage);

  qInfo() << "Uploading torrent";
  emit textChanged("Uploading torrent...");

  API::UploadData uplData;
  uplData.torrent = torrent;
  uplData.picPaths = Data.images;
  uplData.title = Data.title;
  uplData.description = Data.description;
  uplData.mainCateg = Data.categ;
  uplData.sCateg1 = Data.sCateg1;
  uplData.sCateg2 = Data.sCateg2;
  uplData.sCateg3 = Data.sCateg3;
  uplData.sCateg4 = Data.sCateg4;

  if (Data.api == nullptr) {
    qCritical() << "API is NULL...";
    emit errorRaised("FATAL: Invalid API object.");
    return;
  }

  String url;
  try {
    url = Data.api->upload(uplData, [this, currStage](int curr, int total) {
      float p = 100.0 * ((float)curr / (float)total);
      emit valueChanged(currStage + (int)p);
      emit textChanged("Uploading torrent... (" + QString::number(curr) + '/' +
                       QString::number(total) + ')');
    });
  } catch (const std::exception &e) {
    qCritical() << e.what();
    emit errorRaised("An error occured while uploading the torrent.");
    return;
  }
  currStage += 100;
  emit valueChanged(currStage);

  qInfo() << "Downloading torrent:" << QString::fromStdString(url);
  emit textChanged("Verifying torrent...");

  Path tempPath = utils::tempDirPath() / "dl.torrent";
  std::filesystem::remove(tempPath);

  if (!Data.api->download(url, tempPath)) {
    qCritical() << "Error downloading torrent:" << Data.api->getLastStatusCode()
                << Data.api->getLastError().message;
    emit errorRaised("The uploaded torrent could not be verified.");
    return;
  }
  currStage += 50;
  emit valueChanged(currStage);

  if (Cfg == nullptr) {
    qCritical() << "Settings is NULL";
    emit errorRaised("FATAL: Invalid configuration object.");
    return;
  }

  if (Cfg->saveUploads) {
    emit textChanged("Saving torrent...");

    Path saveTo(Cfg->savePath + "/" + Data.title + ".torrent");
    qInfo() << "Saving torrent to:" << QString::fromStdString(saveTo.string());

    try {
      std::filesystem::remove(saveTo);
      std::filesystem::copy(tempPath, saveTo);
    } catch (const std::exception &e) {
      qWarning() << "Error saving torrent:" << e.what();
      emit warningRaised("An error occured while saving the torrent.");
    }
    currStage += 50;
    emit valueChanged(currStage);
  }

  if (Cfg->autoDl) {
    qInfo() << "Sending torrent to client";
    emit textChanged("Sending torrent to client...");

    if (Client != nullptr) {
      if (!Client->addTorrent(tempPath, Data.path.parent_path().string())) {
        qWarning() << "Error sending the torrent to the client";
        emit warningRaised(
            "An error occured while sending the torrent to the client.");
      }
      currStage += 50;
      emit valueChanged(currStage);
    } else {
      qCritical() << "Client is NULL...";
      emit errorRaised("FATAL: Invalid client object.");
      return;
    }
  }

  emit finished();
}
