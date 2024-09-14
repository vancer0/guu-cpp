#ifndef CATEGORIES_H
#define CATEGORIES_H

#include "types.h"

class CategoriesParser {
private:
  Categories categories;
  Path _path;

public:
  CategoriesParser();

  void parseFromApi(String data);

  void loadFromFile();
  void saveToFile();

  bool isEmpty();
  Categories get();
};

#endif // CATEGORIES_H
