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

typedef uint_fast16_t rect_gfx_id_t;
typedef uint_fast16_t sprite_id_t;
typedef uint_fast16_t tile_id_t;

rect_gfx_id_t engine_add_rect( rect_t rect, unsigned int color );
sprite_id_t engine_add_sprite( rect_t pos, rect_t texcoords );
tile_id_t engine_add_tile( float x, float y, float srcx, float srcy );
void engine_change_bg_layer_texture
(
	const unsigned char * pixels,
	size_t width,
	size_t height,
	size_t map_width,
	size_t map_height,
	float scroll_x,
	float scroll_y
);
void engine_change_texture( const unsigned char * pixels );
float engine_get_ticks();
int engine_init( const char * title );
int engine_loop();
void engine_render( const camera_t * camera );
void engine_set_bg_color( float r, float g, float b, float a );
void engine_set_bg_color_gradient( unsigned int direction, color_t start, color_t end );
void engine_set_blur_zoom( float zoom );
void engine_set_rect_h( rect_gfx_id_t rect_id, float h );
void engine_set_rect_x( rect_gfx_id_t rect_id, float x );
void engine_set_rect_y( rect_gfx_id_t rect_id, float y );
void engine_set_palette_index( unsigned int index );
void engine_set_palettes( unsigned char * colors, size_t palette_count );
void engine_set_sprite_flip_x( sprite_id_t sprite_id, unsigned int flip_x );
void engine_set_sprite_src_h( sprite_id_t sprite_id, float h );
void engine_set_sprite_src_x( sprite_id_t sprite_id, float x );
void engine_set_sprite_h( sprite_id_t sprite_id, float h );
void engine_set_sprite_x( sprite_id_t sprite_id, float x );
void engine_set_sprite_y( sprite_id_t sprite_id, float y );
void engine_sleep( uint16_t ms );
void engine_update_bg_layer_offset( float x, float y );
unsigned int input_pressed_down();
unsigned int input_pressed_jump();
unsigned int input_pressed_left();
unsigned int input_pressed_right();
unsigned int input_pressed_run();
unsigned int input_pressed_up();

#endif // ENGINE_H