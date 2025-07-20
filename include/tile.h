#ifndef TILE_H
#define TILE_H

#include <inttypes.h>

#define TILE_EMPTY 0
#define TILE_SOLID 1
#define TILE_SLOPE 2

#define TILE_LEFT 0
#define TILE_RIGHT 1

#define TILE_UP 0
#define TILE_DOWN 1

#define TILE_FLAT 0
#define TILE_LOW 1
#define TILE_MEDIUM 2
#define TILE_HIGH 3

typedef struct tile_t {
	unsigned int type : 4;
	unsigned int dirx : 1;
	unsigned int diry : 1;
	unsigned int steepness : 2;
	uint32_t collision1;
	uint32_t collision2;
	uint32_t collision3;
} tile_t;

tile_t create_empty_tile();
tile_t create_sloped_tile
(
	unsigned int dirx, unsigned int steepness,
	unsigned int l0, unsigned int l1, unsigned int l2, unsigned int l3,
	unsigned int l4, unsigned int l5, unsigned int l6, unsigned int l7,
	unsigned int l8, unsigned int l9, unsigned int l10, unsigned int l11,
	unsigned int l12, unsigned int l13, unsigned int l14, unsigned int l15
);
tile_t create_solid_tile();
unsigned int get_tile_slope_colision( const tile_t * tile, unsigned int x );

#endif // TILE_H
