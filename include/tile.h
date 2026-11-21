#ifndef TILE_H
#define TILE_H

#include <inttypes.h>

// Tile types.
#define TILE_EMPTY           0
#define TILE_NORMAL          1
#define TILE_SLOPE           2
#define TILE_MOVE            3
#define TILE_MONEY           4
#define TILE_SOLID           5
#define TILE_SOLID_TOP       6
#define TILE_CLIMB           7
#define TILE_CLIMB_SOLID_TOP 8
#define TILE_WARP			 9
#define TILE_CARD			10
#define TILE_KEY			11
#define TILE_DOOR			12
#define TILE_DOOR_LOCKED	13
#define TILE_DIAMOND	    14
#define TILE_HEART		    15
#define TILE_CLUB		    16
#define TILE_SPADE		    17
#define TILE_HEALTH		    18
#define TILE_FULL_HEALTH    19
#define TILE_BOUNCY         20

// Slope directions.
#define TILE_LEFT 0
#define TILE_RIGHT 1

// Slope steepness.
#define TILE_FLAT 0
#define TILE_LOW 1
#define TILE_MEDIUM 2
#define TILE_HIGH 3

typedef struct slope_data_t
{
	unsigned int dirx : 1;
	unsigned int steepness : 2;
	unsigned int isceiling : 1;
	uint8_t cols[ 16 ];
} slope_data_t;

typedef struct money_data_t
{
	uint32_t amount;
	uint16_t gfx1;
	uint16_t gfx2;
	uint16_t gfx3;
	uint16_t gfx4;
	unsigned int gfx_layer;
} money_data_t;

typedef struct tile_t {
	uint8_t type;
	unsigned int underwater : 1;
	unsigned int hurt : 1;
	unsigned int dirx : 1;
	unsigned int speedx : 4;
	void * data;
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
tile_t create_100_gem_tile();
tile_t create_solid_tile();
tile_t create_climb_tile();
tile_t create_solid_top_tile();
tile_t create_solid_top_tile();
tile_t create_climb_solid_top_tile();
tile_t create_bouncy_tile();
void make_tile_underwater( tile_t * tile );
unsigned int get_tile_slope_colision( tile_t tile, unsigned int x );
unsigned int is_tile_100_gem( tile_t tile );
unsigned int is_tile_money( tile_t tile );
unsigned int is_tile_solid( tile_t tile );
unsigned int is_tile_slope( tile_t tile );
unsigned int is_tile_ceiling_slope( tile_t tile );
uint32_t get_tile_money_amount( tile_t tile );
unsigned int get_tile_slope_steepness( tile_t tile );
unsigned int get_tile_slope_dirx( tile_t tile );
unsigned int is_tile_climbable( tile_t tile );
unsigned int is_tile_solid_top( tile_t tile );
unsigned int is_tile_climb_solid_top( tile_t tile );
unsigned int is_tile_underwater( tile_t tile );
unsigned int is_tile_bouncy( tile_t tile );

#endif // TILE_H
