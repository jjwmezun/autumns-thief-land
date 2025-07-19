#ifndef TILE_H
#define TILE_H

#include <inttypes.h>

#define TILE_EMPTY 0
#define TILE_SOLID 1
#define TILE_SLOPE 2

typedef struct tile_t {
	unsigned int type : 4;
	uint32_t collision1;
	uint32_t collision2;
	uint32_t collision3;
} tile_t;

tile_t create_empty_tile();
tile_t create_sloped_tile
(
	unsigned l0, unsigned l1, unsigned l2, unsigned l3,
	unsigned l4, unsigned l5, unsigned l6, unsigned l7,
	unsigned l8, unsigned l9, unsigned l10, unsigned l11,
	unsigned l12, unsigned l13, unsigned l14, unsigned l15
);
tile_t create_solid_tile();
unsigned int get_tile_slope_colision( const tile_t * tile, unsigned int x );

#endif // TILE_H
