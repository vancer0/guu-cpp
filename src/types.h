#ifndef TYPES_H
#define TYPES_H

#include <filesystem>
#include <map>
#include <string>
#include <vector>

typedef std::string String;

using Path = std::filesystem::path;

typedef std::vector<char> byteData;

typedef std::map<String, String> Categories;

#endif // TYPES_H
