#ifndef CONFIG_H
#define CONFIG_H 1

#include "common.h"

typedef struct config
{
    int screen_width;
    int screen_height;

    int max_levels;
    char *levels_file;
} config;

config *config_load(void);

#endif
