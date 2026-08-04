#ifndef PLAYER_H
#define PLAYER_H

#include "sprite.h"
#include "tile.h"

void player_interact( sprite_t * a, sprite_t * b );
void player_update( tile_t * map, sprite_t * sprite );

#endif // PLAYER_H
