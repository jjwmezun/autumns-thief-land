#include "config.h"
#include "dir.h"
#include "engine.h"
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <stdio.h>
#include "spotlight.h"
#include "util.h"

#define MAX_RECTS 10000
#define MAX_SPRITES 10000
#define MAX_TILES 10000
#define INVENTORY_TILE_COUNT ( 62 * 4 + 10 )
#define INVENTORY_SIZE sizeof( tile_graphic_t ) * INVENTORY_TILE_COUNT
#define TILE_WIDTH 8.0f / WINDOW_WIDTH_PIXELS_F
#define TILE_HEIGHT 8.0f / WINDOW_HEIGHT_PIXELS_F
#define INVENTORY_X 8.0f
#define INVENTORY_Y 248.0f
#define INVENTORY_RIGHT ( WINDOW_WIDTH_PIXELS_F - 8.0f )
#define INVENTORY_BOTTOM ( WINDOW_HEIGHT_PIXELS_F - 8.0f )
#define INVENTORY_PT_COUNT_START ( INVENTORY_X + 56.0f )

typedef struct rect_data_t
{
	rect_t abspos;
}
rect_data_t;

typedef struct sprite_data_t
{
	rect_t pos;
	rect_t texcoords;
}
sprite_data_t;

typedef struct tile_data_t
{
	pair_t pos;
	pair_t texpos;
}
tile_data_t;

typedef struct sprite_graphic_t
{
	rect_t rect;
	rect_t texcoords;
	pair_t flip;
}
sprite_graphic_t;

typedef struct rect_graphic_t
{
	rect_t rect;
	float color_index;
	float alpha;
}
rect_graphic_t;
typedef struct tile_graphic_t
{
	pair_t pos;
	pair_t texpos;
}
tile_graphic_t;

static float convert_graphic_h( float h );
static float convert_graphic_x( float w, float x );
static float convert_graphic_y( float h, float y );
static GLuint create_shader_program( const char * vertex_shader_src, const char * fragment_shader_src );
static float get_tile_x( float x );
static float get_tile_y( float y );
static float get_tile_srcx( float srcx );
static float get_tile_srcy( float srcy );
static void init_bg_color_renderer();
static void init_bg_layer_renderer();
static void init_blur();
static void init_framebuffers();
static void init_inventory();
static void init_rect_renderer();
static void init_spotlight();
static void init_sprite_renderer();
static void init_tile_renderer();
static void render_bg_color();
static void render_bg_layer( const camera_t * camera );
static void render_inventory();
static void render_rects( const camera_t * camera );
static void render_spotlight();
static void render_sprites( const camera_t * camera );
static void render_tiles( const camera_t * camera );
static void update_screen();
static void update_viewport();

static unsigned int magnification = 4;
static unsigned int screen_width;
static unsigned int screen_height;
static rect_gfx_id_t rects_count = 0;
static SDL_Window * window;
static GLuint fbtextures[ 2 ];
static GLuint fbo[ 2 ];
static GLuint rbo[ 2 ];
static GLuint fbprogram;
static GLuint fbvao;
static GLuint fb_texture_location;
static GLuint bg_color_program;
static GLuint bg_color_vao;
static GLuint bg_color_vbo;
static GLuint rect_program;
static GLuint rect_vao;
static GLuint rect_instances_vbo;
static rect_graphic_t rects[ MAX_RECTS ];
static rect_data_t rects_data[ MAX_RECTS ];
static GLuint rect_camera_location;
static GLuint rect_palette_index_location;
static GLuint rect_palette_texture_location;
static float palette_index = 0.0f;
static GLuint main_texture;
static GLuint palette_texture;
static GLuint sprite_program;
static GLuint sprite_vao;
static GLuint sprite_instances_vbo;
static GLuint sprite_palette_index_location;
static GLuint sprite_camera_location;
static sprite_id_t sprites_count = 0;
static sprite_graphic_t sprites[ MAX_SPRITES ];
static sprite_data_t sprites_data[ MAX_SPRITES ];
static GLuint tile_program;
static GLuint tile_vao;
static GLuint tile_instances_vbo;
static tile_graphic_t tiles[ MAX_TILES ];
static tile_data_t tiles_data[ MAX_TILES ];
static tile_id_t tiles_count = 0;
static GLuint tile_palette_index_location;
static GLuint tile_camera_location;
static GLuint bg_layer_texture;
static GLuint bg_layer_program;
static GLuint bg_layer_vao;
static GLuint bg_layer_palette_index_location;
static GLuint bg_layer_camera_location;
static GLuint bg_layer_model_location;
static GLuint bg_layer_texmodel_location;
static GLuint spotlight_program;
static GLuint spotlight_vao;
static GLuint spotlight_texture;
static GLuint spotlight_texture_index_location;
static GLuint spotlight_fbtexture_index_location;
static GLuint blur_program;
static GLuint blurvao;
static GLuint blur_texture_location;
static GLuint blur_zoom_location;
static tile_graphic_t inventory_tiles[ INVENTORY_TILE_COUNT ];
static float palette_count = 0.0f;
static float bg_layer_scroll_x = 0.0f;
static float bg_layer_scroll_y = 0.0f;
static float bg_layer_texture_width = 0.0f;
static float bg_layer_texture_height = 0.0f;
static float bg_layer_scale_x = 0.0f;
static float bg_layer_scale_y = 0.0f;
static float bg_layer_offset_x = -1000.0f;
static float bg_layer_offset_y = -32.0f;
static struct
{
	unsigned int up : 1;
	unsigned int down : 1;
	unsigned int left : 1;
	unsigned int right : 1;
	unsigned int jump : 1;
	unsigned int run : 1;
} pressed;

rect_gfx_id_t engine_add_rect( rect_t rect, unsigned int color )
{
	if ( rects_count >= MAX_RECTS )
	{
		fprintf( stderr, "Maximum number of rects reached.\n" );
		return 1;
	}

	rects_data[ rects_count ].abspos = rect;
	rect.w /= WINDOW_WIDTH_PIXELS_F;
	rect.h /= WINDOW_HEIGHT_PIXELS_F;
	rect.x = convert_graphic_x( rect.w, rect.x );
	rect.y = convert_graphic_y( rect.h, rect.y );
	rects[ rects_count ].rect = rect;
	rects[ rects_count ].color_index = ( float )( color ) / 8.0f;
	rects[ rects_count ].alpha = 0.5f;
	return rects_count++;
}

sprite_id_t engine_add_sprite( rect_t pos, rect_t texcoords )
{
	if ( sprites_count >= MAX_SPRITES )
	{
		fprintf( stderr, "Maximum number of sprites reached.\n" );
		return 1;
	}

	sprites_data[ sprites_count ].pos = pos;
	sprites_data[ sprites_count ].texcoords = texcoords;
	pos.w /= WINDOW_WIDTH_PIXELS_F;
	pos.h /= WINDOW_HEIGHT_PIXELS_F;
	pos.x = convert_graphic_x( pos.w, pos.x );
	pos.y = convert_graphic_y( pos.h, pos.y );
	sprites[ sprites_count ].rect = pos;
	texcoords.w /= 1024.0f;
	texcoords.h /= 1024.0f;
	texcoords.x /= 1024.0f;
	texcoords.y /= 1024.0f;
	sprites[ sprites_count ].texcoords = texcoords;
	sprites[ sprites_count ].flip = ( pair_t ){ 1.0f, 1.0f };
	return sprites_count++;
}

tile_id_t engine_add_tile( float x, float y, float srcx, float srcy )
{
	if ( tiles_count >= MAX_TILES )
	{
		fprintf( stderr, "Maximum number of tiles reached.\n" );
		return 1;
	}

	tiles_data[ tiles_count ].pos.x = x;
	tiles_data[ tiles_count ].pos.y = y;
	tiles[ tiles_count ].pos.x = get_tile_x( x );
	tiles[ tiles_count ].pos.y = get_tile_y( y );
	tiles[ tiles_count ].texpos.x = get_tile_srcx( srcx );
	tiles[ tiles_count ].texpos.y = get_tile_srcy( srcy );
	return tiles_count++;
}

