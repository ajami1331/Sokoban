#include "menu.h"
#include "game.h"
#include "config.h"
#include <stdlib.h>

#if defined(PLATFORM_WEB)
    #define MAIN_MENU_ITEMS_COUNT 3
#else
    #define MAIN_MENU_ITEMS_COUNT 4
#endif

char *main_menu_items[MAIN_MENU_ITEMS_COUNT] = {
    "New Game",
    "Level Select",
    "Credits",
    "Exit",
};

enum menu_state menu = MENU_STATE_MENU;
int current_menu_item = 0;
int current_level_number = 0;

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
            DrawText(main_menu_items[i], width / 2.5, 250 + (i * 50), 48, ORANGE);
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

    int tile_size = 64;
    int tile_size_with_padding = tile_size + 8 + 8;
    int per_row = width / tile_size_with_padding;
    int y_offset = 180;
    int max_levels = config_load()->max_levels;

    if (current_level_number >= 50) 
    {
        y_offset -= ((current_level_number - 50) / 10 + 1) * tile_size_with_padding;
    }

    for (int i = 0; i < max_levels; i++)
    {
        int x = (i % per_row) * tile_size_with_padding;
        int y = (i / per_row) * tile_size_with_padding;
        DrawRectangle(8 + x, y_offset + y, tile_size, tile_size, WHITE);
        if (current_level_number == i)
        {
            TraceLog(LOG_INFO, "x: %i, y: %i", x, y);
            DrawText(TextFormat("%i", i + 1), 8 + x + 8, y_offset + y + 8, 48, ORANGE);
            continue;
        }

        DrawText(TextFormat("%i", i + 1), 8 + x + 8, y_offset + y + 8, 48, BLACK);
    }

    DrawText("Level Select", width / 3.5, 50, 36, GRAY);
    DrawText("Press ESC to go back", width / 3.5, 100, 36, GRAY);
}

void check_input_in_level_select(void)
{
    if (IsKeyPressed(KEY_ESCAPE))
    {
        menu = MENU_STATE_MENU;
    }

    int max_levels = config_load()->max_levels;

    if (IsKeyPressed(KEY_UP))
    {
        if (current_level_number - 10 >= 0)
        {
            current_level_number -= 10;
        }
        return;
    }

    if (IsKeyPressed(KEY_DOWN))
    {
        if (current_level_number + 10 < max_levels)
        {
            current_level_number += 10;
        }
        return;
    }

    if (IsKeyPressed(KEY_LEFT))
    {
        if (current_level_number - 1 >= 0)
        {
            current_level_number--;
        }
        return;
    }

    if (IsKeyPressed(KEY_RIGHT))
    {
        if (current_level_number + 1 < max_levels)
        {
            current_level_number++;
        }
        return;
    }

    if (IsKeyPressed(KEY_ENTER))
    {
        menu_reset_state();
        game_get_instance()->state = GAME_STATE_PLAYING;
        game_get_instance()->current_level_number = current_level_number;
        game_restart_current_level();
    }
}
