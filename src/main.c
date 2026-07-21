#include "config.h"
#include "engine.h"
#include "map.h"
#include <stdio.h>
#include "player.h"
#include "sprite.h"
#include "tile.h"

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

	// Add BG.
	engine_add_graphic(
		( rect ){ 0.0f, 0.0f, WINDOW_WIDTH_PIXELS_F, WINDOW_HEIGHT_PIXELS_F },
		( color ){ 1.0f, 1.0f, 1.0f, 1.0f }
	);

	// Init map.
	tile_t * map = create_map();

	// Init player sprite.
	//player_t player = player_create( 16.0f, 15.0f );

	// Init other sprites.
	sprite_t player = sprite_create( 16.0f, 15.0f, SPRITE_TYPE_PLAYER );
	sprite_t apple = sprite_create( 10.0f, 15.0f, SPRITE_TYPE_APPLE );
	//sprite_t pollo = sprite_create( 22.0f, 15.0f, SPRITE_TYPE_POLLO );

	add_priority_map_graphics( map );

	// Add gridline graphics.
	for ( size_t i = 0; i < WINDOW_WIDTH_BLOCKS; ++i )
	{
		engine_add_graphic(
			( rect ){ 16.0f * ( float )( i ) - 0.5f, 0.0f, 1.0f, WINDOW_HEIGHT_PIXELS_F },
			( color ){ 0.0f, 0.0f, 1.0f, 0.5f }
		);
	}
	for ( size_t i = 0; i < WINDOW_HEIGHT_BLOCKS; ++i )
	{
		engine_add_graphic(
			( rect ){ 0.0f, 16.0f * ( float )( i ) - 0.5f, WINDOW_WIDTH_PIXELS_F, 1.0f },
			( color ){ 0.0f, 0.0f, 1.0f, 0.5f }
		);
	}

	while ( running )
	{
		running = engine_loop();

		// Update sprites.
		sprite_update( map, &player );
		sprite_update( map, &apple );
		//sprite_update( map, &pollo );

		// Handle sprite interaction.
		//player_interact_with_sprite( &player, &apple );
		//player_interact_with_sprite( &player, &pollo );

		engine_render();

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
