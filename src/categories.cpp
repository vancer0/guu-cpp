#include "categories.h"
#include "nlohmann/json.hpp"
#include "utils.h"
#include <fstream>
#include <sstream>

using json = nlohmann::json;

Categories::Categories() {
  path = utils::tempDirPath() + "/categories-cache.json";
}

void Categories::parseFromApi(str data) {
  std::stringstream categs(data);
  str segment;
  std::vector<str> categListRaw;

  while (std::getline(categs, segment, '\n')) {
    categListRaw.push_back(segment);
  }
  categListRaw.erase(categListRaw.begin());

  for (auto c : categListRaw) {
    std::stringstream categ(c);
    str segment;
    std::vector<str> tmpVec;
    while (std::getline(categ, segment, ';')) {
      tmpVec.push_back(segment);
    }
    if (tmpVec.capacity() > 1)
      categories[tmpVec[1]] = tmpVec[0];
  }
}

void Categories::loadFromFile() {
  std::ifstream ifs(path);
  std::string content((std::istreambuf_iterator<char>(ifs)),
                      (std::istreambuf_iterator<char>()));

  auto categs = json::parse(content);

  for (auto c : categs.items())
    categories[c.key()] = c.value();
}

void Categories::saveToFile() {
  std::ofstream out(path);

  json categs;

  for (auto c : categories)
    categs[c.first] = c.second;

  out << categs.dump();
  out.close();
}

bool Categories::isEmpty() { return categories.empty(); }

std::map<str, str> Categories::get() { return categories; }
