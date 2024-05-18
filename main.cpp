#include "map.h"
#include <ncurses.h>

int main() {
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);

  Map map(32, 16);

  int ax = (COLS - map.width * 2) / 2;
  int ay = (LINES - map.height) / 2;

  paintmap(ax, ay, map);

  mvprintw(0, 0, "Press `n` to render something new.");

  while (getch() != 'n')
    ;

  map.reset();
  map.fill(5, 5, 15, 15, 11);
  map.fill(10, 0, 12, 1, 77);

  rangepaintmap(ax, ay, 5, 5, 10, 10, map);
  rangepaintmap(ax, ay, 10, 0, 2, 1, map);

  mvprintw(1, 0,
           "This is the power of range rendering! Press `n` to fully "
           "rerender.");

  while (getch() != 'n')
    ;

  paintmap(ax, ay, map);

  mvprintw(2, 0,
           "Yes, they're the same if use range rendering properly. Press `n` "
           "to quit.");

  refresh();
  getch();

  endwin();

  return 0;
}
