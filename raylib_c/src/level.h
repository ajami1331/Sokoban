#ifndef LEVEL_H
#define LEVEL_H 1

#define MAX_LEVEL_WIDTH 32
#define MAX_LEVEL_HEIGHT 32

typedef struct level
{
    int width;
    int height;
    char (*data)[MAX_LEVEL_WIDTH];
} level;

level *level_load(int level_number);

level *level_load_all(void);

#endif
