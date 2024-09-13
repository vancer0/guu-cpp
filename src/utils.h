#ifndef UTILS_H
#define UTILS_H

#include "sago/platform_folders.h"
#include <filesystem>
#include <string>
#include <vector>

typedef std::string str;

namespace utils {
str configDirPath();
str tempDirPath();
str logPath();
std::vector<char> createTorrent(str path, str parentDir);
int fetchLatestVersion();
void checkForUpdates(bool msgIfLatest);
bool checkIfCommandExists(str path);
str autoDetectqBitTorrentPath();

#ifdef _WIN32
void installWindowsUpdate(str url);
str autoDetectUTorrentPath();
#endif
} // namespace utils

#endif // UTILS_H
