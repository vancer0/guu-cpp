#ifndef CLIENTS_H
#define CLIENTS_H

#include "cpr/cpr.h"
#include "settings.h"
#include "types.h"
#include <vector>

class TorrentClient {
public:
  virtual void configure(Settings *settings) {}
  virtual bool isConnected() { return false; }
  virtual String name() { return ""; }
  virtual bool addTorrent(Path torrent, Path localPath) { return false; }
  virtual ~TorrentClient() {}
};

class SystemTorrentHandler : public TorrentClient {
public:
  SystemTorrentHandler() {}

  String name() override { return "System Default"; }

  void configure(Settings *settings) override {}
  bool isConnected() override { return true; }
  bool addTorrent(Path torrent, Path localPath) override;

  ~SystemTorrentHandler() {}
};

class qBitTorrentWeb : public TorrentClient {
private:
  String _webUiUrl;
  cpr::Header _header;

public:
  qBitTorrentWeb() {}

  String name() override { return "qBitTorrent WebUI"; }

  void configure(Settings *settings) override;
  bool isConnected() override;
  bool addTorrent(Path torrent, Path localPath) override;

  ~qBitTorrentWeb();
};

class qBitTorrent : public TorrentClient {
private:
  Path _path;

public:
  qBitTorrent() {}

  String name() override { return "qBitTorrent"; }

  void configure(Settings *settings) override;
  bool isConnected() override;
  bool addTorrent(Path torrent, Path localPath) override;

  ~qBitTorrent() {}
};

#ifdef _WIN32
class uTorrent : public TorrentClient {
private:
  Path _path;

public:
  uTorrent() {}

  String name() override { return "uTorrent"; }

  void configure(Settings *settings) override;
  bool isConnected() override;
  bool addTorrent(Path torrent, Path localPath) override;
};
#endif

#endif // CLIENTS_H