void engine_change_bg_layer_texture
(
	const unsigned char * pixels,
	size_t width,
	size_t height,
	size_t map_width,
	size_t map_height,
	float scroll_x,
	float scroll_y
)
{
	glActiveTexture( GL_TEXTURE2 );
	if ( bg_layer_texture == 0 )
	{
		glGenTextures( 1, &bg_layer_texture );
	}
	glBindTexture( GL_TEXTURE_2D, bg_layer_texture );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, pixels );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

	glUseProgram( bg_layer_program );

	bg_layer_scroll_x = scroll_x;
	bg_layer_scroll_y = scroll_y;
	bg_layer_texture_width = ( float )( width );
	bg_layer_texture_height = ( float )( height );
	const float map_width_pixels = ( float )( map_width * 16 );
	const float map_height_pixels = ( float )( map_height * 16 );
	const float xmulti = MAX( map_width_pixels, WINDOW_WIDTH_PIXELS_F );
	const float ymulti = MAX( map_height_pixels, WINDOW_HEIGHT_PIXELS_F );
	const float xmulti2 = xmulti / WINDOW_WIDTH_PIXELS_F;
	const float ymulti2 = ymulti / WINDOW_HEIGHT_PIXELS_F;
	const float xmulti3 = 1.0f + xmulti2 * bg_layer_scroll_x;
	const float ymulti3 = 1.0f + ymulti2 * bg_layer_scroll_y;
	bg_layer_scale_x = xmulti3 * 2.0f;
	bg_layer_scale_y = ymulti3 * 2.0f;
	const float texscalex = WINDOW_WIDTH_PIXELS_F / bg_layer_texture_width * bg_layer_scale_x;
	const float texscaley = WINDOW_HEIGHT_PIXELS_F / bg_layer_texture_height * bg_layer_scale_y;

	const float texmodel[ 9 ] =
	{
		texscalex, 0.0f, 0.0f,
		0.0f, texscaley, 0.0f,
		0.0f, 0.0f, 1.0f,
	};
	glUniformMatrix3fv( bg_layer_texmodel_location, 1, GL_FALSE, texmodel );
}

void engine_change_texture( const unsigned char * pixels )
{
	glActiveTexture( GL_TEXTURE0 );
	if ( main_texture == 0 )
	{
		glGenTextures( 1, &main_texture );
	}
	glBindTexture( GL_TEXTURE_2D, main_texture );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RED, 1024, 1024, 0, GL_RED, GL_UNSIGNED_BYTE, pixels );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
}

float engine_get_ticks()
{
	return ( float )( SDL_GetTicks() );
}

int engine_init( const char * title )
{
	if ( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
		fprintf( stderr, "Could not initialize SDL: %s\n", SDL_GetError() );
		return 1;
	}
	window = SDL_CreateWindow( title, 100, 100, WINDOW_WIDTH_PIXELS * magnification, WINDOW_HEIGHT_PIXELS * magnification, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE );
	if ( ! window )
	{
		fprintf( stderr, "Could not create window: %s\n", SDL_GetError() );
		SDL_Quit();
		return 1;
	}
	SDL_GLContext context = SDL_GL_CreateContext( window );
	glewInit();

	// Set up viewport.
	screen_width = WINDOW_WIDTH_PIXELS * magnification;
	screen_height = WINDOW_HEIGHT_PIXELS * magnification;
	update_viewport();

	// Enable blending.
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	init_bg_color_renderer();
	init_rect_renderer();
	init_bg_layer_renderer();
	init_sprite_renderer();
	init_tile_renderer();
	init_spotlight();
	init_framebuffers();
	init_inventory();
	init_blur();

	// Don't draw back faces.
	glCullFace( GL_BACK );

	// Make sure vsync is off.
	SDL_GL_SetSwapInterval( 0 );
	return 0;
}

int engine_loop()
{
	SDL_Event event;
	while ( SDL_PollEvent( &event ) )
	{
		switch ( event.type )
		{
			case ( SDL_QUIT ):
				return 0;
			break;
			case SDL_WINDOWEVENT:
				// On window resize.
				if ( event.window.event == SDL_WINDOWEVENT_RESIZED )
				{
					screen_width = event.window.data1;
					screen_height = event.window.data2;
					update_screen();
				}
			break;
			case SDL_KEYDOWN:
				switch ( event.key.keysym.sym )
				{
					case SDLK_LEFT:
						pressed.left = 1;
					break;
					case SDLK_RIGHT:
						pressed.right = 1;
					break;
					case SDLK_UP:
						pressed.up = 1;
					break;
					case SDLK_DOWN:
						pressed.down = 1;
					break;
					case SDLK_z:
						pressed.jump = 1;
					break;
					case SDLK_x:
						pressed.run = 1;
					break;
					case SDLK_ESCAPE:
						return 0;
					break;
				}
			break;
			case SDL_KEYUP:
				switch ( event.key.keysym.sym )
				{
					case SDLK_LEFT:
						pressed.left = 0;
					break;
					case SDLK_RIGHT:
						pressed.right = 0;
					break;
					case SDLK_UP:
						pressed.up = 0;
					break;
					case SDLK_DOWN:
						pressed.down = 0;
					break;
					case SDLK_z:
						pressed.jump = 0;
					break;
					case SDLK_x:
						pressed.run = 0;
					break;
				}
			break;
		}
	}
	return 1;
}

void engine_render( const camera_t * camera )
{
	// Clear the screen.
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// Start rendering to framebuffer.
	glBindFramebuffer( GL_FRAMEBUFFER, fbo[ 0 ] );
	glViewport( 0, 0, WINDOW_WIDTH_PIXELS * magnification, WINDOW_HEIGHT_PIXELS * magnification );

	// Clear the screen.
	glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// Render elements to framebuffer.

	// Render elements with depth buffering 1st.
	render_sprites( camera );
	render_tiles( camera );
	render_bg_layer( camera );
	render_bg_color();

	// Render elements without depth buffering 2nd.
	render_rects( camera );

	// Start rendering 2nd framebuffer.
	glBindFramebuffer( GL_FRAMEBUFFER, fbo[ 1 ] );

	// Clear the screen.
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	if ( 1 )
	{
		// Render spotlight o’er framebuffer texture to main buffer.
		render_spotlight();
	}
	else
	{
		// Render framebuffer to main buffer.
		update_viewport();
		glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT );
		glUseProgram( fbprogram );
		glUniform1i( fb_texture_location, 3 );
		glActiveTexture( GL_TEXTURE3 );
		glBindTexture( GL_TEXTURE_2D, fbtextures[ 0 ] );
		glBindVertexArray( fbvao );
		glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );
	}

	render_inventory();

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	/*
	update_viewport();
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );
	glUseProgram( fbprogram );
	glUniform1i( fb_texture_location, 4 );
	glActiveTexture( GL_TEXTURE4 );
	glBindTexture( GL_TEXTURE_2D, fbtextures[ 1 ] );
	glBindVertexArray( fbvao );
	glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );*/
	glUseProgram( blur_program );
	glBindVertexArray( blurvao );
	glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );

	SDL_GL_SwapWindow( window );
}

