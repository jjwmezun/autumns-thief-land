#include <brotli/decode.h>
#include "data.h"
#include "engine.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CIRCLE_COUNT 5
#define LEVELS_PER_CIRCLE 16
#define LEVELS_PER_GAME ( CIRCLE_COUNT * LEVELS_PER_CIRCLE )
#define LEVEL_COUNT ( LEVELS_PER_GAME * 2 )

#define GOAL_TYPE_KEYCANE     0
#define GOAL_TYPE_COLLECT_PTS 1

static void get_bits_from_byte( unsigned char byte, uint8_t * bits );
static unsigned char * data_get_gfx_data( uint32_t pointer, size_t width, size_t height );
static float get_float_from_bytes( const unsigned char * data, size_t i );
static uint16_t get_uint16_from_bytes( const unsigned char * data, size_t i );
static uint32_t get_uint32_from_bytes( const unsigned char * data, size_t i );
static data_t open_file( const char * filename );

static data_t data;

unsigned int data_load()
{
	data = open_file( "data.had" );

	if ( data.size < 4 )
	{
		fprintf( stderr, "Data file is too small.\n" );
		return 1;
	}

	uint32_t checksum = 0;
	uint32_t expected_checksum = get_uint32_from_bytes( data.data, data.size - 4 );
	for ( size_t i = 0; i < data.size - 4; ++i )
	{
		checksum += data.data[ i ];
	}
	if ( checksum != expected_checksum )
	{
		fprintf( stderr, "Data file checksum mismatch: expected %u, got %u.\n", expected_checksum, checksum );
		return 1;
	}

	return 0;
}

gfx_data_t data_get_background_gfx_data( unsigned int n )
{
	const size_t toc_entry = 2
		+ ( data_get_main_palette_count() * 4 )
		+ ( data_get_overworld_palette_count() * 4 )
		+ 32;
	const uint32_t bg_start_pointer = get_uint32_from_bytes( data.data, toc_entry );
	const uint32_t bg_pointer = get_uint32_from_bytes( data.data, bg_start_pointer + 1 + n * 4 );
	size_t width = data.data[ bg_pointer ] * 8;
	size_t height = data.data[ bg_pointer + 1 ] * 8;
	return ( gfx_data_t ){ data_get_gfx_data( bg_pointer + 2, width, height ), width, height };
}

level_data_t data_get_level_data( unsigned int n )
{
	const size_t toc_entry = 2
		+ ( data_get_main_palette_count() * 4 )
		+ ( data_get_overworld_palette_count() * 4 )
		+ 36
		+ ( LEVEL_COUNT * 4 );
	uint32_t level_pointer = get_uint32_from_bytes( data.data, toc_entry + n * 4 );
	const uint32_t level_pointer_before_map_pointers = level_pointer;

	level_data_t level_data;

	level_data.goal.goal_type = data.data[ level_pointer++ ];

	switch ( level_data.goal.goal_type )
	{
		case GOAL_TYPE_KEYCANE:
		{
		}
		break;
		case GOAL_TYPE_COLLECT_PTS:
		{
			level_data.goal.data.collect_pts.amount = get_uint32_from_bytes( data.data, level_pointer );
			level_pointer += 4;
		}
		break;
		default:
		{
			fprintf( stderr, "Unknown goal ID: %u\n", level_data.goal.goal_type );
		}
		break;
	}

	level_data.map_count = data.data[ level_pointer++ ];
	printf( "Map count: %u\n", level_data.map_count );

	level_data.map_pointers = calloc( level_data.map_count, sizeof( uint32_t ) );
	for ( size_t i = 0; i < level_data.map_count; ++i )
	{
		level_data.map_pointers[ i ] = get_uint32_from_bytes( data.data, level_pointer ) + level_pointer_before_map_pointers;
		level_pointer += 4;
	}

	return level_data;
}

#define LAYER_PARSING_LAYER_OPTIONS 0
#define LAYER_PARSING_OBJECT_TYPE   1
#define LAYER_PARSING_OBJECT_DATA   2

#define LAYER_TYPE_BLOCK      0
#define LAYER_TYPE_SPRITE     1
#define LAYER_TYPE_BACKGROUND 2

