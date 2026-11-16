#ifndef MAP_H
#define MAP_H

#include "tile.h"
#include <stdint.h>

typedef struct map_t {
    tile_t * tiles;
    uint_fast16_t width;
    uint_fast16_t height;
    unsigned int palette;
} map_t;

map_t create_map();
void map_clear_tile( map_t * map, unsigned int x, unsigned int y );
tile_t map_get_tile( const map_t * map, unsigned int x, unsigned int y );

#endif // MAP_H