#include "draw.h"
#include "utils.h"
#include "tile.h"

Texture2D tile_sheet;

void draw_init(void)
{
    tile_sheet = LoadTexture("assets/sokoban_tilesheet.png");    
}

void draw_free(void)
{
    UnloadTexture(tile_sheet);
}

void draw_tile(char tile, int pos_x, int pos_y)
{
    switch (tile)
    {
        case WALL:
            draw_texture_tiled(tile_sheet, utils_get_tile_rect(6, 6), utils_get_draw_rect(pos_x, pos_y), (Vector2){0, 0}, 0,
                                .5, WHITE);
            break;
        case BOX:
            draw_texture_tiled(tile_sheet, utils_get_tile_rect(4, 0), utils_get_draw_rect(pos_x, pos_y), (Vector2){0, 0}, 0,
                                .5, WHITE);
            break;
        case BOX_ON_GOAL:
            draw_texture_tiled(tile_sheet, utils_get_tile_rect(4, 3), utils_get_draw_rect(pos_x, pos_y), (Vector2){0, 0}, 0,
                                .5, WHITE);
            draw_texture_tiled(tile_sheet, utils_get_tile_rect(4, 1), utils_get_draw_rect(pos_x, pos_y), (Vector2){0, 0}, 0,
                                .5, WHITE);
            break;
        case GOAL:
            draw_texture_tiled(tile_sheet, utils_get_tile_rect(4, 3), utils_get_draw_rect(pos_x, pos_y), (Vector2){0, 0}, 0,
                                .5, WHITE);
            break;
        case PLAYER:
            draw_texture_tiled(tile_sheet, utils_get_tile_rect(1, 4), utils_get_draw_rect(pos_x, pos_y), (Vector2){0, 0}, 0,
                                .5, WHITE);
            break;
        case PLAYER_ON_GOAL:
            draw_texture_tiled(tile_sheet, utils_get_tile_rect(4, 3), utils_get_draw_rect(pos_x, pos_y), (Vector2){0, 0}, 0,
                                .5, WHITE);
            draw_texture_tiled(tile_sheet, utils_get_tile_rect(1, 4), utils_get_draw_rect(pos_x, pos_y), (Vector2){0, 0}, 0,
                                .5, WHITE);
            break;
        default:
            break;
    }
}

void draw_texture_tiled(Texture2D texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation, float scale, Color tint)
{
    if ((texture.id <= 0) || (scale <= 0.0f)) return;  // Wanna see a infinite loop?!...just delete this line!
    if ((source.width == 0) || (source.height == 0)) return;

    int tileWidth = (int)(source.width*scale), tileHeight = (int)(source.height*scale);
    if ((dest.width < tileWidth) && (dest.height < tileHeight))
    {
        // Can fit only one tile
        DrawTexturePro(texture, (Rectangle){source.x, source.y, ((float)dest.width/tileWidth)*source.width, ((float)dest.height/tileHeight)*source.height},
                    (Rectangle){dest.x, dest.y, dest.width, dest.height}, origin, rotation, tint);
    }
    else if (dest.width <= tileWidth)
    {
        // Tiled vertically (one column)
        int dy = 0;
        for (;dy+tileHeight < dest.height; dy += tileHeight)
        {
            DrawTexturePro(texture, (Rectangle){source.x, source.y, ((float)dest.width/tileWidth)*source.width, source.height}, (Rectangle){dest.x, dest.y + dy, dest.width, (float)tileHeight}, origin, rotation, tint);
        }

        // Fit last tile
        if (dy < dest.height)
        {
            DrawTexturePro(texture, (Rectangle){source.x, source.y, ((float)dest.width/tileWidth)*source.width, ((float)(dest.height - dy)/tileHeight)*source.height},
                        (Rectangle){dest.x, dest.y + dy, dest.width, dest.height - dy}, origin, rotation, tint);
        }
    }
    else if (dest.height <= tileHeight)
    {
        // Tiled horizontally (one row)
        int dx = 0;
        for (;dx+tileWidth < dest.width; dx += tileWidth)
        {
            DrawTexturePro(texture, (Rectangle){source.x, source.y, source.width, ((float)dest.height/tileHeight)*source.height}, (Rectangle){dest.x + dx, dest.y, (float)tileWidth, dest.height}, origin, rotation, tint);
        }

        // Fit last tile
        if (dx < dest.width)
        {
            DrawTexturePro(texture, (Rectangle){source.x, source.y, ((float)(dest.width - dx)/tileWidth)*source.width, ((float)dest.height/tileHeight)*source.height},
                        (Rectangle){dest.x + dx, dest.y, dest.width - dx, dest.height}, origin, rotation, tint);
        }
    }
    else
    {
        // Tiled both horizontally and vertically (rows and columns)
        int dx = 0;
        for (;dx+tileWidth < dest.width; dx += tileWidth)
        {
            int dy = 0;
            for (;dy+tileHeight < dest.height; dy += tileHeight)
            {
                DrawTexturePro(texture, source, (Rectangle){dest.x + dx, dest.y + dy, (float)tileWidth, (float)tileHeight}, origin, rotation, tint);
            }

            if (dy < dest.height)
            {
                DrawTexturePro(texture, (Rectangle){source.x, source.y, source.width, ((float)(dest.height - dy)/tileHeight)*source.height},
                    (Rectangle){dest.x + dx, dest.y + dy, (float)tileWidth, dest.height - dy}, origin, rotation, tint);
            }
        }

        // Fit last column of tiles
        if (dx < dest.width)
        {
            int dy = 0;
            for (;dy+tileHeight < dest.height; dy += tileHeight)
            {
                DrawTexturePro(texture, (Rectangle){source.x, source.y, ((float)(dest.width - dx)/tileWidth)*source.width, source.height},
                        (Rectangle){dest.x + dx, dest.y + dy, dest.width - dx, (float)tileHeight}, origin, rotation, tint);
            }

            // Draw final tile in the bottom right corner
            if (dy < dest.height)
            {
                DrawTexturePro(texture, (Rectangle){source.x, source.y, ((float)(dest.width - dx)/tileWidth)*source.width, ((float)(dest.height - dy)/tileHeight)*source.height},
                    (Rectangle){dest.x + dx, dest.y + dy, dest.width - dx, dest.height - dy}, origin, rotation, tint);
            }
        }
    }
}
