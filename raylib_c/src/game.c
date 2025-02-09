#include "audio.h"
#include "game.h"
#include "draw.h"
#include "config.h"
#include "menu.h"
#include "level.h"
#include "tile.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(PLATFORM_WEB)
#include <emscripten.h>
#define PAUSE_MENU_ITEMS_COUNT 3
#else
#define PAUSE_MENU_ITEMS_COUNT 4
#endif

void game_cleanup(void);
void game_update_draw_frame(void);
void game_draw_current_level(void);
void game_load_current_level(void);
void game_update_current_level(void);
void game_check_for_level_complete(void);
void game_draw_level_complete_message(void);
void game_check_input_for_level_complete(void);
void game_draw_pause_menu(void);
void game_check_input_for_pause_menu(void);
void game_draw_and_check_menu_button(void);

int current_pause_menu_item = 0;
int current_gesture = -1;
int last_gesture = -1;

char *pause_menu_items[PAUSE_MENU_ITEMS_COUNT] = {
    "Resume",
    "Restart",
    "Exit to main menu",
    "Exit to desktop",
};

void game_init(void)
{
    TraceLog(LOG_INFO, "initializing game");

    level_load_all();

    InitWindow(config_load()->screen_width, config_load()->screen_height, "Sokoban");

    audio_init();

    SetTargetFPS(60);

    SetExitKey(0);

    draw_init();

    atexit(game_cleanup);

    game_get_instance()->state = GAME_STATE_MENU;
    game_get_instance()->current_level_number = 0;
    game_get_instance()->current_level = NULL;
    game_load_current_level();

    TraceLog(LOG_INFO, "game initialized");
}

void game_loop(void)
{
    TraceLog(LOG_INFO, "starting game loop");
#if defined(PLATFORM_WEB)
    TraceLog(LOG_INFO, "starting web main loop");
    emscripten_set_main_loop(game_update_draw_frame, 0, 1);
#else
    TraceLog(LOG_INFO, "starting non-web main loop");
    while (!WindowShouldClose())
    {
        game_update_draw_frame();
    }
#endif
    TraceLog(LOG_INFO, "game loop ended");
}

void game_load_current_level(void)
{
    level *l = level_load(game_get_instance()->current_level_number);
    game *g = game_get_instance();
    if (!l)
    {
        TraceLog(LOG_WARNING, "cannot load level %d", game_get_instance()->current_level);
        return;
    }

    if (g->current_level)
    {
        free(g->current_level->data);
        free(g->current_level);
        g->current_level = NULL;
    }

    g->current_level = (level *)malloc(sizeof(level));
    if (!g->current_level)
    {
        TraceLog(LOG_ERROR, "cannot allocate memory for current level");
        exit(-1);
    }

    memcpy(g->current_level, l, sizeof(level));
    g->current_level->data = (char(*)[MAX_LEVEL_WIDTH])malloc(sizeof(char[MAX_LEVEL_WIDTH]) * g->current_level->height);
    if (!g->current_level->data)
    {
        TraceLog(LOG_ERROR, "cannot allocate memory for current level data");
        exit(-1);
    }

    memcpy(g->current_level->data, l->data, sizeof(char[MAX_LEVEL_WIDTH]) * g->current_level->height);

    for (int i = 0; i < l->height; i++)
    {
        for (int j = 0; j < l->width; j++)
        {
            if (l->data[i][j] == PLAYER)
            {
                game_get_instance()->player_x = i;
                game_get_instance()->player_y = j;
                break;
            }
        }
    }
}

void game_draw_current_level(void)
{
    game *g = game_get_instance();

    snprintf(g->level_text, 32, "Level: %d", g->current_level_number + 1);

    DrawText("Sokoban", GetScreenWidth() - 100, 10, 20, WHITE);
    DrawText(g->level_text, GetScreenWidth() - 100, 30, 20, WHITE);

    level *l = g->current_level;

    int y_Offset = (GetScreenHeight() - (l->height * 64) / 2) / 64;
    int x_Offset = (GetScreenWidth() - (l->width * 64) / 2) / 64;

    for (int i = 0; i < l->height; i++)
    {
        for (int j = 0; j < l->width; j++)
        {
            int drax_x = j + x_Offset;
            int draw_y = i + y_Offset;
            draw_tile(l->data[i][j], drax_x, draw_y);
        }
    }
}

game *game_get_instance(void)
{
    static game *g = NULL;

    if (!g)
    {
        g = (game *)malloc(sizeof(game));
        if (!g)
        {
            TraceLog(LOG_ERROR, "cannot allocate memory for game");
            exit(-1);
        }

        g->level_text = (char *)malloc(32);
        if (!g->level_text)
        {
            TraceLog(LOG_ERROR, "cannot allocate memory for level text");
            exit(-1);
        }
    }

    return g;
}