#define OBJECT_TYPE_UNIVERSAL_GEM_100 0

#define OBJECT_TYPE_URBAN_GROUND   256
#define OBJECT_TYPE_URBAN_BUILDING 258

map_t data_get_map_data( uint32_t pointer )
{
	map_t map;
	
	// Read main map options ( width, height, palette, tileset, layer count ).
	map.width = get_uint16_from_bytes( data.data, pointer );
	pointer += 2;
	map.height = get_uint16_from_bytes( data.data, pointer );
	pointer += 2;
	map.palette = data.data[ pointer++ ];
	printf( "Palette: %u\n", map.palette );
	const unsigned int tileset = data.data[ pointer++ ];
	const unsigned int layer_count = data.data[ pointer++ ];

	map.tiles = ( tile_t * )( calloc( map.width * map.height, sizeof( tile_t ) ));

	// Read layer data from buffer.
	uint8_t current_layer_index = 0;
	unsigned int state = LAYER_PARSING_LAYER_OPTIONS;
	uint8_t layer_type = 0;
	uint16_t object_type = 0;
	unsigned int current_gfx_layer = 0;
	while ( current_layer_index < layer_count )
	{
		switch ( state )
		{
			case LAYER_PARSING_LAYER_OPTIONS:
			{
				layer_type = data.data[ pointer++ ];
				switch ( layer_type )
				{
					case LAYER_TYPE_BLOCK:
					case LAYER_TYPE_SPRITE:
					{
						const uint32_t scrollx = get_uint32_from_bytes( data.data, pointer );
						pointer += 4;
						current_gfx_layer = engine_add_layer( GFX_LAYER_TYPE_TILE );
					}
					break;
					case LAYER_TYPE_BACKGROUND:
					{
						const uint8_t graphics = data.data[ pointer++ ];
						const float scrollx = get_float_from_bytes( data.data, pointer );
						pointer += 4;
						const uint32_t scrolly = get_uint32_from_bytes( data.data, pointer );
						pointer += 4;
						current_gfx_layer = engine_add_layer( GFX_LAYER_TYPE_BG );

						gfx_data_t bg_gfx_data = data_get_background_gfx_data( graphics );
						unsigned char bgpixels[ bg_gfx_data.width * bg_gfx_data.height ];
						memset( bgpixels, 0, bg_gfx_data.width * bg_gfx_data.height );
						for ( size_t y = 0; y < bg_gfx_data.height; ++y )
						{
							for ( size_t x = 0; x < bg_gfx_data.width; ++x )
							{
								bgpixels[ y * bg_gfx_data.width + x ] = bg_gfx_data.pixels[ y * bg_gfx_data.width + x ] * 32;
							}
						}
						engine_change_bg_layer_texture( current_gfx_layer, bgpixels, bg_gfx_data.width, bg_gfx_data.height, map.width, map.height, scrollx, 1.0f );
					}
					break;
					default:
					{
						fprintf( stderr, "Unknown layer type: %u\n", layer_type );
						exit( 1 );
					}
					break;
				}
				state = LAYER_PARSING_OBJECT_TYPE;
			}
			break;
			case LAYER_PARSING_OBJECT_TYPE:
			{
				object_type = get_uint16_from_bytes( data.data, pointer );
				pointer += 2;

				// If objectType is terminator, move to next layer.
				if ( object_type == 0xFFFF )
				{
					++current_layer_index;
					state = LAYER_PARSING_LAYER_OPTIONS;
				}
				else // Otherwise, interpret bytes as objectType for next object.
				{
					state = LAYER_PARSING_OBJECT_DATA;
				}
			}
			break;
			case LAYER_PARSING_OBJECT_DATA:
			{
				switch ( layer_type )
				{
					case LAYER_TYPE_BLOCK:
					{
						switch ( object_type )
						{
							case ( OBJECT_TYPE_UNIVERSAL_GEM_100 ):
							{
								const uint16_t id = get_uint16_from_bytes( data.data, pointer );
								pointer += 2;
								const uint16_t x = get_uint16_from_bytes( data.data, pointer );
								pointer += 2;
								const uint16_t y = get_uint16_from_bytes( data.data, pointer );
								pointer += 2;
								const uint8_t width = data.data[ pointer++ ];
								const uint8_t height = data.data[ pointer++ ];
								printf( "Universal Gem 100 - ID: %u, X: %u, Y: %u, Width: %u, Height: %u\n", id, x, y, width, height );

								for ( uint8_t yi = y; yi < height + y; ++yi )
								{
									for ( uint8_t xi = x; xi < width + x; ++xi )
									{
										map.tiles[ ( yi * map.width ) + xi ] = create_100_gem_tile();
										tile_t * tileref = &map.tiles[ ( yi * map.width ) + xi ];
										( ( money_data_t * )( tileref->data ) )->gfx_layer = current_gfx_layer;
										( ( money_data_t * )( tileref->data ) )->gfx1 = engine_add_tile
										(
											current_gfx_layer,
											( tile_gfx_args_t ) {
												.x = 16.0f * ( float )( xi ),
												.y = 16.0f * ( float )( yi ),
												.srcx = get_tileset_universal_x( 0.0f ),
												.srcy = get_tileset_universal_y( 1.0f ),
												.animation = 5.0f
											}
										);
										( ( money_data_t * )( tileref->data ) )->gfx2 = engine_add_tile
										(
											current_gfx_layer,
											( tile_gfx_args_t ) {
												.x = 16.0f * ( float )( xi ) + 8.0f,
												.y = 16.0f * ( float )( yi ),
												.srcx = get_tileset_universal_x( 5.0f ),
												.srcy = get_tileset_universal_y( 1.0f ),
												.animation = 5.0f
											}
										);
										( ( money_data_t * )( tileref->data ) )->gfx3 = engine_add_tile
										(
											current_gfx_layer,
											( tile_gfx_args_t ) {
												.x = 16.0f * ( float )( xi ),
												.y = 16.0f * ( float )( yi ) + 8.0f,
												.srcx = get_tileset_universal_x( 10.0f ),
												.srcy = get_tileset_universal_y( 1.0f ),
												.animation = 5.0f
											}
										);
										( ( money_data_t * )( tileref->data ) )->gfx4 = engine_add_tile
										(
											current_gfx_layer,
											( tile_gfx_args_t ) {
												.x = 16.0f * ( float )( xi ) + 8.0f,
												.y = 16.0f * ( float )( yi ) + 8.0f,
												.srcx = get_tileset_universal_x( 15.0f ),
												.srcy = get_tileset_universal_y( 1.0f ),
												.animation = 5.0f
											}
										);
									}
								}
							}
							break;
							case ( OBJECT_TYPE_URBAN_GROUND ):
							{
								const uint16_t id = get_uint16_from_bytes( data.data, pointer );
								pointer += 2;
								const uint16_t x = get_uint16_from_bytes( data.data, pointer );
								pointer += 2;
								const uint16_t y = get_uint16_from_bytes( data.data, pointer );
								pointer += 2;
								const uint16_t width = get_uint16_from_bytes( data.data, pointer );
								pointer += 2;
								const uint8_t height = data.data[ pointer++ ];
								printf( "Urban Ground - ID: %u, X: %u, Y: %u, Width: %u, Height: %u\n", id, x, y, width, height );

								for ( uint8_t yi = y; yi < height + y; ++yi )
								{
									for ( uint16_t xi = x; xi < width + x; ++xi )
									{
										map.tiles[ ( yi * map.width ) + xi ] = create_solid_tile();

										if ( yi == y )
										{
											engine_add_tile
											(
												current_gfx_layer,
												( tile_gfx_args_t ) {
													.x = 16.0f * ( float )( xi ),
													.y = 16.0f * ( float )( yi ),
													.srcx = get_tileset_block_x( 0.0f ),
													.srcy = get_tileset_block_y( 0.0f )
												}
											);
											engine_add_tile
											(
												current_gfx_layer,
												( tile_gfx_args_t ) {
													.x = 16.0f * ( float )( xi ) + 8.0f,
													.y = 16.0f * ( float )( yi ),
													.srcx = get_tileset_block_x( 1.0f ),
													.srcy = get_tileset_block_y( 0.0f )
												}
											);
											engine_add_tile
											(
												current_gfx_layer,
												( tile_gfx_args_t ) {
													.x = 16.0f * ( float )( xi ),
													.y = 16.0f * ( float )( yi ) + 8.0f,
													.srcx = get_tileset_block_x( 2.0f ),
													.srcy = get_tileset_block_y( 0.0f )
												}
											);
											engine_add_tile
											(
												current_gfx_layer,
												( tile_gfx_args_t ) {
													.x = 16.0f * ( float )( xi ) + 8.0f,
													.y = 16.0f * ( float )( yi ) + 8.0f,
													.srcx = get_tileset_block_x( 2.0f ),
													.srcy = get_tileset_block_y( 0.0f )
												}
											);
										}
										else
										{
											engine_add_tile
											(
												current_gfx_layer,
												( tile_gfx_args_t ) {
													.x = 16.0f * ( float )( xi ),
													.y = 16.0f * ( float )( yi ),
													.srcx = get_tileset_block_x( 3.0f ),
													.srcy = get_tileset_block_y( 0.0f )
												}
											);
											engine_add_tile
											(
												current_gfx_layer,
												( tile_gfx_args_t ) {
													.x = 16.0f * ( float )( xi ) + 8.0f,
													.y = 16.0f * ( float )( yi ),
													.srcx = get_tileset_block_x( 3.0f ),
													.srcy = get_tileset_block_y( 0.0f )
												}
											);
											engine_add_tile
											(
												current_gfx_layer,
												( tile_gfx_args_t ) {
													.x = 16.0f * ( float )( xi ),
													.y = 16.0f * ( float )( yi ) + 8.0f,
													.srcx = get_tileset_block_x( 3.0f ),
													.srcy = get_tileset_block_y( 0.0f )
												}
											);
											engine_add_tile
											(
												current_gfx_layer,
												( tile_gfx_args_t ) {
													.x = 16.0f * ( float )( xi ) + 8.0f,
													.y = 16.0f * ( float )( yi ) + 8.0f,
													.srcx = get_tileset_block_x( 3.0f ),
													.srcy = get_tileset_block_y( 0.0f )
												}
											);
										}
									}
								}
							}
							break;
							case ( OBJECT_TYPE_URBAN_BUILDING ):
							{
								const uint16_t id = get_uint16_from_bytes( data.data, pointer );
								pointer += 2;
								const uint16_t x = get_uint16_from_bytes( data.data, pointer );
								pointer += 2;
								const uint16_t y = get_uint16_from_bytes( data.data, pointer );
								pointer += 2;
								const uint8_t w = data.data[ pointer++ ];
								const uint8_t h = data.data[ pointer++ ];
								const uint8_t door = data.data[ pointer++ ];
								printf( "Urban building: id=%u, x=%u, y=%u, w=%u, h=%u, door=%u\n", id, x, y, w, h, door );

								for ( uint8_t xi = x; xi < w + x; ++xi )
								{
									map.tiles[ ( y * map.width ) + xi ] = create_solid_top_tile();
								}

								const float ystart = 16.0f * ( float )( y );
								const float xstart = 16.0f * ( float )( x );
								const float yend = ystart + 16.0f * ( float )( h );
								const float xend = xstart + 16.0f * ( float )( w );
								const float doorx = xstart + 16.0f * ( float )( door );
								for ( float ypx = ystart; ypx < yend; ypx += 8.0f )
								{
									for ( float xpx = xstart; xpx < xend; xpx += 8.0f )
									{
										if
										(
											(
												ypx == yend - 8.0f
												|| ypx == yend - 16.0f
												|| ypx == yend - 24.0f
												|| ypx == yend - 32.0f
											)
											&&
											(
												xpx == doorx
												|| xpx == doorx + 8.0f
											)
										)
										{
											continue;
										}


										if ( ypx == ystart )
										{
											if ( xpx == xstart )
											{
												engine_add_tile
												(
													current_gfx_layer,
													( tile_gfx_args_t ) {
														.x = xpx,
														.y = ypx,
														.srcx = get_tileset_block_x( 22.0f ),
														.srcy = get_tileset_block_y( 0.0f )
													}
												);
											}
											else if ( xpx == xend - 8.0f )
											{
												engine_add_tile
												(
													current_gfx_layer,
													( tile_gfx_args_t ) {
														.x = xpx,
														.y = ypx,
														.srcx = get_tileset_block_x( 23.0f ),
														.srcy = get_tileset_block_y( 0.0f )
													}
												);
											}
											else
											{
												engine_add_tile
												(
													current_gfx_layer,
													( tile_gfx_args_t ) {
														.x = xpx,
														.y = ypx,
														.srcx = get_tileset_block_x( 18.0f ),
														.srcy = get_tileset_block_y( 0.0f )
													}
												);
											}
										}
										else if ( ypx == yend - 8.0f )
										{
											if ( xpx == xstart )
											{
												engine_add_tile
												(
													current_gfx_layer,
													( tile_gfx_args_t ) {
														.x = xpx,
														.y = ypx,
														.srcx = get_tileset_block_x( 24.0f ),
														.srcy = get_tileset_block_y( 0.0f )
													}
												);
											}
											else if ( xpx == xend - 8.0f )
											{
												engine_add_tile
												(
													current_gfx_layer,
													( tile_gfx_args_t ) {
														.x = xpx,
														.y = ypx,
														.srcx = get_tileset_block_x( 25.0f ),
														.srcy = get_tileset_block_y( 0.0f )
													}
												);
											}
											else
											{
												engine_add_tile
												(
													current_gfx_layer,
													( tile_gfx_args_t ) {
														.x = xpx,
														.y = ypx,
														.srcx = get_tileset_block_x( 19.0f ),
														.srcy = get_tileset_block_y( 0.0f )
													}
												);
											}
										}
										else
										{
											if ( xpx == xstart )
											{
												engine_add_tile
												(
													current_gfx_layer,
													( tile_gfx_args_t ) {
														.x = xpx,
														.y = ypx,
														.srcx = get_tileset_block_x( 20.0f ),
														.srcy = get_tileset_block_y( 0.0f )
													}
												);
											}
											else if ( xpx == xend - 8.0f )
											{
												engine_add_tile
												(
													current_gfx_layer,
													( tile_gfx_args_t ) {
														.x = xpx,
														.y = ypx,
														.srcx = get_tileset_block_x( 21.0f ),
														.srcy = get_tileset_block_y( 0.0f )
													}
												);
											}
											else
											{
												engine_add_tile
												(
													current_gfx_layer,
													( tile_gfx_args_t ) {
														.x = xpx,
														.y = ypx,
														.srcx = get_tileset_block_x( 17.0f ),
														.srcy = get_tileset_block_y( 0.0f )
													}
												);
											}
										}
									}
								}

								engine_add_tile
								(
									current_gfx_layer,
									( tile_gfx_args_t ) {
										.x = doorx,
										.y = yend - 32.0f,
										.srcx = get_tileset_block_x( 41.0f ),
										.srcy = get_tileset_block_y( 0.0f )
									}
								);
								engine_add_tile
								(
									current_gfx_layer,
									( tile_gfx_args_t ) {
										.x = doorx + 8.0f,
										.y = yend - 32.0f,
										.srcx = get_tileset_block_x( 42.0f ),
										.srcy = get_tileset_block_y( 0.0f )
									}
								);
								engine_add_tile
								(
									current_gfx_layer,
									( tile_gfx_args_t ) {
										.x = doorx,
										.y = yend - 24.0f,
										.srcx = get_tileset_block_x( 43.0f ),
										.srcy = get_tileset_block_y( 0.0f )
									}
								);
								engine_add_tile
								(
									current_gfx_layer,
									( tile_gfx_args_t ) {
										.x = doorx + 8.0f,
										.y = yend - 24.0f,
										.srcx = get_tileset_block_x( 44.0f ),
										.srcy = get_tileset_block_y( 0.0f )
									}
								);
								engine_add_tile
								(
									current_gfx_layer,
									( tile_gfx_args_t ) {
										.x = doorx,
										.y = yend - 16.0f,
										.srcx = get_tileset_block_x( 45.0f ),
										.srcy = get_tileset_block_y( 0.0f )
									}
								);
								engine_add_tile
								(
									current_gfx_layer,
									( tile_gfx_args_t ) {
										.x = doorx + 8.0f,
										.y = yend - 16.0f,
										.srcx = get_tileset_block_x( 46.0f ),
										.srcy = get_tileset_block_y( 0.0f )
									}
								);
								engine_add_tile
								(
									current_gfx_layer,
									( tile_gfx_args_t ) {
										.x = doorx,
										.y = yend - 8.0f,
										.srcx = get_tileset_block_x( 47.0f ),
										.srcy = get_tileset_block_y( 0.0f )
									}
								);
								engine_add_tile
								(
									current_gfx_layer,
									( tile_gfx_args_t ) {
										.x = doorx + 8.0f,
										.y = yend - 8.0f,
										.srcx = get_tileset_block_x( 48.0f ),
										.srcy = get_tileset_block_y( 0.0f )
									}
								);
							}
							break;
							default:
							{
								fprintf( stderr, "Unknown object type: %u for block layer type\n", object_type );
								exit( 1 );
							}
							break;
						}
					}
					break;
					case LAYER_TYPE_SPRITE:
					{
						switch ( object_type )
						{
							default:
							{
								fprintf( stderr, "Unknown object type: %u for sprite layer type\n", object_type );
								exit( 1 );
							}
							break;
						}
					}
					break;
					default:
					{
						fprintf( stderr, "Invalid layer type for gathering object data: %u\n", layer_type );
						exit( 1 );
					}
				}
				state = LAYER_PARSING_OBJECT_TYPE;
			}
			break;
		}
	}

	return map;
}

