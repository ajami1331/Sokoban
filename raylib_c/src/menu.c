#include "menu.h"
#include "game.h"
#include "config.h"
#include "raylib.h"
#include <stdlib.h>

#define MAIN_MENU_ITEMS_COUNT 4

char *main_menu_items[MAIN_MENU_ITEMS_COUNT] = {
    "New Game",
    "Level Select",
    "Credits",
    "Exit",
};

enum menu_state menu = MENU_STATE_MENU;
int current_menu_item = 0;

void process_main_menu(void);
void draw_main_menu(void);
void check_input_in_main_menu(void);
void process_credits(void);
void process_level_select(void);
void draw_level_select(void);
void check_input_in_level_select(void);

void menu_reset_state(void)
{
    menu = MENU_STATE_MENU;
    current_menu_item = 0;
}

void menu_process_main_menu(void)
{
    switch (menu)
    {
    case MENU_STATE_MENU:
        process_main_menu();
        break;
    case MENU_STATE_LEVEL_SELECT:
        process_level_select();
        break;
    case MENU_STATE_CREDITS:
        process_credits();
        break;
    case MENU_STATE_EXIT:
        exit(0);
        break;
    
    default:
        break;
    }    
}

void process_main_menu(void)
{
    draw_main_menu();

    check_input_in_main_menu();
}

void draw_main_menu(void)
{
    int width = config_load()->screen_width;

    ClearBackground(SKYBLUE);

    DrawText("Sokoban", width / 3.5, 150, 76, WHITE);

    for (int i = 0; i < MAIN_MENU_ITEMS_COUNT; i++)
    {
        if (i == current_menu_item)
        {
            DrawText(main_menu_items[i], width / 2.5, 250 + (i * 50), 48, RED);
            continue;
        }
        DrawText(main_menu_items[i], width / 2.5, 250 + (i * 50), 48, WHITE);
    }
}

void check_input_in_main_menu(void)
{
    if (IsKeyPressed(KEY_UP))
    {
        if (current_menu_item == 0)
        {
            current_menu_item = MAIN_MENU_ITEMS_COUNT - 1;
            return;
        }
        current_menu_item--;
        return;
    }

    if (IsKeyPressed(KEY_DOWN))
    {
        if (current_menu_item == MAIN_MENU_ITEMS_COUNT - 1)
        {
            current_menu_item = 0;
            return;
        }
        current_menu_item++;
        return;
    }

    if (IsKeyPressed(KEY_ENTER))
    {
        switch (current_menu_item)
        {
        case 0:
            menu_reset_state();
            game_get_instance()->state = GAME_STATE_PLAYING;
            break;
        case 1:
            menu = MENU_STATE_LEVEL_SELECT;
            break;
        case 2:
            menu = MENU_STATE_CREDITS;
            break;
        case 3:
            menu = MENU_STATE_EXIT;
            break;
        
        default:
            break;
        }
    }
}

void process_credits(void)
{    
    int width = config_load()->screen_width;

    ClearBackground(SKYBLUE);
    
    DrawText("Credits", width / 3.5, 150, 76, WHITE);
    
    DrawText("Created by: ", width / 3.5, 250, 48, WHITE);
    DrawText("Araf Al Jami", width / 3.5, 300, 48, WHITE);
    
    DrawText("Assets from kenney.nl", width / 3.5, 400, 48, WHITE);

    DrawText("Press ESC to go back", width / 3.5, 500, 36, GRAY);

    if (IsKeyPressed(KEY_ESCAPE))
    {
        menu = MENU_STATE_MENU;
    }
}

void process_level_select(void)
{
    draw_level_select();

    check_input_in_level_select();
}

void draw_level_select(void)
{    
    int width = config_load()->screen_width;
    
    DrawText("Level Select", width / 3.5, 50, 36, GRAY);
    DrawText("Press ESC to go back", width / 3.5, 100, 36, GRAY);
}

void check_input_in_level_select(void)
{
    if (IsKeyPressed(KEY_ESCAPE))
    {
        menu = MENU_STATE_MENU;
    }
}
