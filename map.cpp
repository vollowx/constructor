#include "map.h"

Map::Map(unsigned width, unsigned height)
    : data(height, std::vector<unsigned>(width, 0)), width(width),
      height(height) {}

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

void Map::fill(unsigned ax, unsigned ay, unsigned width, unsigned height,
               unsigned btype) {
  for (unsigned y = ay; y < ay + height; ++y) {
    for (unsigned x = ax; x < ax + width; ++x) {
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

void mvwprintmap(WINDOW *window, unsigned ax, unsigned ay, const Map &map) {
  for (unsigned y = 0; y < map.height; ++y) {
    for (unsigned x = 0; x < map.width; ++x) {
      mvwprintw(window, ay + y, ax + x * 2, "%02u", map[y][x]);
    }
  }
}

void mvwprintmaprange(WINDOW *window, unsigned ax, unsigned ay, unsigned x,
                      unsigned y, unsigned width, unsigned height,
                      const Map &map) {
  for (unsigned i = 0; i < height; ++i) {
    for (unsigned j = 0; j < width; ++j) {
      mvwprintw(window, ay + y + i, ax + (x + j) * 2, "%02u",
                map[y + i][x + j]);
    }
  }
}