void game_update_draw_frame(void)
{
    BeginDrawing();

    ClearBackground(SKYBLUE);

    switch (game_get_instance()->state)
    {
    case GAME_STATE_MENU:
        menu_process_main_menu();
        break;
    case GAME_STATE_PLAYING:
        game_update_current_level();
        game_draw_current_level();
        game_draw_and_check_menu_button();
        game_check_for_level_complete();
        break;
    case GAME_STATE_LEVEL_COMPLETE:
        game_draw_current_level();
        game_draw_level_complete_message();
        game_check_input_for_level_complete();
        break;
    case GAME_STATE_PAUSED:
        game_draw_current_level();
        game_draw_pause_menu();
        game_check_input_for_pause_menu();
        break;
    default:
        break;
    }

    EndDrawing();
}

void game_cleanup(void)
{
    TraceLog(LOG_INFO, "cleaning up game");

    game *g = game_get_instance();

    if (g->current_level)
    {
        free(g->current_level->data);
        free(g->current_level);
        g->current_level = NULL;
    }

    free(g->level_text);

    free(g);

    draw_free();

    audio_destroy();

    CloseWindow();

    TraceLog(LOG_INFO, "game cleaned up");
}

void game_update_current_level(void)
{
    game *g = game_get_instance();
    level *l = g->current_level;
    int prev_player_x = g->player_x;
    int prev_player_y = g->player_y;
    current_gesture = GetGestureDetected();
    if (current_gesture != last_gesture)
    {
        Vector2 touchPosition = GetTouchPosition(0);
        if (current_gesture == GESTURE_TAP)
        {
            if (touchPosition.x > GetScreenWidth() / 2 + 100)
            {
                g->player_y++;
            }
            else if (touchPosition.x < GetScreenWidth() / 2 - 100)
            {
                g->player_y--;
            }
            else if (touchPosition.y < GetScreenHeight() / 2)
            {
                g->player_x--;
            }
            else
            {
                g->player_x++;
            }
        }
        last_gesture = current_gesture;
    }

    if (IsKeyPressed(KEY_ESCAPE))
    {
        g->state = GAME_STATE_PAUSED;
        return;
    }
    else if (IsKeyPressed(KEY_R))
    {
        game_restart_current_level();
        return;
    }

    if (IsKeyPressed(KEY_RIGHT))
    {
        g->player_y++;
    }
    else if (IsKeyPressed(KEY_LEFT))
    {
        g->player_y--;
    }
    else if (IsKeyPressed(KEY_UP))
    {
        g->player_x--;
    }
    else if (IsKeyPressed(KEY_DOWN))
    {
        g->player_x++;
    }

    bool clear_prev = false;

    if (l->data[g->player_x][g->player_y] == WALL)
    {
        g->player_x = prev_player_x;
        g->player_y = prev_player_y;
    }

    int dir_x = g->player_x - prev_player_x;
    int dir_y = g->player_y - prev_player_y;

    if (l->data[g->player_x][g->player_y] == BOX)
    {
        if (l->data[g->player_x + dir_x][g->player_y + dir_y] == FLOOR)
        {
            l->data[g->player_x + dir_x][g->player_y + dir_y] = BOX;
            l->data[g->player_x][g->player_y] = PLAYER;
            clear_prev = true;
        }
        else if (l->data[g->player_x + dir_x][g->player_y + dir_y] == GOAL)
        {
            l->data[g->player_x + dir_x][g->player_y + dir_y] = BOX_ON_GOAL;
            l->data[g->player_x][g->player_y] = PLAYER;
            clear_prev = true;
        }
        else
        {
            g->player_x = prev_player_x;
            g->player_y = prev_player_y;
        }
    }

    if (l->data[g->player_x][g->player_y] == BOX_ON_GOAL)
    {
        if (l->data[g->player_x + dir_x][g->player_y + dir_y] == FLOOR)
        {
            l->data[g->player_x + dir_x][g->player_y + dir_y] = BOX;
            l->data[g->player_x][g->player_y] = PLAYER_ON_GOAL;
            clear_prev = true;
        }
        else if (l->data[g->player_x + dir_x][g->player_y + dir_y] == GOAL)
        {
            l->data[g->player_x + dir_x][g->player_y + dir_y] = BOX_ON_GOAL;
            l->data[g->player_x][g->player_y] = PLAYER_ON_GOAL;
            clear_prev = true;
        }
        else
        {
            g->player_x = prev_player_x;
            g->player_y = prev_player_y;
        }
    }

    if (l->data[g->player_x][g->player_y] == GOAL)
    {
        l->data[g->player_x][g->player_y] = PLAYER_ON_GOAL;
        clear_prev = true;
    }

    if (l->data[g->player_x][g->player_y] == FLOOR)
    {
        l->data[g->player_x][g->player_y] = PLAYER;
        clear_prev = true;
    }

    if (clear_prev)
    {
        if (l->data[prev_player_x][prev_player_y] == PLAYER_ON_GOAL)
        {
            l->data[prev_player_x][prev_player_y] = GOAL;
        }
        else
        {
            l->data[prev_player_x][prev_player_y] = FLOOR;
        }
    }
}

