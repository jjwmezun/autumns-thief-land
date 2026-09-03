#include "config.h"
#include "engine.h"
#include "map.h"
#include <stdlib.h>

map_t create_map()
{
	map_t map;
	map.width = 64;
	map.height = 32;
	map.tiles = ( tile_t * )( calloc( map.width * map.height, sizeof( tile_t ) ));

	for ( size_t y = 0; y < map.height - 6; ++y )
	{
		const unsigned int l = y * map.width;
		const unsigned int r = ( y + 1 ) * map.width - 1;
		map.tiles[ l ] = create_solid_tile();
		map.tiles[ r ] = create_solid_tile();
	}

	for ( size_t x = 0; x < map.width; ++x )
	{
		map.tiles[ ( map.height - 1 ) * map.width + x ] = create_solid_tile();
		map.tiles[ ( map.height - 2 ) * map.width + x ] = create_solid_tile();
	}

	map.tiles[ ( map.height - 4 ) * map.width + 8 ] = create_solid_tile();
	map.tiles[ ( map.height - 4 ) * map.width + 9 ] = create_solid_tile();

	/*
	// Create a simple map with solid blocks.
	for ( size_t y = 0; y < map.height; ++y )
	{
		const unsigned int l = y * map.width;
		const unsigned int r = ( y + 1 ) * map.width - 1;
		map.tiles[ l ] = create_solid_tile();
		map.tiles[ r ] = create_solid_tile();
	}

	for ( size_t x = 0; x < map.width; ++x )
	{
		map.tiles[ ( map.height - 1 ) * map.width + x ] = create_solid_tile();
		map.tiles[ ( map.height - 2 ) * map.width + x ] = create_solid_tile();
	}

	map.tiles[ ( map.height - 4 ) * map.width + 15 ] = create_solid_tile();
	map.tiles[ ( map.height - 3 ) * map.width + 15 ] = create_solid_tile();
	map.tiles[ ( map.height - 3 ) * map.width + 16 ] = create_solid_tile();

	map.tiles[ ( map.height - 4 ) * map.width + 16 ] = create_sloped_tile(
		TILE_RIGHT,
		TILE_FLAT,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0
	);

	map.tiles[ ( map.height - 4 ) * map.width + 17 ] = create_sloped_tile(
		TILE_RIGHT,
		TILE_MEDIUM,
		0,
		0,
		1,
		2,
		3,
		4,
		5,
		6,
		7,
		8,
		9,
		10,
		11,
		12,
		13,
		14,
		15
	);

	map.tiles[ ( map.height - 3 ) * map.width + 17 ] = create_sloped_tile(
		TILE_RIGHT,
		TILE_FLAT,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0
	);

	map.tiles[ ( map.height - 3 ) * map.width + 18 ] = create_sloped_tile(
		TILE_RIGHT,
		TILE_MEDIUM,
		0,
		0,
		1,
		2,
		3,
		4,
		5,
		6,
		7,
		8,
		9,
		10,
		11,
		12,
		13,
		14,
		15
	);

	map.tiles[ ( map.height - 3 ) * map.width + 20 ] = create_sloped_tile(
		TILE_LEFT,
		TILE_MEDIUM,
		0,
		15,
		14,
		13,
		12,
		11,
		10,
		9,
		8,
		7,
		6,
		5,
		4,
		3,
		2,
		1,
		0
	);

	map.tiles[ ( map.height - 3 ) * map.width + 21 ] = create_sloped_tile(
		TILE_RIGHT,
		TILE_MEDIUM,
		0,
		0,
		1,
		2,
		3,
		4,
		5,
		6,
		7,
		8,
		9,
		10,
		11,
		12,
		13,
		14,
		15
	);

	map.tiles[ ( map.height - 3 ) * map.width + 4 ] = create_sloped_tile(
		TILE_RIGHT,
		TILE_FLAT,
		0,
		15,
		15,
		14,
		14,
		13,
		13,
		12,
		12,
		12,
		12,
		13,
		13,
		14,
		14,
		15,
		15
	);

	map.tiles[ ( map.height - 2 ) * map.width + 4 ] = create_sloped_tile(
		TILE_RIGHT,
		TILE_FLAT,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0
	);

	map.tiles[ ( map.height - 2 ) * map.width + 5 ] = create_sloped_tile(
		TILE_RIGHT,
		TILE_FLAT,
		0,
		0,
		0,
		1,
		1,
		2,
		2,
		3,
		3,
		3,
		3,
		2,
		2,
		1,
		1,
		0,
		0
	);

	map.tiles[ ( map.height - 2 ) * map.width + 6 ] = create_sloped_tile(
		TILE_RIGHT,
		TILE_FLAT,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0
	);

	map.tiles[ ( map.height - 3 ) * map.width + 6 ] = create_sloped_tile(
		TILE_RIGHT,
		TILE_FLAT,
		0,
		15,
		15,
		14,
		14,
		13,
		13,
		12,
		12,
		12,
		12,
		13,
		13,
		14,
		14,
		15,
		15
	);

	map.tiles[ ( map.height - 3 ) * map.width + 7 ] = create_sloped_tile(
		TILE_LEFT,
		TILE_LOW,
		0,
		15,
		15,
		14,
		14,
		13,
		13,
		12,
		12,
		11,
		11,
		10,
		10,
		9,
		9,
		8,
		8
	);

	map.tiles[ ( map.height - 3 ) * map.width + 8 ] = create_sloped_tile(
		TILE_RIGHT,
		TILE_LOW,
		0,
		8,
		8,
		9,
		9,
		10,
		10,
		11,
		11,
		12,
		12,
		13,
		13,
		14,
		14,
		15,
		15
	);

	map.tiles[ ( map.height - 3 ) * map.width + 27 ] = create_sloped_tile(
		TILE_LEFT,
		TILE_MEDIUM,
		0,
		15,
		14,
		13,
		12,
		11,
		10,
		9,
		8,
		7,
		6,
		5,
		4,
		3,
		2,
		1,
		0
	);

	map.tiles[ ( map.height - 3 ) * map.width + 28 ] = create_sloped_tile(
		TILE_LEFT,
		TILE_FLAT,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0
	);

	map.tiles[ ( map.height - 4 ) * map.width + 28 ] = create_sloped_tile(
		TILE_LEFT,
		TILE_MEDIUM,
		0,
		15,
		14,
		13,
		12,
		11,
		10,
		9,
		8,
		7,
		6,
		5,
		4,
		3,
		2,
		1,
		0
	);

	map.tiles[ ( map.height - 4 ) * map.width + 29 ] = create_sloped_tile(
		TILE_LEFT,
		TILE_FLAT,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0
	);

	map.tiles[ ( map.height - 5 ) * map.width + 29 ] = create_sloped_tile(
		TILE_LEFT,
		TILE_MEDIUM,
		0,
		15,
		14,
		13,
		12,
		11,
		10,
		9,
		8,
		7,
		6,
		5,
		4,
		3,
		2,
		1,
		0
	);

	map.tiles[ ( map.height - 5 ) * map.width + 30 ] = create_sloped_tile(
		TILE_LEFT,
		TILE_FLAT,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0
	);

	map.tiles[ ( map.height - 6 ) * map.width + 29 ] = create_sloped_tile(
		TILE_LEFT,
		TILE_FLAT,
		0,
		16,
		16,
		16,
		16,
		16,
		16,
		16,
		16,
		16,
		16,
		16,
		16,
		16,
		16,
		16,
		16
	);

	map.tiles[ ( map.height - 6 ) * map.width + 30 ] = create_sloped_tile(
		TILE_LEFT,
		TILE_MEDIUM,
		0,
		15,
		14,
		13,
		12,
		11,
		10,
		9,
		8,
		7,
		6,
		5,
		4,
		3,
		2,
		1,
		0
	);

	map.tiles[ ( map.height - 6 ) * map.width + 31 ] = create_sloped_tile(
		TILE_LEFT,
		TILE_FLAT,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0
	);

	for ( size_t i = 0; i < 25; ++i )
	{
		map.tiles[ 8 * map.width + i ] = create_solid_tile();
	}

	map.tiles[ 7 * map.width + 8 ] = create_sloped_tile(
		TILE_RIGHT,
		TILE_HIGH,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		2,
		4,
		6,
		8,
		10,
		12,
		14
	);

	map.tiles[ 6 * map.width + 8 ] = create_sloped_tile(
		TILE_RIGHT,
		TILE_HIGH,
		0,
		0,
		2,
		4,
		6,
		8,
		10,
		12,
		14,
		16,
		16,
		16,
		16,
		16,
		16,
		16,
		16
	);

	map.tiles[ 6 * map.width + 5 ] = create_sloped_tile(
		TILE_LEFT,
		TILE_HIGH,
		0,
		16,
		16,
		16,
		16,
		16,
		16,
		16,
		16,
		14,
		12,
		10,
		8,
		6,
		4,
		2,
		0
	);

	map.tiles[ 7 * map.width + 5 ] = create_sloped_tile(
		TILE_LEFT,
		TILE_HIGH,
		0,
		14,
		12,
		10,
		8,
		6,
		4,
		2,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0
	);

	map.tiles[ 5 * map.width + 7 ] = create_sloped_tile(
		TILE_RIGHT,
		TILE_HIGH,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		2,
		4,
		6,
		8,
		10,
		12,
		14
	);

	map.tiles[ 4 * map.width + 7 ] = create_sloped_tile(
		TILE_RIGHT,
		TILE_HIGH,
		0,
		0,
		2,
		4,
		6,
		8,
		10,
		12,
		14,
		16,
		16,
		16,
		16,
		16,
		16,
		16,
		16
	);

	map.tiles[ 4 * map.width + 6 ] = create_sloped_tile(
		TILE_LEFT,
		TILE_HIGH,
		0,
		16,
		16,
		16,
		16,
		16,
		16,
		16,
		16,
		14,
		12,
		10,
		8,
		6,
		4,
		2,
		0
	);

	map.tiles[ 5 * map.width + 6 ] = create_sloped_tile(
		TILE_LEFT,
		TILE_HIGH,
		0,
		14,
		12,
		10,
		8,
		6,
		4,
		2,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0
	);

	map.tiles[ 6 * map.width + 6 ] = create_sloped_tile(
		TILE_LEFT,
		TILE_FLAT,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0
	);

	map.tiles[ 6 * map.width + 7 ] = create_sloped_tile(
		TILE_RIGHT,
		TILE_FLAT,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0
	);

	map.tiles[ 7 * map.width + 12 ] = create_sloped_tile(
		TILE_LEFT,
		TILE_LOW,
		0,
		15,
		15,
		14,
		14,
		13,
		13,
		12,
		12,
		11,
		11,
		10,
		10,
		9,
		9,
		8,
		8
	);

	map.tiles[ 7 * map.width + 13 ] = create_sloped_tile(
		TILE_LEFT,
		TILE_LOW,
		0,
		7,
		7,
		6,
		6,
		5,
		5,
		4,
		4,
		3,
		3,
		2,
		2,
		1,
		1,
		0,
		0
	);

	map.tiles[ 7 * map.width + 14 ] = create_sloped_tile(
		TILE_RIGHT,
		TILE_LOW,
		0,
		0,
		0,
		1,
		1,
		2,
		2,
		3,
		3,
		4,
		4,
		5,
		5,
		6,
		6,
		7,
		7
	);

	map.tiles[ 7 * map.width + 15 ] = create_sloped_tile(
		TILE_RIGHT,
		TILE_LOW,
		0,
		8,
		8,
		9,
		9,
		10,
		10,
		11,
		11,
		12,
		12,
		13,
		13,
		14,
		14,
		15,
		15
	);

	map.tiles[ 9 * map.width + 24 ] = create_sloped_tile(
		TILE_RIGHT,
		TILE_HIGH,
		1,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		2,
		4,
		6,
		8,
		10,
		12,
		14
	);

	map.tiles[ 10 * map.width + 24 ] = create_sloped_tile(
		TILE_RIGHT,
		TILE_HIGH,
		1,
		0,
		2,
		4,
		6,
		8,
		10,
		12,
		14,
		16,
		16,
		16,
		16,
		16,
		16,
		16,
		16
	);

	map.tiles[ 11 * map.width + 23 ] = create_sloped_tile(
		TILE_RIGHT,
		TILE_HIGH,
		1,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		2,
		4,
		6,
		8,
		10,
		12,
		14
	);

	map.tiles[ 12 * map.width + 23 ] = create_sloped_tile(
		TILE_RIGHT,
		TILE_HIGH,
		1,
		0,
		2,
		4,
		6,
		8,
		10,
		12,
		14,
		16,
		16,
		16,
		16,
		16,
		16,
		16,
		16
	);

	map.tiles[ 9 * map.width + 21 ] = create_sloped_tile(
		TILE_LEFT,
		TILE_HIGH,
		1,
		14,
		12,
		10,
		8,
		6,
		4,
		2,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0
	);

	map.tiles[ 10 * map.width + 21 ] = create_sloped_tile(
		TILE_LEFT,
		TILE_HIGH,
		1,
		16,
		16,
		16,
		16,
		16,
		16,
		16,
		16,
		14,
		12,
		10,
		8,
		6,
		4,
		2,
		0
	);

	map.tiles[ 11 * map.width + 22 ] = create_sloped_tile(
		TILE_LEFT,
		TILE_HIGH,
		1,
		14,
		12,
		10,
		8,
		6,
		4,
		2,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0
	);

	map.tiles[ 12 * map.width + 22 ] = create_sloped_tile(
		TILE_LEFT,
		TILE_HIGH,
		1,
		16,
		16,
		16,
		16,
		16,
		16,
		16,
		16,
		14,
		12,
		10,
		8,
		6,
		4,
		2,
		0
	);
	map.tiles[ 9 * map.width + 23 ] = create_solid_tile();
	map.tiles[ 10 * map.width + 23 ] = create_solid_tile();
	map.tiles[ 9 * map.width + 22 ] = create_solid_tile();
	map.tiles[ 10 * map.width + 22 ] = create_solid_tile();
	map.tiles[ 9 * map.width + 19 ] = create_solid_tile();
	map.tiles[ 9 * map.width + 18 ] = create_solid_tile();
	map.tiles[ 9 * map.width + 17 ] = create_solid_tile();
	map.tiles[ 9 * map.width + 16 ] = create_solid_tile();
	//map.tiles[ 9 * map.width + 15 ] = create_solid_tile();
	map.tiles[ 9 * map.width + 14 ] = create_solid_tile();
	map.tiles[ 9 * map.width + 13 ] = create_solid_tile();
	map.tiles[ 9 * map.width + 12 ] = create_solid_tile();


	map.tiles[ 9 * map.width + 15 ] = create_sloped_tile(
		TILE_RIGHT,
		TILE_FLAT,
		1,
		0,
		0,
		1,
		1,
		2,
		2,
		3,
		3,
		3,
		3,
		2,
		2,
		1,
		1,
		0,
		0
	);

	map.tiles[ 10 * map.width + 16 ] = create_sloped_tile(
		TILE_RIGHT,
		TILE_FLAT,
		1,
		15,
		15,
		14,
		14,
		13,
		13,
		12,
		12,
		12,
		12,
		13,
		13,
		14,
		14,
		15,
		15
	);

	map.tiles[ 10 * map.width + 17 ] = create_sloped_tile(
		TILE_LEFT,
		TILE_LOW,
		1,
		15,
		15,
		14,
		14,
		13,
		13,
		12,
		12,
		11,
		11,
		10,
		10,
		9,
		9,
		8,
		8
	);

	map.tiles[ 10 * map.width + 18 ] = create_sloped_tile(
		TILE_RIGHT,
		TILE_LOW,
		1,
		8,
		8,
		9,
		9,
		10,
		10,
		11,
		11,
		12,
		12,
		13,
		13,
		14,
		14,
		15,
		15
	);

	for ( size_t i = 4; i < 16; ++i )
	{
		map.tiles[ i * map.width + 26 ] = create_climb_tile();
	}
	map.tiles[ 8 * map.width + 25 ] = create_solid_tile();
	map.tiles[ 8 * map.width + 27 ] = create_solid_tile();
	map.tiles[ 9 * map.width + 25 ] = create_solid_tile();
	map.tiles[ 9 * map.width + 27 ] = create_solid_tile();
	map.tiles[ 12 * map.width + 10 ] = create_solid_top_tile();
	map.tiles[ 12 * map.width + 11 ] = create_solid_top_tile();
	map.tiles[ 12 * map.width + 12 ] = create_solid_top_tile();
	map.tiles[ 11 * map.width + 29 ] = create_solid_top_tile();
	map.tiles[ 11 * map.width + 30 ] = create_solid_top_tile();
	map.tiles[ 10 * map.width + 29 ] = create_climb_tile();
	map.tiles[ 9 * map.width + 29 ] = create_climb_tile();
	map.tiles[ 3 * map.width + 26 ] = create_climb_solid_top_tile();
	map.tiles[ 13 * map.width + 2 ] = create_bouncy_tile();

	map.tiles[ 8 * map.width + 1 ] = create_empty_tile();
	map.tiles[ 8 * map.width + 2 ] = create_empty_tile();
	map.tiles[ 8 * map.width + 3 ] = create_empty_tile();

	for ( size_t y = 13; y < map.height; ++y )
	{
		for ( size_t x = 0; x < map.width; ++x )
		{
			make_tile_underwater( &map.tiles[ y * map.width + x ] );
		}
	}*/

	// Add BG.
	engine_add_graphic(
		( rect ){ 0.0f, 0.0f, ( float )( map.width * 16 ), ( float )( map.height * 16 ) },
		( color ){ 1.0f, 1.0f, 1.0f, 1.0f }
	);

	// Add solid block graphics.
	for ( size_t y = 0; y < map.height; ++y )
	{
		for ( size_t x = 0; x < map.width; ++x )
		{
			tile_t tile = map_get_tile( &map, x, y );
			if ( is_tile_bouncy( tile ) )
			{
				engine_add_graphic(
					( rect ){ 16.0f * ( float )( x ), 16.0f * ( float )( y ), 16.0f, 16.0f },
					( color ){ 1.0f, 0.0f, 0.0f, 1.0f }
				);
			}
			else if ( is_tile_solid( tile ) )
			{
				engine_add_graphic(
					( rect ){ 16.0f * ( float )( x ), 16.0f * ( float )( y ), 16.0f, 16.0f },
					( color ){ 0.0f, 0.0f, 0.0f, 1.0f }
				);
			}
			else if ( tile.type == TILE_SLOPE )
			{
				for ( size_t sx = 0; sx < 16; ++sx )
				{
					const float s = ( float )( get_tile_slope_colision( tile, sx ) );
					if ( s >= 16.0f )
					{
						continue;
					}
					const float sh = 16.0f - s;
					const float sy = is_tile_ceiling_slope( tile )
						? ( float )( y * 16 )
						: ( float )( y * 16 ) + ( 16.0f - sh );
					engine_add_graphic(
						( rect ){ 16.0f * ( float )( x ) + sx, sy, 1.0f, sh },
						( color ){ 0.0f, 0.0f, 0.0f, 1.0f }
					);
				}
			}
			else if ( is_tile_climb_solid_top( tile ) )
			{
				engine_add_graphic(
					( rect ){ 16.0f * ( float )( x ) + 2.0f, 16.0f * ( float )( y ), 4.0f, 16.0f },
					( color ){ 0.0f, 0.0f, 0.0f, 1.0f }
				);
				engine_add_graphic(
					( rect ){ 16.0f * ( float )( x ) + 10.0f, 16.0f * ( float )( y ), 4.0f, 16.0f },
					( color ){ 0.0f, 0.0f, 0.0f, 1.0f }
				);
				engine_add_graphic(
					( rect ){ 16.0f * ( float )( x ) + 2.0f, 16.0f * ( float )( y ) + 2.0f, 12.0f, 4.0f },
					( color ){ 0.0f, 0.0f, 0.0f, 1.0f }
				);
				engine_add_graphic(
					( rect ){ 16.0f * ( float )( x ) + 2.0f, 16.0f * ( float )( y ) + 10.0f, 12.0f, 4.0f },
					( color ){ 0.0f, 0.0f, 0.0f, 1.0f }
				);
				engine_add_graphic(
					( rect ){ 16.0f * ( float )( x ), 16.0f * ( float )( y ), 16.0f, 4.0f },
					( color ){ 0.0f, 0.0f, 0.0f, 1.0f }
				);
			}
			else if ( is_tile_climbable( tile ) )
			{
				engine_add_graphic(
					( rect ){ 16.0f * ( float )( x ) + 2.0f, 16.0f * ( float )( y ), 4.0f, 16.0f },
					( color ){ 0.0f, 0.0f, 0.0f, 1.0f }
				);
				engine_add_graphic(
					( rect ){ 16.0f * ( float )( x ) + 10.0f, 16.0f * ( float )( y ), 4.0f, 16.0f },
					( color ){ 0.0f, 0.0f, 0.0f, 1.0f }
				);
				engine_add_graphic(
					( rect ){ 16.0f * ( float )( x ) + 2.0f, 16.0f * ( float )( y ) + 2.0f, 12.0f, 4.0f },
					( color ){ 0.0f, 0.0f, 0.0f, 1.0f }
				);
				engine_add_graphic(
					( rect ){ 16.0f * ( float )( x ) + 2.0f, 16.0f * ( float )( y ) + 10.0f, 12.0f, 4.0f },
					( color ){ 0.0f, 0.0f, 0.0f, 1.0f }
				);
			}
			else if ( is_tile_solid_top( tile ) )
			{
				engine_add_graphic(
					( rect ){ 16.0f * ( float )( x ), 16.0f * ( float )( y ), 16.0f, 4.0f },
					( color ){ 0.0f, 0.0f, 0.0f, 1.0f }
				);
			}
		}
	}

	return map;
}

void add_priority_map_graphics( const map_t * map )
{
	// Add solid block graphics.
	for ( size_t y = 0; y < map->height; ++y )
	{
		for ( size_t x = 0; x < map->width; ++x )
		{
			const tile_t tile = map_get_tile( map, x, y );
			if ( is_tile_underwater( tile ) )
			{
				engine_add_graphic(
					( rect ){ 16.0f * ( float )( x ), 16.0f * ( float )( y ), 16.0f, 16.0f },
					( color ){ 0.0f, 0.5f, 1.0f, 0.5f }
				);
			}
		}
	}
}

tile_t map_get_tile( const map_t * map, unsigned int x, unsigned int y )
{
	return map->tiles[ y * map->width + x ];
}
