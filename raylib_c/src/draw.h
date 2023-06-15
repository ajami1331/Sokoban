#ifndef DRAW_H
#define DRAW_H 1

#include "raylib.h"

void draw_init(void);
void draw_free(void);
void draw_tile(char tile, int pos_x, int pos_y);
void draw_texture_tiled(Texture2D texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation, float scale, Color tint);

#endif