void game_check_for_level_complete(void)
{
    game *g = game_get_instance();
    level *l = g->current_level;

    for (int i = 0; i < l->height; i++)
    {
        for (int j = 0; j < l->width; j++)
        {
            if (l->data[i][j] == BOX)
            {
                return;
            }
        }
    }

    g->state = GAME_STATE_LEVEL_COMPLETE;
}

void game_draw_level_complete_message(void)
{
    DrawText("Level Complete", 20, 240, 55, WHITE);
    DrawText("Press n to go to next level", 20, 350, 45, WHITE);
    DrawText("Press p to go to previous level", 20, 450, 45, WHITE);
    DrawText("Press r to restart current one", 20, 550, 45, WHITE);
}

void game_check_input_for_level_complete(void)
{
    game *g = game_get_instance();

    if (IsKeyPressed(KEY_P))
    {
        g->current_level_number = utils_max(0, g->current_level_number - 1);
        game_load_current_level();
        g->state = GAME_STATE_PLAYING;
    }
    else if (IsKeyPressed(KEY_N))
    {
        g->current_level_number = utils_min(config_load()->max_levels - 1, g->current_level_number + 1);
        game_load_current_level();
        g->state = GAME_STATE_PLAYING;
    }
    else if (IsKeyPressed(KEY_R))
    {
        game_load_current_level();
        g->state = GAME_STATE_PLAYING;
    }
}

void game_draw_pause_menu(void)
{
    int width = GetScreenWidth();

    ClearBackground(SKYBLUE);

    DrawText("Sokoban", width / 3.5, 150, 76, WHITE);

    for (int i = 0; i < PAUSE_MENU_ITEMS_COUNT; i++)
    {
        if (i == current_pause_menu_item)
        {
            DrawText(pause_menu_items[i], width / 2.5, 250 + (i * 50), 48, ORANGE);
            continue;
        }
        DrawText(pause_menu_items[i], width / 2.5, 250 + (i * 50), 48, WHITE);
    }
}

void game_check_input_for_pause_menu(void)
{

    if (IsKeyPressed(KEY_ESCAPE))
    {
        game *g = game_get_instance();
        g->state = GAME_STATE_PLAYING;
    }

    Vector2 cursor_position = GetMousePosition();

    for (int i = 0; i < PAUSE_MENU_ITEMS_COUNT; i++)
    {
        if (cursor_position.y >= 250 + (i * 50) && cursor_position.y <= 250 + (i * 50) + 50 && cursor_position.x >= GetScreenWidth() / 2.5 && cursor_position.x <= GetScreenWidth() / 2.5 + 200)
        {
            if (current_pause_menu_item == i)
            {
                break;
            }
            audio_play_menu_item_switch_sound();
            current_pause_menu_item = i;
        }
    }

    if (IsKeyPressed(KEY_UP))
    {
        audio_play_menu_item_switch_sound();
        if (current_pause_menu_item == 0)
        {
            current_pause_menu_item = PAUSE_MENU_ITEMS_COUNT - 1;
            return;
        }
        current_pause_menu_item--;
        return;
    }

    if (IsKeyPressed(KEY_DOWN))
    {
        audio_play_menu_item_switch_sound();
        if (current_pause_menu_item == PAUSE_MENU_ITEMS_COUNT - 1)
        {
            current_pause_menu_item = 0;
            return;
        }
        current_pause_menu_item++;
        return;
    }

    if (IsKeyPressed(KEY_ENTER) ||
        (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && cursor_position.x >= GetScreenWidth() / 2.5 && cursor_position.x <= GetScreenWidth() / 2.5 + 200))
    {
        audio_play_menu_item_switch_sound();
        switch (current_pause_menu_item)
        {
        case 0:
        {
            game_get_instance()->state = GAME_STATE_PLAYING;
            current_pause_menu_item = 0;
            break;
        }
        case 1:
        {
            game_restart_current_level();
            game_get_instance()->state = GAME_STATE_PLAYING;
            current_pause_menu_item = 0;
            break;
        }
        case 2:
        {
            game_get_instance()->state = GAME_STATE_MENU;
            current_pause_menu_item = 0;
            break;
        }
        case 3:
        {
            exit(0);
            break;
        }
        default:
            break;
        }
    }
}

void game_restart_current_level(void)
{
    game_load_current_level();
    TraceLog(LOG_INFO, "reloading level %d", game_get_instance()->current_level_number);
}

void game_draw_and_check_menu_button(void)
{
    Vector2 cursor_position = GetMousePosition();

    if (cursor_position.x >= 10 && cursor_position.x <= 10 + 50 && cursor_position.y >= 10 && cursor_position.y <= 10 + 50)
    {
        DrawText("Menu", 10, 10, 20, ORANGE);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            game_get_instance()->state = GAME_STATE_PAUSED;
        }
    }
    else
    {
        DrawText("Menu", 10, 10, 20, WHITE);
    }
}