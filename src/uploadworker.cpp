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

  Stages = 3;
  if (Cfg != nullptr) {
    if (Cfg->autoDl)
      Stages += 1;
    if (Cfg->saveUploads)
      Stages += 1;
  }
}

void UploadWorker::run(WorkerInputData data) {
  Data = data;

  emit workRequested();
}

void UploadWorker::doWork() {
  int currStage = 1;

  qInfo() << "Creating torrent";
  emit textChanged("Creating torrent...");
  emit valueChanged(currStage++);

  std::string parentDir = "";
  try {
    if (std::filesystem::is_directory(Data.path))
      parentDir = Data.path;
    else if (std::filesystem::is_regular_file(Data.path)) {
      std::filesystem::path tmp(Data.path);
      parentDir = {tmp.parent_path().u8string()};
    }
  } catch (...) {
    qWarning() << "Exception raised while parsing parent path";
    emit errorRaised("An error occured while parsing the input path.");
    return;
  }

  std::vector<char> torrent = {};
  try {
    torrent = utils::createTorrent(Data.path, parentDir);
  } catch (...) {
    qWarning() << "Exception raised while creating the torrent";
    emit errorRaised("An error occured while creating the torrent file.");
    return;
  }

  qInfo() << "Uploading torrent";
  emit textChanged("Uploading torrent...");
  emit valueChanged(currStage++);

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

  auto urlres = Data.api->upload(uplData);
  if (!urlres.has_value()) {
    qCritical() << "Error uploading:" << Data.api->getLastStatusCode()
                << Data.api->getLastError().message;
    emit errorRaised("An error occured while uploading the torrent.");
    return;
  }
  std::string url = urlres.value();

  qInfo() << "Downloading torrent:" << QString::fromStdString(url);
  emit textChanged("Verifying torrent...");
  emit valueChanged(currStage++);

  std::string tempPath = utils::tempDirPath() + "/dl.torrent";
  std::filesystem::remove(tempPath);

  if (!Data.api->download(url, tempPath)) {
    qCritical() << "Error downloading torrent:" << Data.api->getLastStatusCode()
                << Data.api->getLastError().message;
    emit errorRaised("The uploaded torrent could not be verified.");
    return;
  }

  if (Cfg == nullptr) {
    qCritical() << "Settings is NULL";
    emit errorRaised("FATAL: Invalid configuration object.");
    return;
  }

  if (Cfg->saveUploads) {
    emit textChanged("Saving torrent...");
    emit valueChanged(currStage++);

    std::string saveTo = Cfg->savePath + "/" + Data.title + ".torrent";
    qInfo() << "Saving torrent to:" << QString::fromStdString(saveTo);

    try {
      std::filesystem::copy(tempPath, saveTo);
    } catch (...) {
      qCritical() << "Error saving torrent";
      emit errorRaised("An error occured while saving the torrent.");
      return;
    }
  }

  if (Cfg->autoDl) {
    qInfo() << "Sending torrent to client";
    emit textChanged("Sending torrent to client...");
    emit valueChanged(currStage++);

    if (Client != nullptr) {
      if (!Client->addTorrent(tempPath, parentDir)) {
        qCritical() << "Error sending the torrent to the client";
        emit errorRaised(
            "An error occured while sending the torrent to the client.");
        return;
      }
    } else {
      qCritical() << "Client is NULL...";
      emit errorRaised("FATAL: Invalid client object.");
      return;
    }
  }

  emit finished();
}
