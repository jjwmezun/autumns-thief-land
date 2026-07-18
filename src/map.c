#include "config.h"
#include "engine.h"
#include "map.h"
#include <stdlib.h>

tile_t * create_map()
{
	tile_t * map = ( tile_t * )( calloc( WINDOW_WIDTH_BLOCKS * WINDOW_HEIGHT_BLOCKS, sizeof( tile_t ) ));

	// Create a simple map with solid blocks.
	for ( size_t y = 0; y < WINDOW_HEIGHT_BLOCKS; ++y )
	{
		const unsigned int l = y * WINDOW_WIDTH_BLOCKS;
		const unsigned int r = ( y + 1 ) * WINDOW_WIDTH_BLOCKS - 1;
		map[ l ] = create_solid_tile();
		map[ r ] = create_solid_tile();
	}

	for ( size_t x = 0; x < WINDOW_WIDTH_BLOCKS; ++x )
	{
		map[ ( WINDOW_HEIGHT_BLOCKS - 1 ) * WINDOW_WIDTH_BLOCKS + x ] = create_solid_tile();
		map[ ( WINDOW_HEIGHT_BLOCKS - 2 ) * WINDOW_WIDTH_BLOCKS + x ] = create_solid_tile();
	}

	map[ ( WINDOW_HEIGHT_BLOCKS - 4 ) * WINDOW_WIDTH_BLOCKS + 15 ] = create_solid_tile();
	map[ ( WINDOW_HEIGHT_BLOCKS - 3 ) * WINDOW_WIDTH_BLOCKS + 15 ] = create_solid_tile();
	map[ ( WINDOW_HEIGHT_BLOCKS - 3 ) * WINDOW_WIDTH_BLOCKS + 16 ] = create_solid_tile();

	map[ ( WINDOW_HEIGHT_BLOCKS - 4 ) * WINDOW_WIDTH_BLOCKS + 16 ] = create_sloped_tile(
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

	map[ ( WINDOW_HEIGHT_BLOCKS - 4 ) * WINDOW_WIDTH_BLOCKS + 17 ] = create_sloped_tile(
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

	map[ ( WINDOW_HEIGHT_BLOCKS - 3 ) * WINDOW_WIDTH_BLOCKS + 17 ] = create_sloped_tile(
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

	map[ ( WINDOW_HEIGHT_BLOCKS - 3 ) * WINDOW_WIDTH_BLOCKS + 18 ] = create_sloped_tile(
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

	map[ ( WINDOW_HEIGHT_BLOCKS - 3 ) * WINDOW_WIDTH_BLOCKS + 20 ] = create_sloped_tile(
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

	map[ ( WINDOW_HEIGHT_BLOCKS - 3 ) * WINDOW_WIDTH_BLOCKS + 21 ] = create_sloped_tile(
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

	map[ ( WINDOW_HEIGHT_BLOCKS - 3 ) * WINDOW_WIDTH_BLOCKS + 4 ] = create_sloped_tile(
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

	map[ ( WINDOW_HEIGHT_BLOCKS - 2 ) * WINDOW_WIDTH_BLOCKS + 4 ] = create_sloped_tile(
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

	map[ ( WINDOW_HEIGHT_BLOCKS - 2 ) * WINDOW_WIDTH_BLOCKS + 5 ] = create_sloped_tile(
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

	map[ ( WINDOW_HEIGHT_BLOCKS - 2 ) * WINDOW_WIDTH_BLOCKS + 6 ] = create_sloped_tile(
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

	map[ ( WINDOW_HEIGHT_BLOCKS - 3 ) * WINDOW_WIDTH_BLOCKS + 6 ] = create_sloped_tile(
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

	map[ ( WINDOW_HEIGHT_BLOCKS - 3 ) * WINDOW_WIDTH_BLOCKS + 7 ] = create_sloped_tile(
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

	map[ ( WINDOW_HEIGHT_BLOCKS - 3 ) * WINDOW_WIDTH_BLOCKS + 8 ] = create_sloped_tile(
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

	map[ ( WINDOW_HEIGHT_BLOCKS - 3 ) * WINDOW_WIDTH_BLOCKS + 27 ] = create_sloped_tile(
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

	map[ ( WINDOW_HEIGHT_BLOCKS - 3 ) * WINDOW_WIDTH_BLOCKS + 28 ] = create_sloped_tile(
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

	map[ ( WINDOW_HEIGHT_BLOCKS - 4 ) * WINDOW_WIDTH_BLOCKS + 28 ] = create_sloped_tile(
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

	map[ ( WINDOW_HEIGHT_BLOCKS - 4 ) * WINDOW_WIDTH_BLOCKS + 29 ] = create_sloped_tile(
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

	map[ ( WINDOW_HEIGHT_BLOCKS - 5 ) * WINDOW_WIDTH_BLOCKS + 29 ] = create_sloped_tile(
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

	map[ ( WINDOW_HEIGHT_BLOCKS - 5 ) * WINDOW_WIDTH_BLOCKS + 30 ] = create_sloped_tile(
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

	map[ ( WINDOW_HEIGHT_BLOCKS - 6 ) * WINDOW_WIDTH_BLOCKS + 29 ] = create_sloped_tile(
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

	map[ ( WINDOW_HEIGHT_BLOCKS - 6 ) * WINDOW_WIDTH_BLOCKS + 30 ] = create_sloped_tile(
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

	map[ ( WINDOW_HEIGHT_BLOCKS - 6 ) * WINDOW_WIDTH_BLOCKS + 31 ] = create_sloped_tile(
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
		map[ 8 * WINDOW_WIDTH_BLOCKS + i ] = create_solid_tile();
	}

	map[ 7 * WINDOW_WIDTH_BLOCKS + 8 ] = create_sloped_tile(
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

	map[ 6 * WINDOW_WIDTH_BLOCKS + 8 ] = create_sloped_tile(
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

	map[ 6 * WINDOW_WIDTH_BLOCKS + 5 ] = create_sloped_tile(
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

	map[ 7 * WINDOW_WIDTH_BLOCKS + 5 ] = create_sloped_tile(
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

	map[ 5 * WINDOW_WIDTH_BLOCKS + 7 ] = create_sloped_tile(
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

	map[ 4 * WINDOW_WIDTH_BLOCKS + 7 ] = create_sloped_tile(
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

	map[ 4 * WINDOW_WIDTH_BLOCKS + 6 ] = create_sloped_tile(
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

	map[ 5 * WINDOW_WIDTH_BLOCKS + 6 ] = create_sloped_tile(
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

	map[ 6 * WINDOW_WIDTH_BLOCKS + 6 ] = create_sloped_tile(
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

	map[ 6 * WINDOW_WIDTH_BLOCKS + 7 ] = create_sloped_tile(
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

	map[ 7 * WINDOW_WIDTH_BLOCKS + 12 ] = create_sloped_tile(
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

	map[ 7 * WINDOW_WIDTH_BLOCKS + 13 ] = create_sloped_tile(
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

	map[ 7 * WINDOW_WIDTH_BLOCKS + 14 ] = create_sloped_tile(
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

	map[ 7 * WINDOW_WIDTH_BLOCKS + 15 ] = create_sloped_tile(
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

	map[ 9 * WINDOW_WIDTH_BLOCKS + 24 ] = create_sloped_tile(
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

	map[ 10 * WINDOW_WIDTH_BLOCKS + 24 ] = create_sloped_tile(
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

	map[ 11 * WINDOW_WIDTH_BLOCKS + 23 ] = create_sloped_tile(
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

	map[ 12 * WINDOW_WIDTH_BLOCKS + 23 ] = create_sloped_tile(
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

	map[ 9 * WINDOW_WIDTH_BLOCKS + 21 ] = create_sloped_tile(
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

	map[ 10 * WINDOW_WIDTH_BLOCKS + 21 ] = create_sloped_tile(
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

	map[ 11 * WINDOW_WIDTH_BLOCKS + 22 ] = create_sloped_tile(
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

	map[ 12 * WINDOW_WIDTH_BLOCKS + 22 ] = create_sloped_tile(
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
	map[ 9 * WINDOW_WIDTH_BLOCKS + 23 ] = create_solid_tile();
	map[ 10 * WINDOW_WIDTH_BLOCKS + 23 ] = create_solid_tile();
	map[ 9 * WINDOW_WIDTH_BLOCKS + 22 ] = create_solid_tile();
	map[ 10 * WINDOW_WIDTH_BLOCKS + 22 ] = create_solid_tile();
	map[ 9 * WINDOW_WIDTH_BLOCKS + 19 ] = create_solid_tile();
	map[ 9 * WINDOW_WIDTH_BLOCKS + 18 ] = create_solid_tile();
	map[ 9 * WINDOW_WIDTH_BLOCKS + 17 ] = create_solid_tile();
	map[ 9 * WINDOW_WIDTH_BLOCKS + 16 ] = create_solid_tile();
	//map[ 9 * WINDOW_WIDTH_BLOCKS + 15 ] = create_solid_tile();
	map[ 9 * WINDOW_WIDTH_BLOCKS + 14 ] = create_solid_tile();
	map[ 9 * WINDOW_WIDTH_BLOCKS + 13 ] = create_solid_tile();
	map[ 9 * WINDOW_WIDTH_BLOCKS + 12 ] = create_solid_tile();


	map[ 9 * WINDOW_WIDTH_BLOCKS + 15 ] = create_sloped_tile(
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

	map[ 10 * WINDOW_WIDTH_BLOCKS + 16 ] = create_sloped_tile(
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

	map[ 10 * WINDOW_WIDTH_BLOCKS + 17 ] = create_sloped_tile(
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

	map[ 10 * WINDOW_WIDTH_BLOCKS + 18 ] = create_sloped_tile(
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
		map[ i * WINDOW_WIDTH_BLOCKS + 26 ] = create_climb_tile();
	}
	map[ 8 * WINDOW_WIDTH_BLOCKS + 25 ] = create_solid_tile();
	map[ 8 * WINDOW_WIDTH_BLOCKS + 27 ] = create_solid_tile();
	map[ 9 * WINDOW_WIDTH_BLOCKS + 25 ] = create_solid_tile();
	map[ 9 * WINDOW_WIDTH_BLOCKS + 27 ] = create_solid_tile();
	map[ 12 * WINDOW_WIDTH_BLOCKS + 10 ] = create_solid_top_tile();
	map[ 12 * WINDOW_WIDTH_BLOCKS + 11 ] = create_solid_top_tile();
	map[ 12 * WINDOW_WIDTH_BLOCKS + 12 ] = create_solid_top_tile();
	map[ 11 * WINDOW_WIDTH_BLOCKS + 29 ] = create_solid_top_tile();
	map[ 11 * WINDOW_WIDTH_BLOCKS + 30 ] = create_solid_top_tile();
	map[ 10 * WINDOW_WIDTH_BLOCKS + 29 ] = create_climb_tile();
	map[ 9 * WINDOW_WIDTH_BLOCKS + 29 ] = create_climb_tile();
	map[ 3 * WINDOW_WIDTH_BLOCKS + 26 ] = create_climb_solid_top_tile();

	/*
	for ( size_t y = 5; y < WINDOW_HEIGHT_BLOCKS; ++y )
	{
		for ( size_t x = 0; x < WINDOW_WIDTH_BLOCKS; ++x )
		{
			make_tile_underwater( &map[ y * WINDOW_WIDTH_BLOCKS + x ] );
		}
	}*/

	// Add solid block graphics.
	for ( size_t y = 0; y < WINDOW_HEIGHT_BLOCKS; ++y )
	{
		for ( size_t x = 0; x < WINDOW_WIDTH_BLOCKS; ++x )
		{
			tile_t tile = map[ y * WINDOW_WIDTH_BLOCKS + x ];
			if ( is_tile_solid( tile ) )
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

void add_priority_map_graphics( const tile_t * map )
{
	// Add solid block graphics.
	for ( size_t y = 0; y < WINDOW_HEIGHT_BLOCKS; ++y )
	{
		for ( size_t x = 0; x < WINDOW_WIDTH_BLOCKS; ++x )
		{
			const tile_t tile = map[ y * WINDOW_WIDTH_BLOCKS + x ];
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