void engine_set_bg_color( float r, float g, float b, float a )
{
	// Update vertices buffer.
	float vertices[] = {
		-1.0f, -1.0f, r, g, b, a, // Lower left
		 1.0f, -1.0f, r, g, b, a, // Lower right
		 1.0f,  1.0f, r, g, b, a, // Upper right
		-1.0f,  1.0f, r, g, b, a  // Upper left
	};
	glBindBuffer( GL_ARRAY_BUFFER, bg_color_vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );

	// Unbind VBO.
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void engine_set_bg_color_gradient( unsigned int direction, color_t start, color_t end )
{
	color_t tl = start;
	color_t tr = start;
	color_t bl = start;
	color_t br = start;

	switch ( direction )
	{
		case DIR_UP:
			tl = end;
			tr = end;
			bl = start;
			br = start;
		break;
		case DIR_UP_RIGHT:
			tl = start;
			tr = end;
			bl = start;
			br = start;
		break;
		case DIR_RIGHT:
			tl = start;
			tr = end;
			bl = start;
			br = end;
		break;
		case DIR_DOWN_RIGHT:
			tl = start;
			tr = start;
			bl = start;
			br = end;
		break;
		case DIR_DOWN:
			tl = start;
			tr = start;
			bl = end;
			br = end;
		break;
		case DIR_DOWN_LEFT:
			tl = start;
			tr = start;
			bl = end;
			br = start;
		break;
		case DIR_LEFT:
			tl = end;
			tr = start;
			bl = end;
			br = start;
		break;
		case DIR_UP_LEFT:
			tl = end;
			tr = start;
			bl = start;
			br = start;
		break;
	}

	// Update vertices buffer.
	float vertices[] = {
		-1.0f, -1.0f, bl.r, bl.g, bl.b, bl.a, // Lower left
		 1.0f, -1.0f, br.r, br.g, br.b, br.a, // Lower right
		 1.0f,  1.0f, tr.r, tr.g, tr.b, tr.a, // Upper right
		-1.0f,  1.0f, tl.r, tl.g, tl.b, tl.a  // Upper left
	};
	glBindBuffer( GL_ARRAY_BUFFER, bg_color_vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );

	// Unbind VBO.
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void engine_set_blur_zoom( float zoom )
{
	glUniform1f( blur_zoom_location, zoom );
}

void engine_set_palette_index( unsigned int index )
{
	palette_index = ( float )( index ) / palette_count;
}

void engine_set_palettes( unsigned char * colors, size_t count )
{
	glActiveTexture( GL_TEXTURE1 );
	if ( palette_texture == 0 )
	{
		glGenTextures( 1, &palette_texture );
	}
	glBindTexture( GL_TEXTURE_2D, palette_texture );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB5_A1, 8, count, 0, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, colors );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

	palette_count = ( float )( count );
}

void engine_set_rect_h( rect_gfx_id_t rect_id, float h )
{
	if ( rect_id >= rects_count )
	{
		fprintf( stderr, "Invalid rect ID: %lu\n", rect_id );
		return;
	}
	rects_data[ rect_id ].abspos.h = h;
	rects[ rect_id ].rect.h = convert_graphic_h( h );
	rects[ rect_id ].rect.y = convert_graphic_y
	(
		rects[ rect_id ].rect.h, rects_data[ rect_id ].abspos.y
	);
}

void engine_set_rect_x( rect_gfx_id_t rect_id, float x )
{
	if ( rect_id >= rects_count )
	{
		fprintf( stderr, "Invalid rect ID: %lu\n", rect_id );
		return;
	}
	rects_data[ rect_id ].abspos.x = x;
	rects[ rect_id ].rect.x = convert_graphic_x( rects[ rect_id ].rect.w, x );
}

void engine_set_rect_y( rect_gfx_id_t rect_id, float y )
{
	if ( rect_id >= rects_count )
	{
		fprintf( stderr, "Invalid rect ID: %lu\n", rect_id );
		return;
	}
	rects_data[ rect_id ].abspos.y = y;
	rects[ rect_id ].rect.y = convert_graphic_y( rects[ rect_id ].rect.h, y );
}

void engine_set_sprite_flip_x( sprite_id_t sprite_id, unsigned int flip_x )
{
	sprites[ sprite_id ].flip.x = flip_x ? -1.0f : 1.0f;
}

void engine_set_sprite_src_h( sprite_id_t sprite_id, float h )
{
	if ( sprite_id >= sprites_count )
	{
		fprintf( stderr, "Invalid sprite ID: %lu\n", sprite_id );
		return;
	}
	sprites_data[ sprite_id ].texcoords.h = h;
	sprites[ sprite_id ].texcoords.h = h / 1024.0f;
}

void engine_set_sprite_src_x( sprite_id_t sprite_id, float x )
{
	if ( sprite_id >= sprites_count )
	{
		fprintf( stderr, "Invalid sprite ID: %lu\n", sprite_id );
		return;
	}
	sprites_data[ sprite_id ].texcoords.x = x;
	sprites[ sprite_id ].texcoords.x = x / 1024.0f;
}

void engine_set_sprite_h( sprite_id_t sprite_id, float h )
{
	if ( sprite_id >= sprites_count )
	{
		fprintf( stderr, "Invalid sprite ID: %lu\n", sprite_id );
		return;
	}
	sprites_data[ sprite_id ].pos.h = h;
	sprites[ sprite_id ].rect.h = convert_graphic_h( h );
	sprites[ sprite_id ].rect.y = convert_graphic_y
	(
		sprites[ sprite_id ].rect.h, sprites_data[ sprite_id ].pos.y
	);
}

void engine_set_sprite_x( sprite_id_t sprite_id, float x )
{
	if ( sprite_id >= sprites_count )
	{
		fprintf( stderr, "Invalid sprite ID: %lu\n", sprite_id );
		return;
	}
	sprites_data[ sprite_id ].pos.x = x;
	sprites[ sprite_id ].rect.x = convert_graphic_x( sprites[ sprite_id ].rect.w, x );
}

void engine_set_sprite_y( sprite_id_t sprite_id, float y )
{
	if ( sprite_id >= sprites_count )
	{
		fprintf( stderr, "Invalid sprite ID: %lu\n", sprite_id );
		return;
	}
	sprites_data[ sprite_id ].pos.y = y;
	sprites[ sprite_id ].rect.y = convert_graphic_y( sprites[ sprite_id ].rect.h, y );
}

void engine_sleep( uint16_t ms )
{
	SDL_Delay( ms );
}

void engine_update_bg_layer_offset( float x, float y )
{
	bg_layer_offset_x = fmod( bg_layer_offset_x + x, bg_layer_texture_width );
	bg_layer_offset_y = fmod( bg_layer_offset_y + y, bg_layer_texture_height );
}

unsigned int input_pressed_down()
{
	return pressed.down;
}

unsigned int input_pressed_jump()
{
	return pressed.jump;
}

unsigned int input_pressed_left()
{
	return pressed.left;
}

unsigned int input_pressed_right()
{
	return pressed.right;
}

unsigned int input_pressed_run()
{
	return pressed.run;
}

unsigned int input_pressed_up()
{
	return pressed.up;
}

static float convert_graphic_h( float h )
{
	return h /= WINDOW_HEIGHT_PIXELS_F;
}

static float convert_graphic_x( float w, float x )
{
	return ( ( x / WINDOW_WIDTH_PIXELS_F - 0.5f ) * 2.0f + w );
}

static float convert_graphic_y( float h, float y )
{
	return ( ( ( y / WINDOW_HEIGHT_PIXELS_F - 0.5f ) * 2.0f + h ) * -1.0f );
}

static GLuint create_shader_program( const char * vertex_shader_src, const char * fragment_shader_src )
{
	GLuint vertex_shader = glCreateShader( GL_VERTEX_SHADER );
	glShaderSource( vertex_shader, 1, &vertex_shader_src, NULL );
	glCompileShader( vertex_shader );
	GLuint fragment_shader = glCreateShader( GL_FRAGMENT_SHADER );
	glShaderSource( fragment_shader, 1, &fragment_shader_src, NULL );
	glCompileShader( fragment_shader );
	GLuint program = glCreateProgram();
	glAttachShader( program, vertex_shader );
	glAttachShader( program, fragment_shader );
	glLinkProgram( program );

    // Test shader program linking was successful.
    int success;
    char log[ 512 ];
    glGetProgramiv( program, GL_LINK_STATUS, &success );
    if ( !success )
    {
        glGetProgramInfoLog( program, 512, NULL, log );
        printf( "Shader program linking failed! %s\n", log );
    }
	return program;
}

static float get_tile_x( float x )
{
	return convert_graphic_x( TILE_WIDTH, x );
}

static float get_tile_y( float y )
{
	return convert_graphic_y( TILE_HEIGHT, y );
}

static float get_tile_srcx( float srcx )
{
	return srcx / 1024.0f;
}

static float get_tile_srcy( float srcy )
{
	return srcy / 1024.0f;
}

static void init_bg_color_renderer()
{
	const char * vertex_shader_src = "#version 330\n"
		"layout(location = 0) in vec2 i_position;\n"
		"layout(location = 1) in vec4 i_color;\n"
		"\n"
		"uniform mat3 u_model;\n"
		"\n"
		"out vec4 o_color;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	vec3 pos = vec3( i_position, 1.0 ) * u_model;\n"
		"	gl_Position = vec4( pos.xy, 0.0, 1.0 );\n"
		"	o_color = i_color;\n"
		"}\n";

	const char * fragment_shader_src = "#version 330\n"
		"in vec4 o_color;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	gl_FragColor = o_color;\n"
		"}\n";
	
	bg_color_program = create_shader_program( vertex_shader_src, fragment_shader_src );

	glUseProgram( bg_color_program );

	float vertices[] = {
		-1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, // Lower left
		 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, // Lower right
		 1.0f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f, // Upper right
		-1.0f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f  // Upper left
	};

	int indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	glGenVertexArrays( 1, &bg_color_vao );
	glBindVertexArray( bg_color_vao );
	glGenBuffers( 1, &bg_color_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, bg_color_vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );
	glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof( float ), 0 );
	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof( float ), ( void * )( 2 * sizeof( float ) ) );
	glEnableVertexAttribArray( 1 );
	GLuint ebo;
	glGenBuffers( 1, &ebo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( indices ), indices, GL_STATIC_DRAW );

	// Set up model uniform.
	const GLuint bg_color_model_location = glGetUniformLocation( bg_color_program, "u_model" );
	glUniformMatrix3fv( bg_color_model_location, 1, GL_FALSE, ( const GLfloat[] ){
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	} );
}

