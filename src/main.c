#include "config.h"
#include "engine.h"
#include "map.h"
#include <stdio.h>
#include "rand.h"
#include "sprite.h"
#include "tile.h"

#define SPRITE_COUNT 1

static unsigned int running = 1;
static float prev_ticks = 0.0f;
static float maxdt = 0.0f;

int main()
{
	// Init game engine.
	if ( engine_init( "Autumn’s Thief Land" ) != 0 )
	{
		fprintf( stderr, "Failed to initialize engine.\n" );
		return 1;
	}

	rand_init();

	// Init map.
	map_t map = create_map();
	camera_t camera = { 0.0f, 0.0f, WINDOW_WIDTH_PIXELS_F, WINDOW_HEIGHT_PIXELS_F };

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
			( rect ){ 16.0f * ( float )( i ) - 0.5f, 0.0f, 1.0f, ( float )( map.height * 16 ) },
			( color ){ 0.0f, 0.0f, 1.0f, 0.5f }
		);
	}
	for ( size_t i = 0; i < map.height; ++i )
	{
		engine_add_graphic(
			( rect ){ 0.0f, 16.0f * ( float )( i ) - 0.5f, ( float )( map.width * 16 ), 1.0f },
			( color ){ 0.0f, 0.0f, 1.0f, 0.5f }
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

		engine_render( &camera );

		const float ticks = engine_get_ticks();
		if ( ticks - prev_ticks < 16.0f )
		{
			engine_sleep( 16 - ( ticks - prev_ticks ) );
		}
		prev_ticks = engine_get_ticks();
	}

	// Test lowest FPS.
	printf( "Max delta: %.5f seconds.\n", maxdt / 60.0f );

	return 0;
}
