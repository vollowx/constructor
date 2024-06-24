#ifndef NCO_MENU_H
#define NCO_MENU_H

#include <functional>
#include <menu.h>
#include <ncurses.h>
#include <vector>

// nco: ncurses class overlay
namespace nco {

class menu {
public:
  menu();
  ~menu();

  void add_item(const char *name, const char *desc,
                std::function<void()> callback);
  void build_menu();
  void render();

private:
  MENU *_menu;
  std::vector<ITEM *> items;
  std::vector<std::function<void()>> callbacks;
};

} // namespace nco

#endif
