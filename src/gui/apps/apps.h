#ifndef APPS_H
#define APPS_H

#include "gui.h"

void open_about();
void open_paint();
void open_memviz();
void open_viewer3d();

//terminal
void open_terminal();
void terminal_print(const char * str);
void terminal_clear();

#endif
