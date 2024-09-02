#ifndef CATEGORIES_H
#define CATEGORIES_H

#include <map>
#include <string>

typedef std::string str;

class Categories {
private:
  std::map<str, str> categories;
  str path;

public:
  Categories();

  void parseFromApi(str data);

  void loadFromFile();
  void saveToFile();

  bool isEmpty();
  std::map<str, str> get();
};

#endif // CATEGORIES_H
