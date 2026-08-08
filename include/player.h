#ifndef PLAYER_H
#define PLAYER_H

#include "sprite.h"
#include "map.h"

void player_interact( sprite_t * a, sprite_t * b );
void player_update( map_t * map, sprite_t * sprite, camera_t * camera );

#endif // PLAYER_H