static void init_bg_layer_renderer()
{
	const char * vertex_shader_src = "#version 330\n"
		"layout(location = 0) in vec2 i_position;\n"
		"layout(location = 1) in vec2 i_texture_coords;\n"
		"\n"
		"uniform mat3 u_camera;\n"
		"uniform mat3 u_model;\n"
		"uniform mat3 u_texmodel;\n"
		"\n"
		"out vec2 o_texture_coords;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	vec3 pos = vec3( i_position, 1.0 ) * u_model * u_camera;\n"
		"	gl_Position = vec4( pos.xy, 0.0, 1.0 );\n"
		"	vec3 tex = vec3( i_texture_coords, 1.0 ) * u_texmodel;\n"
		"	o_texture_coords = tex.xy;\n"
		"}\n";

	const char * fragment_shader_src = "#version 330\n"
		"\n"
		"in vec2 o_texture_coords;\n"
		"\n"
		"uniform sampler2D u_texture;\n"
		"uniform sampler2D u_palette_texture;\n"
		"uniform float u_palette_index;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	float color_index = texture( u_texture, o_texture_coords ).r;\n"
		"	if ( color_index == 0.0f )\n"
		"	{\n"
		"		discard;\n"
		"		return;\n"
		"	}\n"
		"	gl_FragColor = texture(\n"
		"		u_palette_texture,\n"
		"		vec2( color_index, u_palette_index )\n"
		"	);\n"
		"}\n";
	
	bg_layer_program = create_shader_program( vertex_shader_src, fragment_shader_src );

	glUseProgram( bg_layer_program );

	float vertices[] = {
		-1.0f, -1.0f, 0.0f, 1.0f, // Lower left
		1.0f, -1.0f, 1.0f, 1.0f,  // Lower right
		1.0f, 1.0f, 1.0f, 0.0f,   // Upper right
		-1.0f, 1.0f, 0.0f, 0.0f,  // Upper left
	};

	int indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	glGenVertexArrays( 1, &bg_layer_vao );
	glBindVertexArray( bg_layer_vao );
	GLuint vbo;
	glGenBuffers( 1, &vbo );
	glBindBuffer( GL_ARRAY_BUFFER, vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );
	glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof( float ), 0 );
	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof( float ), ( void * )( 2 * sizeof( float ) ) );
	glEnableVertexAttribArray( 1 );
	GLuint ebo;
	glGenBuffers( 1, &ebo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( indices ), indices, GL_STATIC_DRAW );

	GLuint bg_layer_u_texture_location = glGetUniformLocation( bg_layer_program, "u_texture" );
	glUniform1i( bg_layer_u_texture_location, 2 );
	GLuint bg_layer_u_palette_texture_location = glGetUniformLocation( bg_layer_program, "u_palette_texture" );
	glUniform1i( bg_layer_u_palette_texture_location, 1 );
	bg_layer_palette_index_location = glGetUniformLocation( bg_layer_program, "u_palette_index" );
	glUniform1f( bg_layer_palette_index_location, 0.0f );

	// Set up camera uniform.
	bg_layer_camera_location = glGetUniformLocation( bg_layer_program, "u_camera" );
	glUniformMatrix3fv( bg_layer_camera_location, 1, GL_FALSE, ( const GLfloat[] ){
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	} );

	// Set up model uniform.
	bg_layer_model_location = glGetUniformLocation( bg_layer_program, "u_model" );
	glUniformMatrix3fv( bg_layer_model_location, 1, GL_FALSE, ( const GLfloat[] ){
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	} );

	// Set up texture model uniform.
	bg_layer_texmodel_location = glGetUniformLocation( bg_layer_program, "u_texmodel" );
	glUniformMatrix3fv( bg_layer_texmodel_location, 1, GL_FALSE, ( const GLfloat[] ){
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	} );
}

static void init_blur()
{
	// Init framebuffer.
	const char * vertex_shader =
		"#version 330\n"
		"layout(location = 0) in vec2 a_position;\n"
		"layout(location = 1) in vec2 a_texcoord;\n"
		"\n"
		"out vec2 o_texcoord;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	gl_Position = vec4( a_position, 0.0, 1.0 );\n"
		"	o_texcoord = a_texcoord;\n"
		"}\n";
	const char * fragment_shader =
		"#version 330\n"
		"\n"
		"in vec2 o_texcoord;\n"
		"\n"
		"uniform sampler2D u_texture;\n"
		"uniform float u_zoom;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	float cx = u_zoom == 1.0 ? o_texcoord.x : floor( o_texcoord.x * 512.0 / u_zoom ) / 512.0 * u_zoom;\n"
		"	float cy = u_zoom == 1.0 ? o_texcoord.y : floor( o_texcoord.y * 288.0 / u_zoom ) / 288.0 * u_zoom;\n"
		"	gl_FragColor = texture( u_texture, vec2( cx, cy ) ) + 0.1 * ( u_zoom - 1.0 );\n"
		"}\n";
	blur_program = create_shader_program( vertex_shader, fragment_shader );
	glUseProgram( blur_program );

	float vertices[] = {
		-1.0f, -1.0f, 0.0f, 0.0f, // Lower left
		1.0f, -1.0f, 1.0f, 0.0f,  // Lower right
		1.0f, 1.0f, 1.0f, 1.0f,   // Upper right
		-1.0f, 1.0f, 0.0f, 1.0f,  // Upper left
	};

	int indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	glGenVertexArrays( 1, &blurvao );
	glBindVertexArray( blurvao );
	GLuint vbo;
	glGenBuffers( 1, &vbo );
	glBindBuffer( GL_ARRAY_BUFFER, vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );
	glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof( float ), 0 );
	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof( float ), ( void * )( 2 * sizeof( float ) ) );
	glEnableVertexAttribArray( 1 );
	GLuint ebo;
	glGenBuffers( 1, &ebo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( indices ), indices, GL_STATIC_DRAW );

	blur_texture_location = glGetUniformLocation( blur_program, "u_texture" );
	glUniform1i( blur_texture_location, 4 );
	blur_zoom_location = glGetUniformLocation( blur_program, "u_zoom" );
	glUniform1f( blur_zoom_location, 1.0f );

	glUseProgram( 0 );
}

