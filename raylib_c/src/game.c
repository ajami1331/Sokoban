#include "game.h"
#include "config.h"
#include "level.h"
#include "raylib.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(PLATFORM_WEB)
#include <emscripten.h>
#endif

void game_cleanup(void);
void game_update_draw_frame(void);
game *game_get_instance(void);
void game_draw_current_level(void);
void game_load_current_level(void);
void game_update_current_level(void);
void game_check_for_level_complete(void);
void game_draw_level_complete_message(void);
void game_check_input_for_level_complete(void);

Texture2D tile_sheet;

void game_init(void)
{
    TraceLog(LOG_INFO, "initializing game");

    config *conf = config_load();

    level_load_all();

    InitWindow(conf->screen_width, conf->screen_height, "Sokoban");

    SetTargetFPS(60);

    tile_sheet = LoadTexture("assets/sokoban_tilesheet.png");

    atexit(game_cleanup);

    game_get_instance()->state = GAME_STATE_PLAYING;
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
            if (l->data[i][j] == '@')
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

    DrawText("Sokoban", config_load()->screen_width - 100, 10, 20, WHITE);
    DrawText(g->level_text, config_load()->screen_width - 100, 30, 20, WHITE);

    level *l = g->current_level;

    int y_Offset = (config_load()->screen_height - (l->height * 64) / 2) / 64;
    int x_Offset = (config_load()->screen_width - (l->width * 64) / 2)/ 64;

    for (int i = 0; i < l->height; i++)
    {
        for (int j = 0; j < l->width; j++)
        {
            int drax_x = j + x_Offset;
            int draw_y = i + y_Offset;
            switch (l->data[i][j])
            {
            case '#':
                DrawTextureTiled(tile_sheet, utils_get_tile_rect(6, 6), utils_get_draw_rect(drax_x, draw_y), (Vector2){0, 0}, 0,
                                 .5, WHITE);
                break;
            case '$':
                DrawTextureTiled(tile_sheet, utils_get_tile_rect(4, 0), utils_get_draw_rect(drax_x, draw_y), (Vector2){0, 0}, 0,
                                 .5, WHITE);
                break;
            case '*':
                DrawTextureTiled(tile_sheet, utils_get_tile_rect(4, 3), utils_get_draw_rect(drax_x, draw_y), (Vector2){0, 0}, 0,
                                 .5, WHITE);
                DrawTextureTiled(tile_sheet, utils_get_tile_rect(4, 1), utils_get_draw_rect(drax_x, draw_y), (Vector2){0, 0}, 0,
                                 .5, WHITE);
                break;
            case '.':
                DrawTextureTiled(tile_sheet, utils_get_tile_rect(4, 3), utils_get_draw_rect(drax_x, draw_y), (Vector2){0, 0}, 0,
                                 .5, WHITE);
                break;
            case '@':
                DrawTextureTiled(tile_sheet, utils_get_tile_rect(1, 4), utils_get_draw_rect(drax_x, draw_y), (Vector2){0, 0}, 0,
                                 .5, WHITE);
                break;
            case '+':
                DrawTextureTiled(tile_sheet, utils_get_tile_rect(4, 3), utils_get_draw_rect(drax_x, draw_y), (Vector2){0, 0}, 0,
                                 .5, WHITE);
                DrawTextureTiled(tile_sheet, utils_get_tile_rect(1, 4), utils_get_draw_rect(drax_x, draw_y), (Vector2){0, 0}, 0,
                                 .5, WHITE);
                break;
            default:
                break;
            }
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
    }

    g->level_text = (char *)malloc(32);
    return g;
}

void game_update_draw_frame(void)
{
    BeginDrawing();

    ClearBackground(SKYBLUE);

    switch (game_get_instance()->state)
    {
    case GAME_STATE_PLAYING:
        game_update_current_level();
        game_draw_current_level();
        game_check_for_level_complete();
        break;
    case GAME_STATE_LEVEL_COMPLETE:
        game_draw_current_level();
        game_draw_level_complete_message();
        game_check_input_for_level_complete();
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

    UnloadTexture(tile_sheet);

    CloseWindow();

    TraceLog(LOG_INFO, "game cleaned up");
}

void game_update_current_level(void)
{
    game *g = game_get_instance();
    level *l = g->current_level;
    int prev_player_x = g->player_x;
    int prev_player_y = g->player_y;

    if (IsKeyPressed(KEY_R))
    {
        game_load_current_level();
        TraceLog(LOG_INFO, "reloading level %d", g->current_level_number);
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

    if (l->data[g->player_x][g->player_y] == '#')
    {
        g->player_x = prev_player_x;
        g->player_y = prev_player_y;
    }

    int dir_x = g->player_x - prev_player_x;
    int dir_y = g->player_y - prev_player_y;

    if (l->data[g->player_x][g->player_y] == '$')
    {
        if (l->data[g->player_x + dir_x][g->player_y + dir_y] == ' ')
        {
            l->data[g->player_x + dir_x][g->player_y + dir_y] = '$';
            l->data[g->player_x][g->player_y] = '@';
            clear_prev = true;
        }
        else if (l->data[g->player_x + dir_x][g->player_y + dir_y] == '.')
        {
            l->data[g->player_x + dir_x][g->player_y + dir_y] = '*';
            l->data[g->player_x][g->player_y] = '@';
            clear_prev = true;
        }
        else
        {
            g->player_x = prev_player_x;
            g->player_y = prev_player_y;
        }
    }

    if (l->data[g->player_x][g->player_y] == '*')
    {
        if (l->data[g->player_x + dir_x][g->player_y + dir_y] == ' ')
        {
            l->data[g->player_x + dir_x][g->player_y + dir_y] = '$';
            l->data[g->player_x][g->player_y] = '+';
            clear_prev = true;
        }
        else if (l->data[g->player_x + dir_x][g->player_y + dir_y] == '.')
        {
            l->data[g->player_x + dir_x][g->player_y + dir_y] = '*';
            l->data[g->player_x][g->player_y] = '+';
            clear_prev = true;
        }
        else
        {
            g->player_x = prev_player_x;
            g->player_y = prev_player_y;
        }
    }

    if (l->data[g->player_x][g->player_y] == '.')
    {
        l->data[g->player_x][g->player_y] = '+';
        clear_prev = true;
    }

    if (l->data[g->player_x][g->player_y] == ' ')
    {
        l->data[g->player_x][g->player_y] = '@';
        clear_prev = true;
    }

    if (clear_prev)
    {
        if (l->data[prev_player_x][prev_player_y] == '+')
        {
            l->data[prev_player_x][prev_player_y] = '.';
        }
        else
        {
            l->data[prev_player_x][prev_player_y] = ' ';
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
            if (l->data[i][j] == '$')
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
