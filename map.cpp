#include "map.h"

Map::Map(unsigned width, unsigned height)
    : width(width), height(height),
      data(height, std::vector<unsigned>(width, 0)) {}

const std::vector<unsigned> &Map::operator[](unsigned index) const {
  return data[index];
}

std::vector<unsigned> &Map::operator[](unsigned index) { return data[index]; }

void Map::reset() {
  for (unsigned y = 0; y < height; ++y) {
    for (unsigned x = 0; x < width; ++x) {
      data[y][x] = 0;
    }
  }
}

void Map::fill(unsigned ax, unsigned ay, unsigned bx, unsigned by,
               unsigned btype) {
  for (unsigned y = ay; y < by; ++y) {
    for (unsigned x = ax; x < bx; ++x) {
      data[y][x] = btype;
    }
  }
}

void mvprintmap(unsigned ax, unsigned ay, const Map &map) {
  for (unsigned y = 0; y < map.height; ++y) {
    for (unsigned x = 0; x < map.width; ++x) {
      mvprintw(ay + y, ax + x * 2, "%02u", map[y][x]);
    }
  }
}

void mvprintmaprange(unsigned ax, unsigned ay, unsigned x, unsigned y,
                     unsigned width, unsigned height, const Map &map) {
  for (unsigned i = 0; i < height; ++i) {
    for (unsigned j = 0; j < width; ++j) {
      mvprintw(ay + y + i, ax + (x + j) * 2, "%02u", map[y + i][x + j]);
    }
  }
}