static void init_framebuffers()
{
	// Init shaders.
	const char * fb_vertex_shader =
		"#version 330\n"
		"layout(location = 0) in vec2 a_position;\n"
		"layout(location = 1) in vec2 a_texcoord;\n"
		"\n"
		"out vec2 o_texcoord;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	gl_Position = vec4( a_position, 0.0, 1.0 );\n"
		"	o_texcoord = a_texcoord;\n"
		"}\n";
	const char * fb_fragment_shader =
		"#version 330\n"
		"\n"
		"in vec2 o_texcoord;\n"
		"\n"
		"uniform sampler2D u_texture;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	gl_FragColor = texture( u_texture, o_texcoord );\n"
		"}\n";
	fbprogram = create_shader_program( fb_vertex_shader, fb_fragment_shader );
	glUseProgram( fbprogram );

	// Init vertex data.
	float vertices[] = {
		-1.0f, -1.0f, 0.0f, 0.0f, // Lower left
		1.0f, -1.0f, 1.0f, 0.0f,  // Lower right
		1.0f, 1.0f, 1.0f, 1.0f,   // Upper right
		-1.0f, 1.0f, 0.0f, 1.0f,  // Upper left
	};

	int indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	glGenVertexArrays( 1, &fbvao );
	glBindVertexArray( fbvao );
	GLuint vbo;
	glGenBuffers( 1, &vbo );
	glBindBuffer( GL_ARRAY_BUFFER, vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );
	glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof( float ), 0 );
	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof( float ), ( void * )( 2 * sizeof( float ) ) );
	glEnableVertexAttribArray( 1 );
	GLuint ebo;
	glGenBuffers( 1, &ebo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( indices ), indices, GL_STATIC_DRAW );

	fb_texture_location = glGetUniformLocation( fbprogram, "u_texture" );

	// Init framebuffers, renderbuffers ( for depth testing ) & textures.
	glGenFramebuffers( 2, fbo );
	glGenRenderbuffers( 2, rbo );
	glGenTextures( 2, fbtextures );
	glBindFramebuffer( GL_FRAMEBUFFER, fbo[ 0 ] );
	glActiveTexture( GL_TEXTURE3 );
	glBindTexture( GL_TEXTURE_2D, fbtextures[ 0 ] );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, WINDOW_WIDTH_PIXELS * magnification, WINDOW_HEIGHT_PIXELS * magnification, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbtextures[ 0 ], 0 );
	glBindRenderbuffer( GL_RENDERBUFFER, rbo[ 0 ] );
	glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WINDOW_WIDTH_PIXELS * magnification, WINDOW_HEIGHT_PIXELS * magnification );
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo[ 0 ] );
	if( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
	{
		printf( "Error generating framebuffer.\n" );
	}
	glBindFramebuffer( GL_FRAMEBUFFER, fbo[ 1 ] );
	glActiveTexture( GL_TEXTURE4 );
	glBindTexture( GL_TEXTURE_2D, fbtextures[ 1 ] );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, WINDOW_WIDTH_PIXELS * magnification, WINDOW_HEIGHT_PIXELS * magnification, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbtextures[ 1 ], 0 );
	glBindRenderbuffer( GL_RENDERBUFFER, rbo[ 1 ] );
	glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WINDOW_WIDTH_PIXELS * magnification, WINDOW_HEIGHT_PIXELS * magnification );
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo[ 1 ] );
	if( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
	{
		printf( "Error generating framebuffer.\n" );
	}
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	glUseProgram( 0 );
}

static void init_inventory()
{
	size_t i = 0;

	// HP icon.
	inventory_tiles[ i ].pos.x = get_tile_x( INVENTORY_X + 8.0f );
	inventory_tiles[ i ].pos.y = get_tile_y( INVENTORY_Y + 8.0f );
	inventory_tiles[ i ].texpos.x = get_tile_srcx( 54.0f * 8.0f );
	inventory_tiles[ i ].texpos.y = get_tile_srcy( 7.0f * 8.0f );
	++i;

	// HP %
	inventory_tiles[ i ].pos.x = get_tile_x( INVENTORY_X + 16.0f );
	inventory_tiles[ i ].pos.y = get_tile_y( INVENTORY_Y + 8.0f );
	inventory_tiles[ i ].texpos.x = get_tile_srcx( 7.0f * 8.0f );
	inventory_tiles[ i ].texpos.y = get_tile_srcy( 8.0f * 8.0f );
	++i;
	inventory_tiles[ i ].pos.x = get_tile_x( INVENTORY_X + 24.0f );
	inventory_tiles[ i ].pos.y = get_tile_y( INVENTORY_Y + 8.0f );
	inventory_tiles[ i ].texpos.x = get_tile_srcx( 5.0f * 8.0f );
	inventory_tiles[ i ].texpos.y = get_tile_srcy( 8.0f * 8.0f );
	++i;
	inventory_tiles[ i ].pos.x = get_tile_x( INVENTORY_X + 32.0f );
	inventory_tiles[ i ].pos.y = get_tile_y( INVENTORY_Y + 8.0f );
	inventory_tiles[ i ].texpos.x = get_tile_srcx( 43.0f * 8.0f );
	inventory_tiles[ i ].texpos.y = get_tile_srcy( 8.0f * 8.0f );
	++i;

	// ₧ char.
	inventory_tiles[ i ].pos.x = get_tile_x( INVENTORY_X + 48.0f );
	inventory_tiles[ i ].pos.y = get_tile_y( INVENTORY_Y + 8.0f );
	inventory_tiles[ i ].texpos.x = get_tile_srcx( 4.0f * 8.0f );
	inventory_tiles[ i ].texpos.y = get_tile_srcy( 9.0f * 8.0f );
	++i;

	// ₧ count.
	for ( size_t x = INVENTORY_PT_COUNT_START; x < INVENTORY_PT_COUNT_START + ( 5 * 8.0f ); x += 8.0f )
	{
		inventory_tiles[ i ].pos.x = get_tile_x( x );
		inventory_tiles[ i ].pos.y = get_tile_y( INVENTORY_Y + 8.0f );
		inventory_tiles[ i ].texpos.x = get_tile_srcx( 0.0f * 8.0f );
		inventory_tiles[ i ].texpos.y = get_tile_srcy( 8.0f * 8.0f );
		++i;
	}

	// Inside box.
	for ( size_t y = INVENTORY_Y + 8.0f; y < INVENTORY_BOTTOM - 8.0f; y += 8.0f )
	{
		for ( size_t x = INVENTORY_X + 8.0f; x < INVENTORY_RIGHT - 8.0f; x += 8.0f )
		{
			inventory_tiles[ i ].pos.x = get_tile_x( x );
			inventory_tiles[ i ].pos.y = get_tile_y( y );
			inventory_tiles[ i ].texpos.x = get_tile_srcx( 59.0f * 8.0f );
			inventory_tiles[ i ].texpos.y = get_tile_srcy( 7.0f * 8.0f );
			++i;
		}
	}

	// Top left corner.
	inventory_tiles[ i ].pos.x = get_tile_x( INVENTORY_X );
	inventory_tiles[ i ].pos.y = get_tile_y( INVENTORY_Y );
	inventory_tiles[ i ].texpos.x = get_tile_srcx( 55.0f * 8.0f );
	inventory_tiles[ i ].texpos.y = get_tile_srcy( 7.0f * 8.0f );
	++i;

	// Top right corner.
	inventory_tiles[ i ].pos.x = get_tile_x( INVENTORY_RIGHT - 8.0f );
	inventory_tiles[ i ].pos.y = get_tile_y( INVENTORY_Y );
	inventory_tiles[ i ].texpos.x = get_tile_srcx( 57.0f * 8.0f );
	inventory_tiles[ i ].texpos.y = get_tile_srcy( 7.0f * 8.0f );
	++i;

	// Bottom left corner.
	inventory_tiles[ i ].pos.x = get_tile_x( INVENTORY_X );
	inventory_tiles[ i ].pos.y = get_tile_y( INVENTORY_BOTTOM - 8.0f );
	inventory_tiles[ i ].texpos.x = get_tile_srcx( 61.0f * 8.0f );
	inventory_tiles[ i ].texpos.y = get_tile_srcy( 7.0f * 8.0f );
	++i;

	// Bottom right corner.
	inventory_tiles[ i ].pos.x = get_tile_x( INVENTORY_RIGHT - 8.0f );
	inventory_tiles[ i ].pos.y = get_tile_y( INVENTORY_BOTTOM - 8.0f );
	inventory_tiles[ i ].texpos.x = get_tile_srcx( 63.0f * 8.0f );
	inventory_tiles[ i ].texpos.y = get_tile_srcy( 7.0f * 8.0f );
	++i;

	// Top edge tiles.
	for ( size_t x = INVENTORY_X + 8.0f; x < INVENTORY_RIGHT - 8.0f; x += 8.0f )
	{
		inventory_tiles[ i ].pos.x = get_tile_x( x );
		inventory_tiles[ i ].pos.y = get_tile_y( INVENTORY_Y );
		inventory_tiles[ i ].texpos.x = get_tile_srcx( 56.0f * 8.0f );
		inventory_tiles[ i ].texpos.y = get_tile_srcy( 7.0f * 8.0f );
		++i;
	}

	// Bottom edge tiles.
	for ( size_t x = INVENTORY_X + 8.0f; x < INVENTORY_RIGHT - 8.0f; x += 8.0f )
	{
		inventory_tiles[ i ].pos.x = get_tile_x( x );
		inventory_tiles[ i ].pos.y = get_tile_y( INVENTORY_BOTTOM - 8.0f );
		inventory_tiles[ i ].texpos.x = get_tile_srcx( 62.0f * 8.0f );
		inventory_tiles[ i ].texpos.y = get_tile_srcy( 7.0f * 8.0f );
		++i;
	}

	// Left edge tiles.
	for ( size_t y = INVENTORY_Y + 8.0f; y < INVENTORY_BOTTOM - 8.0f; y += 8.0f )
	{
		inventory_tiles[ i ].pos.x = get_tile_x( INVENTORY_X );
		inventory_tiles[ i ].pos.y = get_tile_y( y );
		inventory_tiles[ i ].texpos.x = get_tile_srcx( 58.0f * 8.0f );
		inventory_tiles[ i ].texpos.y = get_tile_srcy( 7.0f * 8.0f );
		++i;
	}

	// Right edge tiles.
	for ( size_t y = INVENTORY_Y + 8.0f; y < INVENTORY_BOTTOM - 8.0f; y += 8.0f )
	{
		inventory_tiles[ i ].pos.x = get_tile_x( INVENTORY_RIGHT - 8.0f );
		inventory_tiles[ i ].pos.y = get_tile_y( y );
		inventory_tiles[ i ].texpos.x = get_tile_srcx( 60.0f * 8.0f );
		inventory_tiles[ i ].texpos.y = get_tile_srcy( 7.0f * 8.0f );
		++i;
	}

	printf( "Inventory tiles initialized: %zu, %u\n", i, INVENTORY_TILE_COUNT );
}

