#include "config.h"
#include "data.h"
#include "engine.h"
#include "map.h"
#include <stdlib.h>
#include <stdio.h>
#include "rand.h"
#include "sprite.h"
#include <string.h>
#include "tile.h"

#define SPRITE_COUNT 1

static unsigned int running = 1;
static float prev_ticks = 0.0f;
static float maxdt = 0.0f;
static float palette_index = 0.0f;
static float timecounts[ 1000 ] = { 0.0f };
static size_t timecount_index = 0;

int main()
{
	// Init game data.
	if ( data_load() != 0 )
	{
		fprintf( stderr, "Failed to load data.\n" );
		return 1;
	}

	// Init game engine.
	if ( engine_init( "Autumn’s Thief Land" ) != 0 )
	{
		fprintf( stderr, "Failed to initialize engine.\n" );
		return 1;
	}

	// Init texture.
	unsigned char pixels[ 1024 * 1024 ];
	memset( pixels, 0, 1024 * 1024 );
	unsigned char * block_gfx_data = data_get_universal_block_gfx_data();
	for ( size_t y = 0; y < 64; ++y )
	{
		for ( size_t x = 0; x < 512; ++x )
		{
			pixels[ y * 1024 + x ] = block_gfx_data[ y * 512 + x ] * 32;
		}
	}
	unsigned char * sprite_gfx_data = data_get_sprite_gfx_data();
	for ( size_t y = 0; y < 512; ++y )
	{
		for ( size_t x = 0; x < 512; ++x )
		{
			pixels[ y * 1024 + 512 + x ] = sprite_gfx_data[ y * 512 + x ] * 32;
		}
	}
	engine_change_texture( pixels );

	// Init palette.
	const size_t palette_count = data_get_main_palette_count();
	unsigned char * colors = data_get_main_palette_data();
	engine_set_palettes( colors, palette_count );
	free( colors );

	rand_init();

	// Init map.
	map_t map = create_map();
	camera_t camera = { 0.0f, 0.0f, WINDOW_WIDTH_PIXELS_F, WINDOW_HEIGHT_PIXELS_F };

	// Init BG.
	gfx_data_t bg_gfx_data = data_get_background_gfx_data( 0 );
	unsigned char bgpixels[ bg_gfx_data.width * bg_gfx_data.height ];
	memset( bgpixels, 0, bg_gfx_data.width * bg_gfx_data.height );
	for ( size_t y = 0; y < bg_gfx_data.height; ++y )
	{
		for ( size_t x = 0; x < bg_gfx_data.width; ++x )
		{
			bgpixels[ y * bg_gfx_data.width + x ] = bg_gfx_data.pixels[ y * bg_gfx_data.width + x ] * 32;
		}
	}
	engine_change_bg_texture( bgpixels, bg_gfx_data.width, bg_gfx_data.height, map.width, map.height, 1.0f, 1.0f );

	// Init other sprites.
	sprite_t sprites[ SPRITE_COUNT ] = {
		sprite_create( 16.0f, 15.0f, SPRITE_TYPE_PLAYER ),
		//sprite_create( 10.0f, 15.0f, SPRITE_TYPE_APPLE ),
		//sprite_create( 22.0f, 15.0f, SPRITE_TYPE_POLLO_STILL ),
		//sprite_create( 22.0f, 15.0f, SPRITE_TYPE_BEE_SPIN ),
		//sprite_create( 22.0f, 15.0f, SPRITE_TYPE_POLLO_MOVE_HORIZONTAL ),
		//sprite_create( 13.0f, 15.0f, SPRITE_TYPE_CRAB )
	};

	add_priority_map_graphics( &map );

	// Add gridline graphics.
	for ( size_t i = 0; i < map.width; ++i )
	{
		engine_add_graphic(
			( rect_t ){ 16.0f * ( float )( i ) - 0.5f, 0.0f, 1.0f, ( float )( map.height * 16 ) },
			( color_t ){ 0.0f, 0.0f, 1.0f, 0.5f }
		);
	}
	for ( size_t i = 0; i < map.height; ++i )
	{
		engine_add_graphic(
			( rect_t ){ 0.0f, 16.0f * ( float )( i ) - 0.5f, ( float )( map.width * 16 ), 1.0f },
			( color_t ){ 0.0f, 0.0f, 1.0f, 0.5f }
		);
	}

	while ( running )
	{
		running = engine_loop();

		// Update sprites.
		for ( size_t i = 0; i < SPRITE_COUNT; ++i )
		{
			sprite_update( &map, &sprites[ i ], &camera );
			for ( size_t j = 0; j < SPRITE_COUNT; ++j )
			{
				if ( i != j )
				{
					sprite_interact( &sprites[ i ], &sprites[ j ] );
				}
			}
		}

		palette_index += 0.01f;
		engine_set_palette_index( palette_index );

		engine_render( &camera );

		const float ticks = engine_get_ticks();
		if ( timecount_index < 1000 )
		{
			timecounts[ timecount_index++ ] = ticks - prev_ticks;
		}
		if ( ticks - prev_ticks < 16.0f )
		{
			engine_sleep( 16 - ( ticks - prev_ticks ) );
		}
		prev_ticks = engine_get_ticks();
	}

	// Test lowest FPS.
	float maxtime = 0.0f;
	float totaltime = 0.0f;
	for ( size_t i = 0; i < timecount_index; ++i )
	{
		if ( timecounts[ i ] > maxtime )
		{
			maxtime = timecounts[ i ];
		}
		totaltime += timecounts[ i ];
	}
	float avgtime = totaltime / timecount_index;
	printf( "Max time: %.5f seconds.\n", maxtime / 60.0f );
	printf( "Avg time: %.5f seconds.\n", avgtime / 60.0f );

	return 0;
}
