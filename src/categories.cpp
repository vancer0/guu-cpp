#include "categories.h"
#include "nlohmann/json.hpp"
#include "utils.h"
#include <fstream>
#include <sstream>

using json = nlohmann::json;

CategoriesParser::CategoriesParser() {
  _path = utils::tempDirPath() / "categories-cache.json";
}

void CategoriesParser::parseFromApi(String data) {
  std::stringstream categs(data);
  String segment;
  std::vector<String> categListRaw;

  while (std::getline(categs, segment, '\n')) {
    categListRaw.push_back(segment);
  }
  categListRaw.erase(categListRaw.begin());

  for (auto c : categListRaw) {
    std::stringstream categ(c);
    String segment;
    std::vector<String> tmpVec;
    while (std::getline(categ, segment, ';')) {
      tmpVec.push_back(segment);
    }
    if (tmpVec.capacity() > 1)
      categories[tmpVec[1]] = tmpVec[0];
  }
}

void CategoriesParser::loadFromFile() {
  std::ifstream ifs(_path);
  std::string content((std::istreambuf_iterator<char>(ifs)),
                      (std::istreambuf_iterator<char>()));

  auto categs = json::parse(content);

  for (auto c : categs.items())
    categories[c.key()] = c.value();
}

void CategoriesParser::saveToFile() {
  std::ofstream out(_path);

  json categs;

  for (auto c : categories)
    categs[c.first] = c.second;

  out << categs.dump();
  out.close();
}

bool CategoriesParser::isEmpty() { return categories.empty(); }

Categories CategoriesParser::get() { return categories; }
