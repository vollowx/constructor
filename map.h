#ifndef MAP_H
#define MAP_H

#include <ncurses.h>
#include <vector>

class Map {
private:
  std::vector<std::vector<unsigned>> data;

public:
  const unsigned width;
  const unsigned height;

  Map(unsigned width, unsigned height);

  const std::vector<unsigned> &operator[](unsigned index) const;
  std::vector<unsigned> &operator[](unsigned index);

  void reset();
  void fill(unsigned ax, unsigned ay, unsigned width, unsigned height,
            unsigned btype);
};

void mvprintmap(unsigned ax, unsigned ay, const Map &map);
void mvprintmaprange(unsigned ax, unsigned ay, unsigned x, unsigned y,
                     unsigned width, unsigned height, const Map &map);
void mvwprintmap(WINDOW *window, unsigned ax, unsigned ay, const Map &map);
void mvwprintmaprange(WINDOW *window, unsigned ax, unsigned ay, unsigned x,
                      unsigned y, unsigned width, unsigned height,
                      const Map &map);

#endif
