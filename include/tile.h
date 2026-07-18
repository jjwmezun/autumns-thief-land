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
#define TILE_BOUNCY         16

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
	unsigned int col0, unsigned int col1, unsigned int col2, unsigned int col3,
	unsigned int col4, unsigned int col5, unsigned int col6, unsigned int col7,
	unsigned int col8, unsigned int col9, unsigned int col10, unsigned int col11,
	unsigned int col12, unsigned int col13, unsigned int col14, unsigned int col15
);
tile_t create_solid_tile();
tile_t create_climb_tile();
tile_t create_solid_top_tile();
tile_t create_solid_top_tile();
tile_t create_climb_solid_top_tile();
tile_t create_bouncy_tile();
void make_tile_underwater( tile_t * tile );
unsigned int get_tile_slope_colision( tile_t tile, unsigned int x );
unsigned int is_tile_solid( tile_t tile );
unsigned int is_tile_slope( tile_t tile );
unsigned int is_tile_ceiling_slope( tile_t tile );
unsigned int get_tile_slope_steepness( tile_t tile );
unsigned int get_tile_slope_dirx( tile_t tile );
unsigned int is_tile_climbable( tile_t tile );
unsigned int is_tile_solid_top( tile_t tile );
unsigned int is_tile_climb_solid_top( tile_t tile );
unsigned int is_tile_underwater( tile_t tile );
unsigned int is_tile_bouncy( tile_t tile );

#endif // TILE_H