static void init_rect_renderer()
{
	const char * vertex_shader_src = "#version 330\n"
		"layout(location = 0) in vec2 position;\n"
		"layout(location = 1) in vec4 rect;\n"
		"layout(location = 2) in float i_color_index;\n"
		"layout(location = 3) in float i_alpha;\n"
		"\n"
		"uniform vec2 u_camera;\n"
		"\n"
		"out float o_color_index;\n"
		"out float o_alpha;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	mat3 model = mat3(\n"
		"		rect.z, 0.0, rect.x,\n"
		"		0.0, rect.w, rect.y,\n"
		"		0.0, 0.0, 1.0\n"
		"	);\n"
		"	mat3 cam = mat3(\n"
		"		1.0, 0.0, -u_camera.x,\n"
		"		0.0, 1.0, u_camera.y,\n"
		"		0.0, 0.0, 1.0\n"
		"	);\n"
		"	vec3 pos = vec3( position, 1.0 ) * model * cam;\n"
		"	gl_Position = vec4( pos, 1.0 );\n"
		"	o_color_index = i_color_index;\n"
		"	o_alpha = i_alpha;\n"
		"}\n";
	
	const char * fragment_shader_src = "#version 330\n"
		"\n"
		"in float o_color_index;\n"
		"in float o_alpha;\n"
		"\n"
		"uniform sampler2D u_palette_texture;\n"
		"uniform float u_palette_index;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	if ( o_color_index == 0.0f )\n"
		"	{\n"
		"		discard;\n"
		"		return;\n"
		"	}\n"
		"	vec3 color = texture(\n"
		"		u_palette_texture,\n"
		"		vec2( o_color_index, u_palette_index )\n"
		"	).rgb;\n"
		"	gl_FragColor = vec4( color, o_alpha );\n"
		"}\n";
	
	rect_program = create_shader_program( vertex_shader_src, fragment_shader_src );

	glUseProgram( rect_program );

	float vertices[] = {
		-1.0f, -1.0f, // Lower left
		1.0f, -1.0f,  // Lower right
		1.0f, 1.0f,   // Upper right
		-1.0f, 1.0f,  // Upper left
	};

	int indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	glGenVertexArrays( 1, &rect_vao );
	glBindVertexArray( rect_vao );
	GLuint vbo;
	glGenBuffers( 1, &vbo );
	glBindBuffer( GL_ARRAY_BUFFER, vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );
	glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, 0 );
	glEnableVertexAttribArray( 0 );
	GLuint ebo;
	glGenBuffers( 1, &ebo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( indices ), indices, GL_STATIC_DRAW );

	glGenBuffers( 1, &rect_instances_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, rect_instances_vbo );
	glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, sizeof( rect_graphic_t ), 0 );
	glEnableVertexAttribArray( 1 );
	glVertexAttribDivisor( 1, 1 );
	glVertexAttribPointer( 2, 1, GL_FLOAT, GL_FALSE, sizeof( rect_graphic_t ), ( void * )( sizeof( rect_t ) ) );
	glEnableVertexAttribArray( 2 );
	glVertexAttribDivisor( 2, 1 );
	glVertexAttribPointer( 3, 1, GL_FLOAT, GL_FALSE, sizeof( rect_graphic_t ), ( void * )( sizeof( rect_t ) + sizeof( float ) ) );
	glEnableVertexAttribArray( 3 );
	glVertexAttribDivisor( 3, 1 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

	// Set up rect uniforms.
	rect_camera_location = glGetUniformLocation( rect_program, "u_camera" );
	glUniform2f( rect_camera_location, 0.0f, 0.0f );
	GLuint rect_palette_texture_location = glGetUniformLocation( rect_program, "u_palette_texture" );
	glUniform1i( rect_palette_texture_location, 1 );
	rect_palette_index_location = glGetUniformLocation( rect_program, "u_palette_index" );
}

static void init_spotlight()
{
	// Init framebuffer.
	const char * fb_vertex_shader =
		"#version 330\n"
		"layout(location = 0) in vec2 a_position;\n"
		"layout(location = 1) in vec2 a_texcoord;\n"
		"\n"
		"out vec2 o_texcoord;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	gl_Position = vec4( a_position, 0.0, 1.0 );\n"
		"	o_texcoord = a_texcoord;\n"
		"}\n";
	const char * fb_fragment_shader =
		"#version 330\n"
		"\n"
		"in vec2 o_texcoord;\n"
		"\n"
		"uniform sampler2D u_texture;\n"
		"uniform sampler2D u_fbtexture;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	float lighting = texture( u_texture, o_texcoord ).r;\n"
		"	gl_FragColor = texture( u_fbtexture, o_texcoord ) * vec4( lighting, lighting, lighting, 1.0 );\n"
		"}\n";
	spotlight_program = create_shader_program( fb_vertex_shader, fb_fragment_shader );
	glUseProgram( spotlight_program );

	float vertices[] = {
		-1.0f, -1.0f, 0.0f, 0.0f, // Lower left
		1.0f, -1.0f, 1.0f, 0.0f,  // Lower right
		1.0f, 1.0f, 1.0f, 1.0f,   // Upper right
		-1.0f, 1.0f, 0.0f, 1.0f,  // Upper left
	};

	int indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	glGenVertexArrays( 1, &spotlight_vao );
	glBindVertexArray( spotlight_vao );
	GLuint vbo;
	glGenBuffers( 1, &vbo );
	glBindBuffer( GL_ARRAY_BUFFER, vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );
	glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof( float ), 0 );
	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof( float ), ( void * )( 2 * sizeof( float ) ) );
	glEnableVertexAttribArray( 1 );
	GLuint ebo;
	glGenBuffers( 1, &ebo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( indices ), indices, GL_STATIC_DRAW );

	glActiveTexture( GL_TEXTURE5 );
	if ( spotlight_texture == 0 )
	{
		glGenTextures( 1, &spotlight_texture );
	}
	glBindTexture( GL_TEXTURE_2D, spotlight_texture );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RED, WINDOW_WIDTH_PIXELS, WINDOW_HEIGHT_PIXELS, 0, GL_RED, GL_UNSIGNED_BYTE, spotlight_pixels );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

	spotlight_texture_index_location = glGetUniformLocation( spotlight_program, "u_texture" );
	glUniform1i( spotlight_texture_index_location, 5 );
	spotlight_fbtexture_index_location = glGetUniformLocation( spotlight_program, "u_fbtexture" );
	glUniform1i( spotlight_fbtexture_index_location, 3 );
	glUseProgram( 0 );
}

