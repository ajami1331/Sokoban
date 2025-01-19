#include "audio.h"
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
int level_from_mouse_input(void);

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
    int width = GetScreenWidth();

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
    Vector2 cursor_position = GetMousePosition();

    for (int i = 0; i < MAIN_MENU_ITEMS_COUNT; i++)
    {
        if (cursor_position.y >= 250 + (i * 50) && cursor_position.y <= 250 + (i * 50) + 50 && cursor_position.x >= GetScreenWidth() / 2.5 && cursor_position.x <= GetScreenWidth() / 2.5 + 200)
        {
            if (current_menu_item == i)
            {
                break;
            }
            audio_play_menu_item_switch_sound();
            current_menu_item = i;
        }
    }

    if (IsKeyPressed(KEY_UP))
    {
        audio_play_menu_item_switch_sound();
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
        audio_play_menu_item_switch_sound();
        if (current_menu_item == MAIN_MENU_ITEMS_COUNT - 1)
        {
            current_menu_item = 0;
            return;
        }
        current_menu_item++;
        return;
    }

    if (IsKeyPressed(KEY_ENTER) ||
        (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && cursor_position.x >= GetScreenWidth() / 2.5 && cursor_position.x <= GetScreenWidth() / 2.5 + 200))
    {
        audio_play_menu_item_switch_sound();
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
    int width = GetScreenWidth();

    ClearBackground(SKYBLUE);

    DrawText("Credits", width / 3.5, 150, 76, WHITE);

    DrawText("Created by: ", width / 3.5, 250, 48, WHITE);
    DrawText("Araf Al Jami", width / 3.5, 300, 48, WHITE);

    DrawText("Assets from kenney.nl", width / 3.5, 400, 48, WHITE);

    Vector2 cursor_position = GetMousePosition();

    bool is_mouse_over =
        (cursor_position.x >= width / 3.5 && cursor_position.x <= width / 3.5 + 252 && cursor_position.y >= 500 && cursor_position.y <= 550) ||
        (cursor_position.x >= width / 3.5 && cursor_position.x <= width / 3.5 + 278 && cursor_position.y >= 550 && cursor_position.y <= 600);

    DrawText("Press ESC or", width / 3.5, 500, 36, is_mouse_over ? ORANGE : GRAY);
    DrawText("click to go back", width / 3.5, 550, 36, is_mouse_over ? ORANGE : GRAY);

    if (IsKeyPressed(KEY_ESCAPE) || (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && is_mouse_over))
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
    int width = GetScreenWidth();

    int tile_size = 64;
    int tile_size_with_padding = tile_size + 8 + 8;
    int per_row = width / tile_size_with_padding;
    int y_offset = 220;
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

    DrawRectangle(width / 3.5 - 25, 30, 320, 170, SKYBLUE);

    DrawText("Level Select", width / 3.5, 50, 42, WHITE);

    Vector2 cursor_position = GetMousePosition();

    bool is_mouse_over =
        (cursor_position.x >= width / 3.5 && cursor_position.x <= width / 3.5 + 252 && cursor_position.y >= 100 && cursor_position.y <= 150) ||
        (cursor_position.x >= width / 3.5 && cursor_position.x <= width / 3.5 + 278 && cursor_position.y >= 150 && cursor_position.y <= 190);

    DrawText("Press ESC or", width / 3.5, 100, 36, is_mouse_over ? ORANGE : GRAY);
    DrawText("click to go back", width / 3.5, 150, 36, is_mouse_over ? ORANGE : GRAY);
}

void check_input_in_level_select(void)
{
    Vector2 cursor_position = GetMousePosition();
    int width = GetScreenWidth();

    bool is_mouse_over_esc =
        (cursor_position.x >= width / 3.5 && cursor_position.x <= width / 3.5 + 252 && cursor_position.y >= 100 && cursor_position.y <= 150) ||
        (cursor_position.x >= width / 3.5 && cursor_position.x <= width / 3.5 + 278 && cursor_position.y >= 150 && cursor_position.y <= 190);

    if (IsKeyPressed(KEY_ESCAPE) || (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && is_mouse_over_esc))
    {
        audio_play_menu_item_switch_sound();
        menu = MENU_STATE_MENU;
    }

    int max_levels = config_load()->max_levels;

    int level_from_mouse = level_from_mouse_input();

    if (level_from_mouse != -1 && level_from_mouse != current_level_number)
    {
        audio_play_menu_item_switch_sound();
        current_level_number = level_from_mouse;
    }

    if (IsKeyPressed(KEY_UP))
    {
        audio_play_menu_item_switch_sound();
        if (current_level_number - 10 >= 0)
        {
            current_level_number -= 10;
        }
        return;
    }

    if (IsKeyPressed(KEY_DOWN))
    {
        audio_play_menu_item_switch_sound();
        if (current_level_number + 10 < max_levels)
        {
            current_level_number += 10;
        }
        return;
    }

    if (IsKeyPressed(KEY_LEFT))
    {
        audio_play_menu_item_switch_sound();
        if (current_level_number - 1 >= 0)
        {
            current_level_number--;
        }
        return;
    }

    if (IsKeyPressed(KEY_RIGHT))
    {
        audio_play_menu_item_switch_sound();
        if (current_level_number + 1 < max_levels)
        {
            current_level_number++;
        }
        return;
    }

    if (IsKeyPressed(KEY_ENTER) || (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && level_from_mouse != -1))
    {
        audio_play_menu_item_switch_sound();
        menu_reset_state();
        game_get_instance()->state = GAME_STATE_PLAYING;
        game_get_instance()->current_level_number = current_level_number;
        game_restart_current_level();
    }
}

int level_from_mouse_input(void)
{
    int width = GetScreenWidth();

    int tile_size = 64;
    int tile_size_with_padding = tile_size + 8 + 8;
    int per_row = width / tile_size_with_padding;
    int y_offset = 220;
    int max_levels = config_load()->max_levels;

    if (current_level_number >= 50)
    {
        y_offset -= ((current_level_number - 50) / 10 + 1) * tile_size_with_padding;
    }

    Vector2 cursor_position = GetMousePosition();

    for (int i = 0; i < max_levels; i++)
    {
        int x = (i % per_row) * tile_size_with_padding;
        int y = (i / per_row) * tile_size_with_padding;

        if (cursor_position.x >= 8 + x && cursor_position.x <= 8 + x + tile_size && cursor_position.y >= y_offset + y && cursor_position.y <= y_offset + y + tile_size)
        {
            return i;
        }
    }

    return -1;
}