unsigned char * data_get_charset_gfx_data()
{
	const size_t after_palette_pointers = 2
		+ ( data_get_main_palette_count() * 4 )
		+ ( data_get_overworld_palette_count() * 4 )
		+ 8;
	const uint32_t pointer = get_uint32_from_bytes( data.data, after_palette_pointers );
	return data_get_gfx_data( pointer, 512, 576 );
}

unsigned char * data_get_tileset_block_gfx_data( unsigned int n )
{
	const size_t after_palette_pointers = 2
		+ ( data_get_main_palette_count() * 4 )
		+ ( data_get_overworld_palette_count() * 4 )
		+ 24;
	const uint32_t pointer = get_uint32_from_bytes( data.data, after_palette_pointers );
	return data_get_gfx_data( pointer, 512, 256 );
};

unsigned char * data_get_universal_block_gfx_data()
{
	const size_t after_palette_pointers = 2
		+ ( data_get_main_palette_count() * 4 )
		+ ( data_get_overworld_palette_count() * 4 )
		+ 20;
	const uint32_t pointer = get_uint32_from_bytes( data.data, after_palette_pointers );
	return data_get_gfx_data( pointer, 512, 64 );
}

unsigned char * data_get_sprite_gfx_data()
{
	const size_t after_palette_pointers = 2
		+ ( data_get_main_palette_count() * 4 )
		+ ( data_get_overworld_palette_count() * 4 )
		+ 16;
	const uint32_t pointer = get_uint32_from_bytes( data.data, after_palette_pointers );
	return data_get_gfx_data( pointer, 512, 768 );
}

