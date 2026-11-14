#include <stdlib.h>
#include <string.h>
#include "tile.h"

tile_t create_empty_tile()
{
	return ( tile_t ){ .type = TILE_EMPTY };
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
	slope_data_t data;
	data.dirx = dirx;
	data.steepness = steepness;
	data.isceiling = isceiling;
	data.cols[ 0 ] = col0;
	data.cols[ 1 ] = col1;
	data.cols[ 2 ] = col2;
	data.cols[ 3 ] = col3;
	data.cols[ 4 ] = col4;
	data.cols[ 5 ] = col5;
	data.cols[ 6 ] = col6;
	data.cols[ 7 ] = col7;
	data.cols[ 8 ] = col8;
	data.cols[ 9 ] = col9;
	data.cols[ 10 ] = col10;
	data.cols[ 11 ] = col11;
	data.cols[ 12 ] = col12;
	data.cols[ 13 ] = col13;
	data.cols[ 14 ] = col14;
	data.cols[ 15 ] = col15;

	tile_t tile;
	tile.type = TILE_SLOPE;
	tile.underwater = 0;
	tile.hurt = 0;
	tile.data = malloc( sizeof( slope_data_t ) );
	memcpy( tile.data, &data, sizeof( slope_data_t ) );

	return tile;
};

unsigned int get_tile_slope_colision( tile_t tile, unsigned int x )
{
	if ( x > 16 )
	{
		return 0;
	}
	else
	{
		return ( ( slope_data_t * )( tile.data ) )->cols[ x ];
	}
};
// Tile subtypes.

tile_t create_100_gem_tile()
{
	money_data_t data;
	data.amount = 100;

	tile_t tile;
	tile.type = TILE_MONEY;
	tile.data = malloc( sizeof( money_data_t ) );
	memcpy( tile.data, &data, sizeof( money_data_t ) );

	return tile;
};

tile_t create_solid_tile()
{
	return ( tile_t ){ .type = TILE_SOLID };
};

tile_t create_climb_tile()
{
	return ( tile_t ){ .type = TILE_CLIMB };
};

tile_t create_solid_top_tile()
{
	return ( tile_t ){ .type = TILE_SOLID_TOP };
};

tile_t create_climb_solid_top_tile()
{
	return ( tile_t ){ .type = TILE_CLIMB_SOLID_TOP };
};

tile_t create_bouncy_tile()
{
	return ( tile_t ){ .type = TILE_BOUNCY };
};

void make_tile_underwater( tile_t * tile )
{
	tile->underwater = 1;
};

unsigned int is_tile_100_gem( tile_t tile )
{
	return tile.type == TILE_MONEY && ( ( money_data_t * )( tile.data ) )->amount == 100;
};

unsigned int is_tile_money( tile_t tile )
{
	return tile.type == TILE_MONEY;
};

unsigned int is_tile_solid( tile_t tile )
{
	return tile.type == TILE_SOLID || tile.type == TILE_BOUNCY;
};

unsigned int is_tile_slope( tile_t tile )
{
	return tile.type == TILE_SLOPE && !( ( slope_data_t * )( tile.data ) )->isceiling;
};

unsigned int is_tile_ceiling_slope( tile_t tile )
{
	return tile.type == TILE_SLOPE && ( ( slope_data_t * )( tile.data ) )->isceiling;
};

uint32_t get_tile_money_amount( tile_t tile )
{
	return ( ( money_data_t * )( tile.data ) )->amount;
};

unsigned int get_tile_slope_steepness( tile_t tile )
{
	return ( ( slope_data_t * )( tile.data ) )->steepness;
};

unsigned int get_tile_slope_dirx( tile_t tile )
{
	return ( ( slope_data_t * )( tile.data ) )->dirx;
};

unsigned int is_tile_climbable( tile_t tile )
{
	return tile.type == TILE_CLIMB || tile.type == TILE_CLIMB_SOLID_TOP;
};

unsigned int is_tile_solid_top( tile_t tile )
{
	return tile.type == TILE_SOLID_TOP || tile.type == TILE_CLIMB_SOLID_TOP;
};

unsigned int is_tile_climb_solid_top( tile_t tile )
{
	return tile.type == TILE_CLIMB_SOLID_TOP;
};

unsigned int is_tile_underwater( tile_t tile )
{
	return tile.underwater;
};

unsigned int is_tile_bouncy( tile_t tile )
{
	return tile.type == TILE_BOUNCY;
};
