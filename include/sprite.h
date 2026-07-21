#ifndef SPRITE_H
#define SPRITE_H

#include "engine.h"
#include <stdint.h>
#include "tile.h"

#define SPRITE_TYPE_PLAYER 0
#define SPRITE_TYPE_APPLE  1
#define SPRITE_TYPE_POLLO  2

#define SPRITE_DIRX_LEFT  0
#define SPRITE_DIRX_RIGHT 1

typedef struct sprite_t
{
    float w;
    float h;
    float x;
    float y;
	float accx;
	float vx;
	float accy;
	float vy;
	float maxgravity;
	float startgravity;
	float startspeed;
	float maxspeed;
	float startjump;
	float jumpacc;
	float maxjump;
	float bounce;
	float friction;
	unsigned int onground : 1;
	unsigned int isjumping : 1;
    unsigned int isunderwater : 1;
	unsigned int dirx : 1;
	unsigned int isdead : 1;
	unsigned int collided_left_solid : 1;
	unsigned int collided_right_solid : 1;
	unsigned int collided_left_slope : 1;
	unsigned int collided_right_slope : 1;
    uint8_t type;
	struct
	{
		float lpadding;
		float rpadding;
		float tpadding;
		float bpadding;
	}
	hitbox;
	struct
	{
		graphic_id_t rect;
		graphic_id_t lcollision;
		graphic_id_t rcollision;
		graphic_id_t bcollision;
		graphic_id_t tcollision;
		graphic_id_t slopepoint;
	}
	graphics;
	union
	{
		struct
		{
			float startspeed;
			float maxspeed;
			unsigned int isducking : 1;
		}
		player;
	}
	specific;
} sprite_t;

sprite_t sprite_create( float x, float y, uint8_t type );
void sprite_update( tile_t * map, sprite_t * sprite );

#endif // SPRITE_H