size_t data_get_main_palette_count()
{
	return data.data[ 0 ];
}

unsigned char * data_get_main_palette_data()
{
	const size_t palette_count = data_get_main_palette_count();
	const size_t after_palette_name_pointers = 2 + palette_count * 4 + data_get_overworld_palette_count() * 4;
	const size_t palette_data_size = palette_count * 8;
	const size_t palette_length = 14;
	uint16_t * palette_data = calloc( palette_data_size, sizeof( uint16_t ) );
	uint32_t pointer = get_uint32_from_bytes( data.data, after_palette_name_pointers );
	for ( size_t i = 0; i < palette_count; ++i )
	{
		for ( size_t j = 0; j < 7; ++j )
		{
			uint16_t color = get_uint16_from_bytes( data.data, pointer + palette_length * i + j * 2 );
			palette_data[ 8 * i + j + 1 ] = color;
		}
	}
	return ( unsigned char * )( palette_data );
}

size_t data_get_overworld_palette_count()
{
	return data.data[ 1 ];
}

static void get_bits_from_byte( unsigned char byte, uint8_t * bits )
{
	for ( int i = 0; i < 8; i++ ) {
		bits[ i ] = ( byte >> ( 7-i ) ) & 1;
	}
}

static unsigned char * data_get_gfx_data( uint32_t pointer, size_t width, size_t height )
{
	const uint32_t block_gfx_compressed_len = get_uint32_from_bytes( data.data, pointer );
	size_t block_gfx_decompressed_len = ( size_t )( ceil( width * height * ( 3.0 / 8.0 ) ) );

	BrotliDecoderState * state = BrotliDecoderCreateInstance( NULL, NULL, NULL );
	unsigned char * decoded = ( unsigned char * )( malloc( block_gfx_decompressed_len) );
	BrotliDecoderResult result = BrotliDecoderDecompress( block_gfx_compressed_len, &data.data[ pointer + 4 ], &block_gfx_decompressed_len, decoded );
	if ( result != BROTLI_DECODER_RESULT_SUCCESS ) {
		printf( "Brotli decompression failed\n" );
		free( decoded );
		return NULL;
	}
	BrotliDecoderDestroyInstance( state );

	uint8_t total_bits[ width * height * 3 ];
	memset( total_bits, 0, block_gfx_decompressed_len );
	size_t total_bits_used = 0;
	unsigned char * pixels = ( unsigned char * )( malloc( width * height ) );
	memset( pixels, 0, width * height );
	uint8_t bits[ 10 ];
	memset( bits, 0, 10 );
	size_t pixels_used = 0;
	size_t bits_used = 0;
	size_t bits_index = 0;

	for ( size_t i = 0; i < block_gfx_decompressed_len; i++ ) {
		get_bits_from_byte( decoded[ i ], &bits[ bits_used ] );
		bits_used += 8;
		while ( bits_used >= 3 ) {
			total_bits[ total_bits_used++ ] = bits[ bits_index ];
			total_bits[ total_bits_used++ ] = bits[ bits_index+1 ];
			total_bits[ total_bits_used++ ] = bits[ bits_index+2 ];
			pixels[ pixels_used++ ] = ( bits[ bits_index++ ] << 2 ) | ( bits[ bits_index++ ] << 1 ) | bits[ bits_index++ ];
			bits_used -= 3;
		}
		for ( size_t j = 0; j < bits_used; j++ ) {
			bits[ j ] = bits[ bits_index++ ];
		}
		bits_index = 0;
	}

	if ( bits_used > 0 ) {
		printf( "Warning: %zu unused bits remaining after decoding\n", bits_used );
	}

	free( decoded );

	return pixels;
}

