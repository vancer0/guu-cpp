#include "uploadworker.h"
#include <iostream>

#include <QTimer>
#include <QEventLoop>
#include "utils.h"
#include <filesystem>
#include <fstream>

UploadWorker::UploadWorker(QObject *parent) :
    QObject(parent) {}

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
        emit errorRaised("Error parsing input path.");
        return;
    }

    std::vector<char> torrent = {};
    try {
        torrent = utils::createTorrent(Data.path, parentDir);
    } catch (...) {
        emit errorRaised("Error creating torrent.");
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

    if (Data.api != nullptr) {
        std::string url = Data.api->upload(uplData);

        if (Data.api->getLastError().code != cpr::ErrorCode::OK) {
            emit errorRaised("Error uploading torrent.");
            return;
        }

        emit textChanged("Verifying torrent...");
        emit valueChanged(currStage++);

        std::string tempPath = utils::tempDirPath() + "dl.torrent";
        if (!Data.api->download(url, tempPath)) {
            emit errorRaised("Error verifying torrent.");
            return;
        }

        if (Cfg != nullptr) {
            if (Cfg->saveUploads) {
                emit textChanged("Saving torrent...");
                emit valueChanged(currStage++);

                std::string saveTo =
                    Cfg->savePath + "/" + Data.title + ".torrent";

                try {
                    std::filesystem::copy(tempPath, saveTo);
                } catch (...) {
                    emit errorRaised("Error saving torrent.");
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
                    if (!Client->addTorrent(torBytes, Data.path)) {
                        emit errorRaised("Error sending torrent to client.");
                        return;
                    }
                } else {
                    emit errorRaised("API error.");
                    return;
                }
            }
        } else {
            emit errorRaised("Configuration error.");
            return;
        }
    } else {
        emit errorRaised("API error.");
        return;
    }
    emit finished();
}
