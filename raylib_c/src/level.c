#include "level.h"
#include "config.h"
#include "raylib.h"
#include "utils.h"
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

pthread_mutex_t level_lock = PTHREAD_MUTEX_INITIALIZER;

int LEVELS_LOADED = 0;

void level_free_all(void);

level *level_load(int level_number)
{
    if (level_number >= LEVELS_LOADED || level_number < 0)
    {
        TraceLog(LOG_WARNING, "level number %d is out of range", level_number);
        return NULL;
    }

    level *levels = level_load_all();
    level *l = &levels[level_number];
    return l;
}

level *level_load_all(void)
{
    static level *levels = NULL;

    if (levels)
    {
        return levels;
    }

    pthread_mutex_lock(&level_lock);

    TraceLog(LOG_INFO, "loading levels from %s", config_load()->levels_file);

    levels = (level *)malloc(sizeof(level) * config_load()->max_levels);
    if (!levels)
    {
        TraceLog(LOG_ERROR, "cannot allocate memory for levels");
        exit(-1);
    }

    atexit(level_free_all);

    FILE *fp = fopen(config_load()->levels_file, "r");

    if (!fp)
    {
        TraceLog(LOG_ERROR, "cannot open levels file - %s", strerror(errno));
        exit(errno);
    }

    TraceLog(LOG_DEBUG, "Starting reading from %s", config_load()->levels_file);

    char *line = NULL;
    line = malloc(sizeof(char) * MAX_LEVEL_WIDTH);
    size_t len = 0;
    LEVELS_LOADED = 0;
    while (fgets(line, MAX_LEVEL_WIDTH, fp) != NULL)
    {
        TraceLog(LOG_DEBUG, "Reading level %d", LEVELS_LOADED);
        len = strlen(line);
        levels[LEVELS_LOADED].data = (char(*)[MAX_LEVEL_WIDTH])malloc(sizeof(char[MAX_LEVEL_HEIGHT][MAX_LEVEL_WIDTH]));
        levels[LEVELS_LOADED].height = 0;
        levels[LEVELS_LOADED].width = 0;
        while (fgets(line, MAX_LEVEL_WIDTH, fp) != NULL && line[0] != '~')
        {
            len = strlen(line);
            strncpy(levels[LEVELS_LOADED].data[levels[LEVELS_LOADED].height], line, MAX_LEVEL_WIDTH);
            levels[LEVELS_LOADED].width = utils_max(levels[LEVELS_LOADED].width, len);
            levels[LEVELS_LOADED].height++;
        }
        TraceLog(LOG_DEBUG, "Finished reading level %d", LEVELS_LOADED);
        LEVELS_LOADED++;
    }

    TraceLog(LOG_DEBUG, "Finished reading from %s", config_load()->levels_file);

    free(line);
    fclose(fp);

    TraceLog(LOG_INFO, "loaded %d levels", LEVELS_LOADED);

    pthread_mutex_unlock(&level_lock);
    return levels;
}

void level_free_all(void)
{
    TraceLog(LOG_INFO, "freeing levels");
    level *levels = level_load_all();
    for (int i = 0; i < LEVELS_LOADED; i++)
    {
        free(levels[i].data);
    }
    free(levels);
    TraceLog(LOG_INFO, "levels freed");
}