static float get_float_from_bytes( const unsigned char * data, size_t i )
{
	float value;
	const uint32_t as_uint = get_uint32_from_bytes( data, i );
	memcpy( &value, &as_uint, sizeof( float ) );
	return value;
}

static uint16_t get_uint16_from_bytes( const unsigned char * data, size_t i )
{
	return data[ i + 1 ] | ( data[ i ] << 8 );
}

static uint32_t get_uint32_from_bytes( const unsigned char * data, size_t i )
{
	return data[ i + 3 ] | ( data[ i + 2 ] << 8 ) | ( data[ i + 1 ] << 16 ) | ( data[ i ] << 24 );
}

static data_t open_file( const char * filename )
{
	data_t had = { 0, 0 };
	FILE * file = fopen( filename, "rb" );
	if ( ! file ) {
		perror( "Failed to open file" );
		return had;
	}
	fseek( file, 0, SEEK_END );
	long size = ftell( file );
	if ( size < 0 ) {
		perror( "Failed to determine file size" );
		fclose( file );
		return had;
	}
	char * buffer = malloc( size );
	if ( ! buffer ) {
		perror( "Failed to allocate memory" );
		fclose( file );
		return had;
	}
	fseek( file, 0, SEEK_SET );
	size_t bytesread = fread( buffer, 1, size, file );
	if ( bytesread < size ) {
		perror( "Failed to read file" );
		free( buffer );
		fclose( file );
		return had;
	}
	fclose( file );
	had.data = buffer;
	had.size = size;
	return had;
}
