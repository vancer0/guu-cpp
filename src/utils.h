#ifndef UTILS_H
#define UTILS_H

#include "sago/platform_folders.h"
#include "types.h"
#include <filesystem>
#include <functional>

namespace utils {
Path configDirPath();
Path tempDirPath();
Path logPath();
byteData createTorrent(Path path,
                       const std::function<void(int, int)> &progressCallback);
int fetchLatestVersion();
void checkForUpdates(bool msgIfLatest);
bool checkIfCommandExists(String cmd);
Path autoDetectqBitTorrentPath();

#ifdef _WIN32
void installWindowsUpdate(String url);
Path autoDetectUTorrentPath();
#endif
} // namespace utils

#endif // UTILS_H
