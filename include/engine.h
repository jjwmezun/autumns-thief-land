#ifndef ENGINE_H
#define ENGINE_H

#include <inttypes.h>

typedef struct rect {
	float x, y, w, h;
} rect;

typedef struct color {
	float r, g, b, a;
} color;

typedef struct graphic {
	rect rect;
	color color;
} graphic;

typedef uint_fast16_t graphic_id_t;

graphic_id_t engine_add_graphic( rect rect, color color );
float engine_get_ticks();
int engine_init( const char * title );
int engine_loop();
void engine_render();
void engine_set_graphic_h( graphic_id_t graphic_id, float h );
void engine_set_graphic_x( graphic_id_t graphic_id, float x );
void engine_set_graphic_y( graphic_id_t graphic_id, float y );
unsigned int input_pressed_down();
unsigned int input_pressed_jump();
unsigned int input_pressed_left();
unsigned int input_pressed_right();
unsigned int input_pressed_run();
unsigned int input_pressed_up();

#endif // ENGINE_H