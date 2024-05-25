#include "save.h"

Save::Save(unsigned width, unsigned height) : map(width, height) {}

void Save::save(const char *filepath) {
  std::ofstream file(filepath);

  if (file.is_open()) {
    file << map.width << " " << map.height << std::endl;

    for (unsigned y = 0; y < map.height; ++y) {
      for (unsigned x = 0; x < map.width; ++x) {
        file << map[y][x] << " ";
      }
      file << std::endl;
    }

    file.close();
    mvprintw(LINES - 1, 0, "Map saved to %s", filepath);
  } else {
    mvprintw(LINES - 1, 0, "Unable to open file for saving");
  }
}

void Save::load(const char *filepath) {
  std::ifstream file(filepath);

  if (file.is_open()) {
    unsigned width, height;
    file >> width >> height;

    for (unsigned y = 0; y < height; ++y) {
      for (unsigned x = 0; x < width; ++x) {
        unsigned value;
        file >> value;
        map[y][x] = value;
      }
    }

    file.close();
    mvprintw(LINES - 1, 0, "Map loaded from %s", filepath);
  } else {
    mvprintw(LINES - 1, 0, "Unable to open file for loading");
  }
}
