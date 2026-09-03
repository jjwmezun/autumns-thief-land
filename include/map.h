#ifndef MAP_H
#define MAP_H

#include "tile.h"

typedef struct map_t {
    tile_t * tiles;
    int width;
    int height;
} map_t;

map_t create_map();
void add_priority_map_graphics( const map_t * map );
tile_t map_get_tile( const map_t * map, unsigned int x, unsigned int y );

#endif // MAP_H