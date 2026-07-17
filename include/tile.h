#ifndef TILE_H
#define TILE_H

#include <inttypes.h>

// Tile types.
#define TILE_NORMAL 0
#define TILE_SLOPE  1
#define TILE_MOVE   2
#define TILE_MONEY  3

// Tile subtypes.
#define TILE_EMPTY           0
#define TILE_SOLID           1
#define TILE_SOLID_TOP       2
#define TILE_CLIMB           3
#define TILE_CLIMB_SOLID_TOP 4
#define TILE_WARP			 5
#define TILE_CARD			 6
#define TILE_KEY			 7
#define TILE_DOOR			 8
#define TILE_DOOR_LOCKED	 9
#define TILE_DIAMOND	    10
#define TILE_HEART		    11
#define TILE_CLUB		    12
#define TILE_SPADE		    13
#define TILE_HEALTH		    14
#define TILE_FULL_HEALTH    15

// Slope directions.
#define TILE_LEFT 0
#define TILE_RIGHT 1

// Slope steepness.
#define TILE_FLAT 0
#define TILE_LOW 1
#define TILE_MEDIUM 2
#define TILE_HIGH 3

typedef struct tile_t {
	unsigned int type : 2;
	unsigned int underwater : 1;
	unsigned int hurt : 1;
	union
	{
		struct
		{
			uint8_t subtype;
		} normal;
		struct
		{
			uint32_t data1;
			uint32_t data2;
			uint32_t data3;
		} slope;
		struct
		{
			unsigned int dirx : 1;
			unsigned int speedx : 4;
		} move;
		struct
		{
			uint32_t amount;
		} money;
	} data;
} tile_t;

tile_t create_empty_tile();
tile_t create_sloped_tile
(
	unsigned int dirx, unsigned int steepness, unsigned int isceiling,
	unsigned int l0, unsigned int l1, unsigned int l2, unsigned int l3,
	unsigned int l4, unsigned int l5, unsigned int l6, unsigned int l7,
	unsigned int l8, unsigned int l9, unsigned int l10, unsigned int l11,
	unsigned int l12, unsigned int l13, unsigned int l14, unsigned int l15
);
tile_t create_solid_tile();
tile_t create_climb_tile();
unsigned int get_tile_slope_colision( const tile_t * tile, unsigned int x );
unsigned int is_tile_solid( const tile_t * tile );
unsigned int is_tile_slope( const tile_t * tile );
unsigned int is_tile_ceiling_slope( const tile_t * tile );
unsigned int get_tile_slope_steepness( const tile_t * tile );
unsigned int get_tile_slope_dirx( const tile_t * tile );
unsigned int is_tile_climbable( const tile_t * tile );

#endif // TILE_H