static void init_sprite_renderer()
{
	const char * vertex_shader_src = "#version 330\n"
		"layout(location = 0) in vec2 i_position;\n"
		"layout(location = 1) in vec2 i_texture_coords;\n"
		"layout(location = 2) in vec4 i_pos;\n"
		"layout(location = 3) in vec4 i_texcoords;\n"
		"layout(location = 4) in vec2 i_flip;\n"
		"\n"
		"uniform vec2 u_camera;\n"
		"\n"
		"out vec2 o_texture_coords;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	mat3 model = mat3(\n"
		"		i_pos.z, 0.0, i_pos.x,\n"
		"		0.0, i_pos.w, i_pos.y,\n"
		"		0.0, 0.0, 1.0\n"
		"	);\n"
		"	mat3 cam = mat3(\n"
		"		1.0, 0.0, -u_camera.x,\n"
		"		0.0, 1.0, u_camera.y,\n"
		"		0.0, 0.0, 1.0\n"
		"	);\n"
		"	mat3 texmodel = mat3(\n"
		"		i_texcoords.z, 0.0, i_texcoords.x,\n"
		"		0.0, i_texcoords.w, i_texcoords.y,\n"
		"		0.0, 0.0, 1.0\n"
		"	);\n"
		"	vec3 pos = vec3( i_position * i_flip, 1.0 ) * model * cam;\n"
		"	gl_Position = vec4( pos.xy, 0.0, 1.0 );\n"
		"	vec3 tex = vec3( i_texture_coords, 1.0 ) * texmodel;\n"
		"	o_texture_coords = tex.xy;\n"
		"}\n";

	const char * fragment_shader_src = "#version 330\n"
		"\n"
		"in vec2 o_texture_coords;\n"
		"\n"
		"uniform sampler2D u_texture;\n"
		"uniform sampler2D u_palette_texture;\n"
		"uniform float u_palette_index;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	float color_index = texture( u_texture, o_texture_coords ).r;\n"
		"	if ( color_index == 0.0f )\n"
		"	{\n"
		"		discard;\n"
		"		return;\n"
		"	}\n"
		"	gl_FragColor = texture(\n"
		"		u_palette_texture,\n"
		"		vec2( color_index, u_palette_index )\n"
		"	);\n"
		"}\n";
	
	sprite_program = create_shader_program( vertex_shader_src, fragment_shader_src );

	glUseProgram( sprite_program );

	float vertices[] = {
		-1.0f, -1.0f, 0.0f, 1.0f, // Lower left
		1.0f, -1.0f, 1.0f, 1.0f,  // Lower right
		1.0f, 1.0f, 1.0f, 0.0f,   // Upper right
		-1.0f, 1.0f, 0.0f, 0.0f,  // Upper left
	};

	int indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	glGenVertexArrays( 1, &sprite_vao );
	glBindVertexArray( sprite_vao );
	GLuint vbo;
	glGenBuffers( 1, &vbo );
	glBindBuffer( GL_ARRAY_BUFFER, vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );
	glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof( float ), 0 );
	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof( float ), ( void * )( 2 * sizeof( float ) ) );
	glEnableVertexAttribArray( 1 );
	GLuint ebo;
	glGenBuffers( 1, &ebo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( indices ), indices, GL_STATIC_DRAW );

	glGenBuffers( 1, &sprite_instances_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, sprite_instances_vbo );
	glVertexAttribPointer( 2, 4, GL_FLOAT, GL_FALSE, sizeof( sprite_graphic_t ), 0 );
	glEnableVertexAttribArray( 2 );
	glVertexAttribDivisor( 2, 1 );
	glVertexAttribPointer( 3, 4, GL_FLOAT, GL_FALSE, sizeof( sprite_graphic_t ), ( void * )( sizeof( rect_t ) ) );
	glEnableVertexAttribArray( 3 );
	glVertexAttribDivisor( 3, 1 );
	glVertexAttribPointer( 4, 2, GL_FLOAT, GL_FALSE, sizeof( sprite_graphic_t ), ( void * )( sizeof( rect_t ) * 2 ) );
	glEnableVertexAttribArray( 4 );
	glVertexAttribDivisor( 4, 1 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

	GLuint sprite_u_texture_location = glGetUniformLocation( sprite_program, "u_texture" );
	glUniform1i( sprite_u_texture_location, 0 );
	GLuint sprite_u_palette_texture_location = glGetUniformLocation( sprite_program, "u_palette_texture" );
	glUniform1i( sprite_u_palette_texture_location, 1 );

	sprite_palette_index_location = glGetUniformLocation( sprite_program, "u_palette_index" );

	// Set up camera uniform.
	sprite_camera_location = glGetUniformLocation( sprite_program, "u_camera" );
	glUniform2f( sprite_camera_location, 0.0f, 0.0f );
}

static void init_tile_renderer()
{
	const char * vertex_shader_src = "#version 330\n"
		"layout(location = 0) in vec2 i_position;\n"
		"layout(location = 1) in vec2 i_texture_coords;\n"
		"layout(location = 2) in vec2 i_pos;\n"
		"layout(location = 3) in vec2 i_texcoords;\n"
		"\n"
		"uniform vec2 u_camera;\n"
		"\n"
		"out vec2 o_texture_coords;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	mat3 model = mat3(\n"
		"		8.0 / 512.0f, 0.0, i_pos.x,\n"
		"		0.0, 8.0 / 288.0f, i_pos.y,\n"
		"		0.0, 0.0, 1.0\n"
		"	);\n"
		"	mat3 cam = mat3(\n"
		"		1.0, 0.0, -u_camera.x,\n"
		"		0.0, 1.0, u_camera.y,\n"
		"		0.0, 0.0, 1.0\n"
		"	);\n"
		"	mat3 texmodel = mat3(\n"
		"		8.0 / 1024, 0.0, i_texcoords.x,\n"
		"		0.0, 8.0 / 1024, i_texcoords.y,\n"
		"		0.0, 0.0, 1.0\n"
		"	);\n"
		"	vec3 pos = vec3( i_position, 1.0 ) * model * cam;\n"
		"	gl_Position = vec4( pos.xy, 0.0, 1.0 );\n"
		"	vec3 tex = vec3( i_texture_coords, 1.0 ) * texmodel;\n"
		"	o_texture_coords = tex.xy;\n"
		"}\n";
	
	const char * fragment_shader_src = "#version 330\n"
		"\n"
		"in vec2 o_texture_coords;\n"
		"\n"
		"uniform sampler2D u_texture;\n"
		"uniform sampler2D u_palette_texture;\n"
		"uniform float u_palette_index;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	float color_index = texture( u_texture, o_texture_coords ).r;\n"
		"	if ( color_index == 0.0f )\n"
		"	{\n"
		"		discard;\n"
		"		return;\n"
		"	}\n"
		"	gl_FragColor = texture(\n"
		"		u_palette_texture,\n"
		"		vec2( color_index, u_palette_index )\n"
		"	);\n"
		"}\n";
	
	tile_program = create_shader_program( vertex_shader_src, fragment_shader_src );

	glUseProgram( tile_program );

	float vertices[] = {
		-1.0f, -1.0f, 0.0f, 1.0f, // Lower left
		1.0f, -1.0f, 1.0f, 1.0f,  // Lower right
		1.0f, 1.0f, 1.0f, 0.0f,   // Upper right
		-1.0f, 1.0f, 0.0f, 0.0f,  // Upper left
	};

	int indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	glGenVertexArrays( 1, &tile_vao );
	glBindVertexArray( tile_vao );
	GLuint vbo;
	glGenBuffers( 1, &vbo );
	glBindBuffer( GL_ARRAY_BUFFER, vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );
	glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof( float ), 0 );
	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof( float ), ( void * )( 2 * sizeof( float ) ) );
	glEnableVertexAttribArray( 1 );
	GLuint ebo;
	glGenBuffers( 1, &ebo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( indices ), indices, GL_STATIC_DRAW );

	glGenBuffers( 1, &tile_instances_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, tile_instances_vbo );
	glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, sizeof( tile_graphic_t ), 0 );
	glEnableVertexAttribArray( 2 );
	glVertexAttribDivisor( 2, 1 );
	glVertexAttribPointer( 3, 2, GL_FLOAT, GL_FALSE, sizeof( tile_graphic_t ), ( void * )( sizeof( pair_t ) ) );
	glEnableVertexAttribArray( 3 );
	glVertexAttribDivisor( 3, 1 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

	GLuint tile_u_texture_location = glGetUniformLocation( tile_program, "u_texture" );
	glUniform1i( tile_u_texture_location, 0 );
	GLuint tile_u_palette_texture_location = glGetUniformLocation( tile_program, "u_palette_texture" );
	glUniform1i( tile_u_palette_texture_location, 1 );

	tile_palette_index_location = glGetUniformLocation( tile_program, "u_palette_index" );

	// Set up camera uniform.
	tile_camera_location = glGetUniformLocation( tile_program, "u_camera" );
	glUniform2f( tile_camera_location, 0.0f, 0.0f );
}

