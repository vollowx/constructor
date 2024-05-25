#include "map.h"
#include "save.h"
#include <ncurses.h>

#define KEY_CTRL(c) ((c) & 0x1f)

#define clearline(y)                                                           \
  do {                                                                         \
    move(y, 0);                                                                \
    clrtoeol();                                                                \
  } while (0)

const int MAPWIN_HEIGHT = 18;
const int MAPWIN_WIDTH = 66;

int main() {
  srand(time(0));

  initscr();
  cbreak();
  noecho();
  keypad(stdscr, true);
  raw();

  curs_set(false);

  start_color();
  use_default_colors();

  Save save(32, 16);

  int ax, ay;

  ax = (COLS - MAPWIN_WIDTH) / 2;
  ay = (LINES - MAPWIN_HEIGHT) / 2;

  WINDOW *mapwin = newwin(MAPWIN_HEIGHT, MAPWIN_WIDTH, ay, ax);
  box(mapwin, 0, 0);
  mvwprintw(mapwin, 0, 2, "Map");
  mvwprintmap(mapwin, 1, 1, save.map);

  while (true) {
    refresh();
    wrefresh(mapwin);

    int input = getch();
    bool quit = false;

    clearline(LINES - 1);
    mvprintw(0, 0, "Just received %d", input);
    clrtoeol();

    switch (input) {
    case KEY_CTRL('c'):
      quit = true;

    case KEY_RESIZE:
      touchline(stdscr, ay, MAPWIN_HEIGHT);
      ax = (COLS - MAPWIN_WIDTH) / 2;
      ay = (LINES - MAPWIN_HEIGHT) / 2;
      if (OK != mvwin(mapwin, ay, ax))
        mvprintw(1, 0, "Error moving window <%p>", (void *)mapwin);
      else
        clearline(1);
      break;

    case KEY_CTRL('o'):
      save.load("save.dat");
      mvwprintmap(mapwin, 1, 1, save.map);
      break;

    case KEY_CTRL('s'):
      save.save("save.dat");
      break;

    case 'r':
      save.map.reset();
      mvwprintmap(mapwin, 1, 1, save.map);

      mvprintw(LINES - 1, 0, "Map reset");

      break;

    case 'a':
      int rw = rand() % (save.map.width / 2);
      int rh = rand() % (save.map.height / 2);

      int rx = rand() % (save.map.width - rw + 1);
      int ry = rand() % (save.map.height - rh + 1);

      int rn = (rand() % 9) * 11;

      save.map.fill(rx, ry, rw, rh, rn);
      mvwprintmaprange(mapwin, 1, 1, rx, ry, rw, rh, save.map);

      mvprintw(LINES - 1, 0, "Something is added randomly to map");

      break;
    }

    if (quit)
      break;
  }

  delwin(mapwin);
  endwin();

  return 0;
}
