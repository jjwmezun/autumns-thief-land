#ifndef SPRITE_H
#define SPRITE_H

#include "engine.h"
#include <stdint.h>
#include "tile.h"

#define SPRITE_TYPE_PLAYER                 0
#define SPRITE_TYPE_APPLE                  1
#define SPRITE_TYPE_POLLO_STILL            2
#define SPRITE_TYPE_POLLO_SPIN             3
#define SPRITE_TYPE_POLLO_MOVE_HORIZONTAL  4
#define SPRITE_TYPE_POLLO_MOVE_VERTICAL    5
#define SPRITE_TYPE_CRAB                   6
#define SPRITE_TYPE_TRUCK                  7
#define SPRITE_TYPE_BEE_STILL              8
#define SPRITE_TYPE_BEE_SPIN               9
#define SPRITE_TYPE_BEE_MOVE_HORIZONTAL   10
#define SPRITE_TYPE_BEE_MOVE_VERTICAL     11
#define SPRITE_TYPE_HYDRANT               12

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
	unsigned int onground : 1;
	unsigned int isjumping : 1;
    unsigned int isunderwater : 1;
	unsigned int dirx : 1;
	unsigned int isdead : 1;
	unsigned int isairborne : 1;
	unsigned int interacts_with_map : 1;
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
			float origx;
			float origy;
			float angle;
			unsigned int dir : 1;
		}
		bee;
		struct
		{
			float origx;
			float origy;
			float angle;
			unsigned int diry : 1;
		}
		pollo;
		struct
		{
			unsigned int awake : 1;
			float timer;
		}
		hydrant;
	}
	specific;
} sprite_t;

sprite_t sprite_create( float x, float y, uint8_t type );
void sprite_update( tile_t * map, sprite_t * sprite );

#endif // SPRITE_H
