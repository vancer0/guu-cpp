#ifndef UPLOADWORKER_H
#define UPLOADWORKER_H

#include <QObject>
#include <string>
#include <vector>
#include "api.h"
#include "settings.h"
#include "clients.h"

typedef std::string str;

class UploadWorker : public QObject
{
    Q_OBJECT
public:
    explicit UploadWorker(QObject *parent = nullptr);

    struct WorkerInputData {
        API *api = nullptr;
        str path = "";
        str categ = "";
        str sCateg1 = "";
        str sCateg2 = "";
        str sCateg3 = "";
        str sCateg4 = "";
        str title = "";
        str description = "";
        std::vector<str> images = {};
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
    void errorRaised(const QString &value);
    void finished();

public slots:
    void doWork();
};

#endif // UPLOADWORKER_H
