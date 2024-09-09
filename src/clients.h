#ifndef CLIENTS_H
#define CLIENTS_H

#include "cpr/cpr.h"
#include "settings.h"
#include <vector>

#define CLIENT_TIMEOUT 1000      // ms
#define CLIENT_UPL_TIMEOUT 60000 // ms

typedef std::string str;

class TorrentClient {
public:
  virtual void configure(Settings *settings) {}
  virtual bool isConnected() { return false; }
  virtual str name() { return ""; }
  virtual bool addTorrent(str torrent, str localPath) { return false; }
  virtual ~TorrentClient() {}
};

class SystemTorrentHandler : public TorrentClient {
public:
  SystemTorrentHandler() {}

  str name() override { return "System Default"; }

  void configure(Settings *settings) override {}
  bool isConnected() override { return true; }
  bool addTorrent(str torrent, str localPath) override;

  ~SystemTorrentHandler() {}
};

class qBitTorrentWeb : public TorrentClient {
private:
  str webUiUrl;
  cpr::Header header;

public:
  qBitTorrentWeb() {}

  str name() override { return "qBitTorrent WebUI"; }

  void configure(Settings *settings) override;
  bool isConnected() override;
  bool addTorrent(str torrent, str localPath) override;

  ~qBitTorrentWeb();
};

class qBitTorrent : public TorrentClient {
private:
  str Path;

public:
  qBitTorrent() {}

  str name() override { return "qBitTorrent"; }

  void configure(Settings *settings) override;
  bool isConnected() override;
  bool addTorrent(str torrent, str localPath) override;

  ~qBitTorrent() {}
};

#ifdef _WIN32
class uTorrent : public TorrentClient {
private:
  str Path;

public:
  uTorrent() {}

  str name() override { return "uTorrent"; }

  void configure(Settings *settings) override;
  bool isConnected() override;
  bool addTorrent(str torrent, str localPath) override;
};
#endif

#endif // CLIENTS_H
