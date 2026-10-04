#ifndef SPRITE_H
#define SPRITE_H

#include "camera.h"
#include "engine.h"
#include <stdint.h>
#include "map.h"

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

#define SPRITE_PLAYER_STATE_NORMAL      0
#define SPRITE_PLAYER_STATE_SLIDING     1
#define SPRITE_PLAYER_STATE_SLIDING_END 2
#define SPRITE_PLAYER_STATE_CLIMBING    3

#define BOUNDLX( sprite ) ( ( ( sprite )->x ) + ( ( sprite )->hitbox.lpadding ) )
#define BOUNDRX( sprite ) ( ( ( sprite )->x ) + ( ( sprite )->w ) - ( ( sprite )->hitbox.rpadding ) )
#define BOUNDLRY( sprite ) ( ( ( sprite )->y ) - ( ( sprite )->h ) + ( ( sprite )->hitbox.tpadding ) + 3.0f )
#define BOUNDLRH( sprite ) ( ( ( sprite )->h ) - ( ( sprite )->hitbox.tpadding ) - ( ( sprite )->hitbox.bpadding ) - 6.0f )
#define BOUNDTY( sprite ) ( ( ( sprite )->y ) - ( ( sprite )->h ) + ( ( sprite )->hitbox.tpadding ) )
#define BOUNDBY( sprite ) ( ( ( sprite )->y ) - ( ( sprite )->hitbox.bpadding ) )
#define BOUNDTBX( sprite ) ( ( ( sprite )->x ) + ( ( sprite )->hitbox.lpadding ) + 3.0f )
#define BOUNDTBW( sprite ) ( ( ( sprite )->w ) - ( ( sprite )->hitbox.lpadding ) - ( ( sprite )->hitbox.rpadding ) - 6.0f )
#define SLOPEPOINTY( sprite ) ( ( ( sprite )->y ) - ( ( sprite )->hitbox.bpadding ) - 1.0f )
#define SLOPEPOINTX( sprite ) ( ( ( sprite )->x ) + ( ( sprite )->w ) / 2.0f )

typedef struct collision_t {
	tile_t tile;
	uint16_t x;
	uint16_t y;
	unsigned int valid : 1;
}
collision_t;

typedef struct sprite_t
{
	union
	{
		struct
		{
			float startspeed;
			float maxspeed;
			float maxjump;
			float startgravity;
			float maxgravity;
			float jump_padding;
			float maxslidespeedx;
			float maxslidespeedy;
			unsigned int state : 2;
			unsigned int isducking : 1;
			unsigned int jumplock : 1;
			unsigned int prev_underwater : 1;
			unsigned int walktimer : 3;
			unsigned int walkanimation : 2;
		}
		player;
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

	#ifdef DEBUG
		struct
		{
			graphic_id_t rect_t;
			graphic_id_t lcollision;
			graphic_id_t rcollision;
			graphic_id_t bcollision;
			graphic_id_t tcollision;
			graphic_id_t slopepoint;
		}
		graphics;
	#endif

	sprite_id_t graphics;

	struct
	{
		float lpadding;
		float rpadding;
		float tpadding;
		float bpadding;
	}
	hitbox;
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
    uint8_t type;
	unsigned int onground : 1;
	unsigned int isjumping : 1;
    unsigned int isunderwater : 1;
	unsigned int dirx : 1;
	unsigned int isdead : 1;
	unsigned int isairborne : 1;
	unsigned int interacts_with_map : 1;
	unsigned int collided_bottom_solid : 1;
	unsigned int collided_left_solid : 1;
	unsigned int collided_right_solid : 1;
	unsigned int collided_bottom_slope : 1;
	unsigned int collided_left_slope : 1;
	unsigned int collided_right_slope : 1;
	unsigned int collided_bottom_slope_steepness : 2;
	unsigned int collided_bottom_slope_dir : 1;
}
sprite_t;

sprite_t sprite_create( float x, float y, uint8_t type );
void sprite_update( map_t * map, sprite_t * sprite, camera_t * camera );
void sprite_interact( sprite_t * a, sprite_t * b );

void sprite_interact_move_toward( sprite_t * a, sprite_t * b );
unsigned int sprite_interact_test_bop( sprite_t * a, sprite_t * b, float padding );
unsigned int sprite_interact_test_top_collision( sprite_t * a, sprite_t * b, float padding );
void sprite_interact_test_harm( sprite_t * a, sprite_t * b );
void sprite_fall( sprite_t * sprite );
void sprite_falling_and_jumping( const map_t * map, sprite_t * sprite );
void sprite_jump( sprite_t * sprite );
void sprite_jump_when_on_ground( sprite_t * sprite );
void sprite_map_interaction( const map_t * map, sprite_t * sprite );
void sprite_move_in_direction( sprite_t * sprite );
void sprite_move_x( sprite_t * sprite );
unsigned int sprite_slope_physics( const map_t * map, sprite_t * sprite, float ypoint );
collision_t sprite_test_bottom_collision( const map_t * map, sprite_t * sprite, unsigned int ( * test )( tile_t ) );
collision_t sprite_test_horizontal_collision( const map_t * map, sprite_t * sprite, unsigned int ( * test )( tile_t ), int x );
collision_t sprite_test_left_collision( const map_t * map, sprite_t * sprite, unsigned int ( * test )( tile_t ) );
collision_t sprite_test_right_collision( const map_t * map, sprite_t * sprite, unsigned int ( * test )( tile_t ) );
collision_t sprite_test_top_collision( const map_t * map, sprite_t * sprite, unsigned int ( * test )( tile_t ) );
collision_t sprite_test_vertical_collision( const map_t * map, sprite_t * sprite, unsigned int ( * test )( tile_t ), int y );
void sprite_turn_on_collision( sprite_t * sprite );

#endif // SPRITE_H
