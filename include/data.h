#ifndef DATA_H
#define DATA_H

#include <stddef.h>
#include <stdint.h>

#include "map.h"

typedef struct data_t
{
	unsigned char * data;
	size_t size;
}
data_t;

typedef struct gfx_data_t
{
	unsigned char * pixels;
	size_t width;
	size_t height;
}
gfx_data_t;

typedef struct goal_data_t
{
	uint8_t goal_type;
	union
	{
		struct
		{
			uint32_t amount;
		}
		collect_pts;
	}
	data;
}
goal_data_t;

typedef struct level_data_t
{
	uint32_t * map_pointers;
	uint8_t map_count;
	goal_data_t goal;
}
level_data_t;

typedef struct map_data_t
{
	uint16_t w;
	uint16_t h;
	uint8_t palette;
	uint8_t tileset;
	uint8_t layer_count;
}
map_data_t;

unsigned int data_load();
gfx_data_t data_get_background_gfx_data( unsigned int n );
unsigned char * data_get_charset_gfx_data();
unsigned char * data_get_tileset_block_gfx_data( unsigned int n );
level_data_t data_get_level_data( unsigned int n );
map_t data_get_map_data( uint32_t pointer );
unsigned char * data_get_universal_block_gfx_data();
size_t data_get_main_palette_count();
unsigned char * data_get_main_palette_data();
size_t data_get_overworld_palette_count();
unsigned char * data_get_sprite_gfx_data();

#endif // DATA_H