static void render_bg_color()
{
	glDisable( GL_BLEND );
	glEnable( GL_DEPTH_TEST );

	glUseProgram( bg_color_program );

	// Draw graphics.
	glBindVertexArray( bg_color_vao );
	glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );
}

static void render_bg_layer( const camera_t * camera )
{
	glDisable( GL_BLEND );
	glEnable( GL_DEPTH_TEST );

	glUseProgram( bg_layer_program );

	// Update camera.
	const float camera_mat[ 9 ] =
	{
		1.0f, 0.0f, -( camera->x * 2.0f / WINDOW_WIDTH_PIXELS_F ) * bg_layer_scroll_x,
		0.0f, 1.0f, ( camera->y * 2.0f / WINDOW_HEIGHT_PIXELS_F ) * bg_layer_scroll_y,
		0.0f, 0.0f, 1.0f,
	};
	glUniformMatrix3fv( bg_layer_camera_location, 1, GL_FALSE, camera_mat );

	// Update palette.
	glUniform1f( bg_layer_palette_index_location, palette_index );

	const float bgw = bg_layer_scale_x * WINDOW_WIDTH_PIXELS_F;
	const float bgh = bg_layer_scale_y * WINDOW_HEIGHT_PIXELS_F;
	const float xoffset = bg_layer_offset_x > 0.0f
		? -bg_layer_texture_width + bg_layer_offset_x
		: bg_layer_offset_x;
	const float yoffset = bg_layer_offset_y > 0.0f
		? -bg_layer_texture_height + bg_layer_offset_y
		: bg_layer_offset_y;
	const float bgcenterx = bgw / 2.0f + xoffset;
	const float bgcentery = bgh / 2.0f + yoffset;
	const float screencenterx = WINDOW_WIDTH_PIXELS_F / 2.0f;
	const float screencentery = WINDOW_HEIGHT_PIXELS_F / 2.0f;
	const float bgoffsetx = bgcenterx - screencenterx;
	const float bgoffsety = bgcentery - screencentery;
	const float xpos = bgoffsetx / WINDOW_WIDTH_PIXELS_F * 2.0f;
	const float ypos = bgoffsety / WINDOW_HEIGHT_PIXELS_F * 2.0f;

	const float model[ 9 ] =
	{
		bg_layer_scale_x, 0.0f, xpos,
		0.0f, bg_layer_scale_y, -ypos,
		0.0f, 0.0f, 1.0f,
	};
	glUniformMatrix3fv( bg_layer_model_location, 1, GL_FALSE, model );

	// Draw graphics.
	glBindVertexArray( bg_layer_vao );
	glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );
}

static void render_inventory()
{
	glDisable( GL_BLEND );
	glEnable( GL_DEPTH_TEST );

	glUseProgram( tile_program );

	// Update uniforms.
	glUniform2f( tile_camera_location, 0.0f, 0.0f );
	glUniform1f( tile_palette_index_location, palette_index );

	// Update graphics data.
	glBindBuffer( GL_ARRAY_BUFFER, tile_instances_vbo );
	glBufferData( GL_ARRAY_BUFFER, INVENTORY_SIZE, inventory_tiles, GL_STATIC_DRAW );

	// Draw graphics.
	glBindVertexArray( tile_vao );
	glDrawElementsInstanced( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, INVENTORY_TILE_COUNT );
}

static void render_rects( const camera_t * camera )
{
	glEnable( GL_BLEND );
	glDisable( GL_DEPTH_TEST );

	glUseProgram( rect_program );

	// Update camera.
	//glUniform2f( u_camera_location, camera->x * 2.0f / WINDOW_WIDTH_PIXELS_F, camera->y * 2.0f / WINDOW_HEIGHT_PIXELS_F );
	glUniform2f( rect_camera_location, 0.0f, 0.0f );

	glUniform1f( rect_palette_index_location, palette_index );

	// Update graphics data.
	glBindBuffer( GL_ARRAY_BUFFER, rect_instances_vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof( rect_graphic_t ) * rects_count, rects, GL_STATIC_DRAW );

	// Draw graphics.
	glBindVertexArray( rect_vao );
	glDrawElementsInstanced( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, rects_count );
}

static void render_spotlight()
{
	glDisable( GL_BLEND );
	glDisable( GL_DEPTH_TEST );

	glUseProgram( spotlight_program );

	// Draw graphics.
	glBindVertexArray( spotlight_vao );
	glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );
}

static void render_sprites( const camera_t * camera )
{
	glDisable( GL_BLEND );
	glEnable( GL_DEPTH_TEST );

	glUseProgram( sprite_program );

	// Update camera.
	glUniform2f( sprite_camera_location, camera->x * 2.0f / WINDOW_WIDTH_PIXELS_F, camera->y * 2.0f / WINDOW_HEIGHT_PIXELS_F );

	glUniform1f( sprite_palette_index_location, palette_index );

	// Update graphics data.
	glActiveTexture( GL_TEXTURE0 );
	glBindBuffer( GL_ARRAY_BUFFER, sprite_instances_vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof( sprite_graphic_t ) * sprites_count, sprites, GL_STATIC_DRAW );

	// Draw graphics.
	glBindVertexArray( sprite_vao );
	glDrawElementsInstanced( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, sprites_count );
}

static void render_tiles( const camera_t * camera )
{
	glDisable( GL_BLEND );
	glEnable( GL_DEPTH_TEST );

	glUseProgram( tile_program );

	// Update camera.
	glUniform2f( tile_camera_location, camera->x * 2.0f / WINDOW_WIDTH_PIXELS_F, camera->y * 2.0f / WINDOW_HEIGHT_PIXELS_F );

	glUniform1f( tile_palette_index_location, palette_index );

	// Update graphics data.
	glActiveTexture( GL_TEXTURE0 );
	glBindBuffer( GL_ARRAY_BUFFER, tile_instances_vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof( tile_graphic_t ) * tiles_count, tiles, GL_STATIC_DRAW );

	// Draw graphics.
	glBindVertexArray( tile_vao );
	glDrawElementsInstanced( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, tiles_count );
}

static void update_screen()
{
	const double screen_aspect_ratio = ( double )( WINDOW_WIDTH_PIXELS ) / ( double )( WINDOW_HEIGHT_PIXELS );
	const double monitor_aspect_ratio = ( double )( screen_width ) / ( double )( screen_height );

	// Base magnification on max that fits in window.
	magnification = 
		( unsigned int )( floor(
			( monitor_aspect_ratio > screen_aspect_ratio )
				? ( double )( screen_height ) / ( double )( WINDOW_HEIGHT_PIXELS )
				: ( double )( screen_width ) / ( double )( WINDOW_WIDTH_PIXELS )
		));

	// Clamp minimum magnification to 1.
	if ( magnification < 1 )
	{
		magnification = 1;
	}

	update_viewport();

	// Update framebuffer texture sizes & depth buffer sizes.
	glActiveTexture( GL_TEXTURE3 );
	glBindTexture( GL_TEXTURE_2D, fbtextures[ 0 ] );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, WINDOW_WIDTH_PIXELS * magnification, WINDOW_HEIGHT_PIXELS * magnification, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
	glBindRenderbuffer( GL_RENDERBUFFER, rbo[ 0 ] );
	glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WINDOW_WIDTH_PIXELS * magnification, WINDOW_HEIGHT_PIXELS * magnification );
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo[ 0 ] );
	glActiveTexture( GL_TEXTURE4 );
	glBindTexture( GL_TEXTURE_2D, fbtextures[ 1 ] );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, WINDOW_WIDTH_PIXELS * magnification, WINDOW_HEIGHT_PIXELS * magnification, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
	glBindRenderbuffer( GL_RENDERBUFFER, rbo[ 1 ] );
	glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WINDOW_WIDTH_PIXELS * magnification, WINDOW_HEIGHT_PIXELS * magnification );
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo[ 1 ] );
}

static void update_viewport()
{
	float viewportw = WINDOW_WIDTH_PIXELS * magnification;
	float viewporth = WINDOW_HEIGHT_PIXELS * magnification;
	float viewportx = floor( ( double )( screen_width - viewportw ) / 2.0 );
	float viewporty = floor( ( double )( screen_height - viewporth ) / 2.0 );
	glViewport( viewportx, viewporty, viewportw, viewporth );
}
