#ifndef DATA_H
#define DATA_H

#include <stddef.h>
#include <stdint.h>

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

unsigned int data_load();
gfx_data_t data_get_background_gfx_data( unsigned int n );
unsigned char * data_get_universal_block_gfx_data();
size_t data_get_main_palette_count();
unsigned char * data_get_main_palette_data();
size_t data_get_overworld_palette_count();
unsigned char * data_get_sprite_gfx_data();

#endif // DATA_H