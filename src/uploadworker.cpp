#include "uploadworker.h"
#include <iostream>

#include "utils.h"
#include <filesystem>
#include <fstream>

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
    emit errorRaised("An error occured while parsing the input path.");
    return;
  }

  std::vector<char> torrent = {};
  try {
    torrent = utils::createTorrent(Data.path, parentDir);
  } catch (...) {
    emit errorRaised("An error occured while creating the torrent file.");
    return;
  }

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
      emit errorRaised("FATAL: Invalid API object.");
      return;
  }

  std::string url = Data.api->upload(uplData);

  if (Data.api->getLastError().code != cpr::ErrorCode::OK) {
      emit errorRaised("An error occured while uploading the torrent.");
      return;
  }

  emit textChanged("Verifying torrent...");
  emit valueChanged(currStage++);

  std::string tempPath = utils::tempDirPath() + "/dl.torrent";
  std::filesystem::remove(tempPath);

  if (!Data.api->download(url, tempPath)) {
      emit errorRaised("The uploaded torrent could not be verified.");
      return;
  }

  if (Cfg == nullptr) {
      emit errorRaised("FATAL: Invalid configuration object.");
      return;
  }
  if (Cfg->saveUploads) {
      emit textChanged("Saving torrent...");
      emit valueChanged(currStage++);

      std::string saveTo = Cfg->savePath + "/" + Data.title + ".torrent";

      try {
          std::filesystem::copy(tempPath, saveTo);
      } catch (...) {
          emit errorRaised("An error occured while saving the torrent.");
          return;
      }
  }

  if (Cfg->autoDl) {
      emit textChanged("Sending torrent to client...");
      emit valueChanged(currStage++);

      if (Client != nullptr) {
          std::basic_ifstream<char> file(tempPath, std::ios::binary);
          std::vector<char> torBytes((std::istreambuf_iterator<char>(file)),
                                     std::istreambuf_iterator<char>());
          if (!Client->addTorrent(torBytes, parentDir)) {
              emit errorRaised("An error occured while sending the torrent to client.");
              return;
          }
      } else {
          emit errorRaised("FATAL: Invalid client object.");
          return;
      }
  }

  emit finished();
}
