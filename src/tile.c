#include "tile.h"

tile_t create_empty_tile()
{
	return ( tile_t ){ .type = TILE_EMPTY, .collision1 = 0, .collision2 = 0, .collision3 = 0 };
};

tile_t create_sloped_tile
(
	unsigned int dirx, unsigned int steepness,
	unsigned int l0, unsigned int l1, unsigned int l2, unsigned int l3,
	unsigned int l4, unsigned int l5, unsigned int l6, unsigned int l7,
	unsigned int l8, unsigned int l9, unsigned int l10, unsigned int l11,
	unsigned int l12, unsigned int l13, unsigned int l14, unsigned int l15
)
{
	return ( tile_t )
	{
		.type = TILE_SLOPE,
		.dirx = dirx,
		.steepness = steepness,
		.collision1 =
			( l0 << 0 ) |
			( l1 << 5 ) |
			( l2 << 10 ) |
			( l3 << 15 ) |
			( l4 << 20 ) |
			( l5 << 25 ),
		.collision2 =
			( l6 << 0 ) |
			( l7 << 5 ) |
			( l8 << 10 ) |
			( l9 << 15 ) |
			( l10 << 20 ) |
			( l11 << 25 ),
		.collision3 =
			( l12 << 0 ) |
			( l13 << 5 ) |
			( l14 << 10 ) |
			( l15 << 15 )
	};
};

unsigned int get_tile_slope_colision( const tile_t * tile, unsigned int x )
{
	if ( x > 16 )
	{
		return 0;
	}
	else if ( x < 6 )
	{
		return ( tile->collision1 >> ( x * 5 ) ) & 0x1F;
	}
	else if ( x < 12 )
	{
		return ( tile->collision2 >> ( ( x - 6 ) * 5 ) ) & 0x1F;
	}
	else
	{
		return ( tile->collision3 >> ( ( x - 12 ) * 5 ) ) & 0x1F;
	}
};

tile_t create_solid_tile()
{
	return ( tile_t ){ .type = TILE_SOLID, .collision1 = 0xFFFFFFFF, .collision2 = 0xFFFFFFFF, .collision3 = 0xFFFFFFFF };
};
