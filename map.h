#include "ncurses.h"
#include <vector>

class Map {
public:
  const unsigned width;
  const unsigned height;

private:
  std::vector<std::vector<unsigned>> data;

public:
  Map(unsigned width, unsigned height);

  const std::vector<unsigned> &operator[](unsigned index) const;
  std::vector<unsigned> &operator[](unsigned index);

  void reset();
  void fill(unsigned ax, unsigned ay, unsigned bx, unsigned by, unsigned btype);
};

void paintmap(unsigned ax, unsigned ay, const Map &map);
void rangepaintmap(unsigned ax, unsigned ay, unsigned x, unsigned y,
                   unsigned width, unsigned height, const Map &map);
