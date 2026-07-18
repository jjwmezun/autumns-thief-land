#ifndef PLAYER_H
#define PLAYER_H

#include "engine.h"
#include "tile.h"

#define PLAYER_STATE_NORMAL      0
#define PLAYER_STATE_SLIDING     1
#define PLAYER_STATE_SLIDING_END 2
#define PLAYER_STATE_CLIMBING    3

typedef struct player_t {
	float w;
	float h;
	float x;
	float y;
	float vx;
	float vy;
	float walkmaxspeedx;
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
	unsigned int isswimming : 1;
	unsigned int onground : 1;
	unsigned int jumplock : 1;
	unsigned int isducking : 1;
	unsigned int prevslidingdir : 2;
	unsigned int state : 2;
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
		graphic_id_t ytm_hitpoint;
	}
	graphics;
} player_t;

player_t create_player( float x, float y );
void update_player( tile_t * map, player_t * player );

#endif // PLAYER_H