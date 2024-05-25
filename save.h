#ifndef SAVE_H
#define SAVE_H

#include "map.h"
#include <fstream>
#include <iostream>
#include <string>

class Save {
public:
  Save(unsigned width, unsigned height);

  Map map;

  void save(const char *filepath);
  void load(const char *filepath);
};

#endif
