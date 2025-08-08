#ifndef SPRITE_H
#define SPRITE_H

#include "engine.h"
#include "tile.h"

#define SLIDE_NONE 0
#define SLIDE_DUCK 1
#define SLIDE_LEFT 2
#define SLIDE_RIGHT 3

#define SPRITE_STATE_NORMAL 0
#define SPRITE_STATE_SLIDING 1

typedef struct sprite_t {
	float w;
	float h;
	float x;
	float y;
	float vx;
	float vy;
	float walkmaxspeedx;
	float runvy;
	float accx;
	float accy;
	float startspeedx;
	float startspeedy;
	float maxspeedx;
	float maxspeedy;
	float friction;
	float startgravity;
	float maxgravity;
	float startjump;
	float jumpacc;
	float maxjump;
	float jump_padding;
	float maxslidespeedx;
	float maxslidespeedy;
	unsigned int isjumping : 1;
	unsigned int onground : 1;
	unsigned int jumplock : 1;
	unsigned int slidestate : 2;
	unsigned int prevslidingdir : 2;
	unsigned int state : 1;
	unsigned int dirx : 1;
	struct
	{
		graphic_id_t rect;
		graphic_id_t xtl_hitpoint;
		graphic_id_t xtr_hitpoint;
		graphic_id_t xml_hitpoint;
		graphic_id_t xmr_hitpoint;
		graphic_id_t xbl_hitpoint;
		graphic_id_t xbr_hitpoint;
		graphic_id_t ytl_hitpoint;
		graphic_id_t ytr_hitpoint;
		graphic_id_t ybl_hitpoint;
		graphic_id_t ybr_hitpoint;
		graphic_id_t ybc_hitpoint;
		graphic_id_t ybc2_hitpoint;
	}
	graphics;
} sprite_t;

sprite_t create_player( float x, float y );
void update_player( tile_t * map, sprite_t * player, float dt );

#endif // SPRITE_H