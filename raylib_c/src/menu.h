#ifndef MENU_H
#define MENU_H 1

#include "common.h"

enum menu_state {
    MENU_STATE_MENU,
    MENU_STATE_LEVEL_SELECT,
    MENU_STATE_CREDITS,
    MENU_STATE_EXIT,
};

void menu_reset_state(void);

void menu_process_main_menu(void);

#endif
