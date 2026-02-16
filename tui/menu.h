#ifndef NCO_MENU_H
#define NCO_MENU_H

#include <functional>
#include <menu.h>
#include <ncurses.h>
#include <string>
#include <vector>

// nco: ncurses class overlay
namespace nco {

class menu {
public:
  menu();
  ~menu();

  // mark
  int set_mark(const char *mark);
  const char *get_mark();

  // item
  int count();
  void add_item(const char *name, const char *desc,
                std::function<void()> callback);

  // non-ncurses
  void set_title(const std::string &title);
  const std::string &get_title() const;
  void render();
  void exec_callback();

private:
  WINDOW *_window;
  MENU *_menu;
  std::string _title;
  std::vector<ITEM *> _items;
  std::vector<std::function<void()>> _callbacks;
  int _selected_index;
};

} // namespace nco

#endif
