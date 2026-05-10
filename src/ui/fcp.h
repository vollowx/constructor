// Flexible Color Pairs
//
// Defines color pairs and cache the bg/fg combination for future reuse.

#ifndef FCP_H
#define FCP_H

#include <ncurses.h>

// Should be called after start_color()
void fcp_init(void);
short fcp_get(short fg, short bg);

#endif
