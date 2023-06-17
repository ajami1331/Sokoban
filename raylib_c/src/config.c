#include "config.h"
#include "tomlc99/toml.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CONFIG_FILE "./assets/config.toml"

void config_free(void);
void config_parse_window_table(config *instance, toml_table_t *conf);
void config_parse_game_table(config *instance, toml_table_t *conf);

config *config_load(void)
{
    static config *instance = NULL;

    if (instance)
    {
        return instance;
    }

    instance = malloc(sizeof(config));

    if (!instance)
    {
        TraceLog(LOG_ERROR, "cannot allocate memory for config");
        exit(-1);
    }

    atexit(config_free);

    FILE *fp;
    char errbuf[200];

    TraceLog(LOG_INFO, "loading config from %s", CONFIG_FILE);
    fp = fopen(CONFIG_FILE, "r");
    if (!fp)
    {
        TraceLog(LOG_ERROR, "cannot open config.toml - %s", strerror(errno));
        exit(errno);
    }

    toml_table_t *conf = toml_parse_file(fp, errbuf, sizeof(errbuf));
    fclose(fp);

    if (!conf)
    {
        TraceLog(LOG_ERROR, "cannot parse - %s", errbuf);
        exit(-1);
    }

    config_parse_window_table(instance, conf);

    config_parse_game_table(instance, conf);

    toml_free(conf);

    TraceLog(LOG_INFO, "config loaded");


    return instance;
}

void config_free(void)
{
    TraceLog(LOG_INFO, "freeing config");
    config *instance = config_load();
    if (!instance)
    {
        return;
    }
    free(instance->levels_file);
    free(instance);
    TraceLog(LOG_INFO, "config freed");
}

void config_parse_window_table(config *instance, toml_table_t *conf)
{
    toml_table_t *window = toml_table_in(conf, "window");
    if (!window)
    {
        TraceLog(LOG_ERROR, "missing [window]");
        exit(-1);
    }

    toml_datum_t width = toml_int_in(window, "width");
    if (!width.ok)
    {
        TraceLog(LOG_ERROR, "missing width");
        exit(-1);
    }

    toml_datum_t height = toml_int_in(window, "height");
    if (!height.ok)
    {
        TraceLog(LOG_ERROR, "missing height");
        exit(-1);
    }

    instance->screen_width = width.u.i;
    instance->screen_height = height.u.i;
}

void config_parse_game_table(config *instance, toml_table_t *conf)
{
    toml_table_t *game = toml_table_in(conf, "game");
    if (!game)
    {
        TraceLog(LOG_ERROR, "missing [game]");
        exit(-1);
    }

    toml_datum_t max_levels = toml_int_in(game, "max_levels");
    if (!max_levels.ok)
    {
        TraceLog(LOG_ERROR, "missing max_levels");
        exit(-1);
    }

    toml_datum_t levels_file = toml_string_in(game, "levels_file");
    if (!levels_file.ok)
    {
        TraceLog(LOG_ERROR, "missing levels_file");
        exit(-1);
    }

    instance->max_levels = max_levels.u.i;
    instance->levels_file = strdup(levels_file.u.s);

    free(levels_file.u.s);
}
