#ifndef ENGINE_H
#define ENGINE_H

#include "camera.h"
#include <inttypes.h>
#include <stddef.h>

typedef struct pair_t
{
	float x, y;
}
pair_t;

typedef struct rect_t
{
	float x, y, w, h;
}
rect_t;

typedef struct color_t
{
	float r, g, b, a;
}
color_t;

typedef uint_fast16_t graphic_id_t;
typedef uint_fast16_t sprite_id_t;
typedef uint_fast16_t tile_id_t;

graphic_id_t engine_add_graphic( rect_t rect, color_t color );
sprite_id_t engine_add_sprite( rect_t pos, rect_t texcoords );
tile_id_t engine_add_tile( pair_t pos, pair_t texpos );
void engine_change_texture( const unsigned char * pixels );
float engine_get_ticks();
int engine_init( const char * title );
int engine_loop();
void engine_render( const camera_t * camera );
void engine_set_graphic_h( graphic_id_t graphic_id, float h );
void engine_set_graphic_x( graphic_id_t graphic_id, float x );
void engine_set_graphic_y( graphic_id_t graphic_id, float y );
void engine_set_palette_index( float index );
void engine_set_palettes( unsigned char * colors, size_t palette_count );
void engine_set_sprite_flip_x( sprite_id_t sprite_id, unsigned int flip_x );
void engine_set_sprite_src_h( sprite_id_t sprite_id, float h );
void engine_set_sprite_src_x( sprite_id_t sprite_id, float x );
void engine_set_sprite_h( sprite_id_t sprite_id, float h );
void engine_set_sprite_x( sprite_id_t sprite_id, float x );
void engine_set_sprite_y( sprite_id_t sprite_id, float y );
void engine_sleep( uint16_t ms );
unsigned int input_pressed_down();
unsigned int input_pressed_jump();
unsigned int input_pressed_left();
unsigned int input_pressed_right();
unsigned int input_pressed_run();
unsigned int input_pressed_up();

#endif // ENGINE_H