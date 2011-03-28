#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "gui.h"

extern void KeyboardMenu(gui_menu *parent, const char *name, char *string, size_t size, int (*isValid)(const char *new, const char *old));

#endif
