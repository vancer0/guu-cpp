#ifndef CLIENTS_H
#define CLIENTS_H

#include "cpr/cpr.h"
#include <vector>
#include "settings.h"

#define CLIENT_TIMEOUT 10000 // ms

typedef std::string str;

class TorrentClient
{
public:
    virtual void configure(Settings &settings) {}
    virtual bool isConnected() { return false; }
    virtual str name() { return ""; }
    virtual bool addTorrent(std::vector<char> torrent, str localPath) { return false; }
    virtual ~TorrentClient() {}
};

class qBitTorrent : public TorrentClient
{
private:
    str webUiUrl;
    cpr::Header header;
public:
    qBitTorrent() {}

    str name() override {
        return "qBitTorrent";
    }

    void configure(Settings &settings) override;
    bool isConnected() override;
    bool addTorrent(std::vector<char> torrent, str localPath) override;

    ~qBitTorrent();
};

#endif // CLIENTS_H
