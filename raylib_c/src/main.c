#include "game.h"
#include "raylib.h"

int main(void)
{
    SetTraceLogLevel(LOG_LEVEL);

    game_init();

    game_loop();

    return 0;
}
