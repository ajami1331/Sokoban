#include "game.h"
#include "common.h"

#ifdef DEBBUG_MODE
    #define STB_LEAKCHECK_IMPLEMENTATION
    #define STB_LEAKCHECK_SHOWALL
    #include "stb_leakcheck.h"
#endif

int main(void)
{
#ifdef DEBBUG_MODE
    atexit(stb_leakcheck_dumpmem);
#endif
    SetTraceLogLevel(LOG_LEVEL);

    game_init();

    game_loop();

    return 0;
}
