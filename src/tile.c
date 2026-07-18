#include "tile.h"

tile_t create_empty_tile()
{
	return ( tile_t ){ .type = TILE_NORMAL, .data = { .normal = { .subtype = TILE_EMPTY } } };
};

tile_t create_sloped_tile
(
	unsigned int dirx, unsigned int steepness, unsigned int isceiling,
	unsigned int col0, unsigned int col1, unsigned int col2, unsigned int col3,
	unsigned int col4, unsigned int col5, unsigned int col6, unsigned int col7,
	unsigned int col8, unsigned int col9, unsigned int col10, unsigned int col11,
	unsigned int col12, unsigned int col13, unsigned int col14, unsigned int col15
)
{
	return ( tile_t )
	{
		.type = TILE_SLOPE,
		.data = {
			.slope =
			{
				.data1 =
					( col0 << 0 ) |
					( col1 << 5 ) |
					( col2 << 10 ) |
					( col3 << 15 ) |
					( col4 << 20 ) |
					( col5 << 25 ),
				.data2 =
					( col6 << 0 ) |
					( col7 << 5 ) |
					( col8 << 10 ) |
					( col9 << 15 ) |
					( col10 << 20 ) |
					( col11 << 25 ),
				.data3 =
					( col12 << 0 ) |
					( col13 << 5 ) |
					( col14 << 10 ) |
					( col15 << 15 ) |
					( isceiling << 20 ) |
					( dirx << 21 ) |
					( steepness << 22 )
			}
		}
	};
};

unsigned int get_tile_slope_colision( tile_t tile, unsigned int x )
{
	if ( x > 16 )
	{
		return 0;
	}
	else if ( x < 6 )
	{
		return ( tile.data.slope.data1 >> ( x * 5 ) ) & 0x1F;
	}
	else if ( x < 12 )
	{
		return ( tile.data.slope.data2 >> ( ( x - 6 ) * 5 ) ) & 0x1F;
	}
	else
	{
		return ( tile.data.slope.data3 >> ( ( x - 12 ) * 5 ) ) & 0x1F;
	}
};

tile_t create_solid_tile()
{
	return ( tile_t ){ .type = TILE_NORMAL, .data = { .normal = { .subtype = TILE_SOLID } } };
};

tile_t create_climb_tile()
{
	return ( tile_t ){ .type = TILE_NORMAL, .data = { .normal = { .subtype = TILE_CLIMB } } };
};

tile_t create_solid_top_tile()
{
	return ( tile_t ){ .type = TILE_NORMAL, .data = { .normal = { .subtype = TILE_SOLID_TOP } } };
};

tile_t create_climb_solid_top_tile()
{
	return ( tile_t ){ .type = TILE_NORMAL, .data = { .normal = { .subtype = TILE_CLIMB_SOLID_TOP } } };
};

void make_tile_underwater( tile_t * tile )
{
	tile->underwater = 1;
};

unsigned int is_tile_solid( tile_t tile )
{
	return tile.type == TILE_NORMAL && tile.data.normal.subtype == TILE_SOLID;
};

unsigned int is_tile_slope( tile_t tile )
{
	return tile.type == TILE_SLOPE && !((tile.data.slope.data3 >> 20) & 0x1);
};

unsigned int is_tile_ceiling_slope( tile_t tile )
{
	return tile.type == TILE_SLOPE && ((tile.data.slope.data3 >> 20) & 0x1);
};

unsigned int get_tile_slope_steepness( tile_t tile )
{
	return ( tile.data.slope.data3 >> 22 ) & 0x3;
};

unsigned int get_tile_slope_dirx( tile_t tile )
{
	return ( tile.data.slope.data3 >> 21 ) & 0x1;
};

unsigned int is_tile_climbable( tile_t tile )
{
	return tile.type == TILE_NORMAL && ( tile.data.normal.subtype == TILE_CLIMB || tile.data.normal.subtype == TILE_CLIMB_SOLID_TOP );
};

unsigned int is_tile_solid_top( tile_t tile )
{
	return tile.type == TILE_NORMAL && ( tile.data.normal.subtype == TILE_SOLID_TOP || tile.data.normal.subtype == TILE_CLIMB_SOLID_TOP );
};

unsigned int is_tile_climb_solid_top( tile_t tile )
{
	return tile.type == TILE_NORMAL && tile.data.normal.subtype == TILE_CLIMB_SOLID_TOP;
};

unsigned int is_tile_underwater( tile_t tile )
{
	return tile.underwater;
};
