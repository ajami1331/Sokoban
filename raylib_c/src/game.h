#ifndef GAME_H
#define GAME_H 1

#include "level.h"

enum game_state {
    GAME_STATE_MENU,
    GAME_STATE_PLAYING,
    GAME_STATE_PAUSED,
    GAME_STATE_LEVEL_COMPLETE,
    GAME_STATE_GAME_OVER,
};

typedef struct {
    enum game_state state;
    int current_level_number;
    level *current_level;
    int player_x;
    int player_y;
    int current_score;
    char *level_text;
} game;

void game_init(void);

void game_loop(void);

#endif
