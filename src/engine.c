#include "config.h"
#include "engine.h"
#include <stdio.h>
#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>

#define MAX_GRAPHICS 10000
#define MAX_SPRITES 10000

typedef struct graphic_data_t
{
	rect abspos;
}
graphic_data_t;

typedef struct sprite_data_t
{
	rect pos;
	rect texcoords;
}
sprite_data_t;

static float convert_graphic_h( rect rect, float h );
static float convert_graphic_x( rect rect, float x );
static float convert_graphic_y( rect rect, float y );
static GLuint create_shader_program( const char * vertex_shader_src, const char * fragment_shader_src );
static void init_rect_renderer();
static void init_sprite_renderer();
static void render_rects( const camera_t * camera );
static void render_sprites( const camera_t * camera );
static void update_screen();
static void update_viewport();

static unsigned int magnification = 4;
static unsigned int screen_width;
static unsigned int screen_height;
static graphic_id_t graphics_count = 2;
static SDL_Window * window;
static GLuint program;
static GLuint vao;
static GLuint instances_vbo;
static graphic graphics[ MAX_GRAPHICS ];
static graphic_data_t graphics_data[ MAX_GRAPHICS ];
static GLuint u_camera_location;
static GLuint sprite_program;
static GLuint sprite_vao;
static GLuint sprite_instances_vbo;
static GLuint sprite_texture;
static GLuint sprite_palette_texture;
static float sprite_palette_index = 0.0f;
static GLuint sprite_palette_index_location;
static GLuint sprite_camera_location;
static sprite_id_t sprites_count = 0;
static sprite sprites[ MAX_SPRITES ];
static sprite_data_t sprites_data[ MAX_SPRITES ];
static struct
{
	unsigned int up : 1;
	unsigned int down : 1;
	unsigned int left : 1;
	unsigned int right : 1;
	unsigned int jump : 1;
	unsigned int run : 1;
} pressed;

graphic_id_t engine_add_graphic( rect rect, color color )
{
	if ( graphics_count >= MAX_GRAPHICS )
	{
		fprintf( stderr, "Maximum number of graphics reached.\n" );
		return 1;
	}

	graphics_data[ graphics_count ].abspos = rect;
	rect.w /= WINDOW_WIDTH_PIXELS_F;
	rect.h /= WINDOW_HEIGHT_PIXELS_F;
	rect.x = convert_graphic_x( rect, rect.x );
	rect.y = convert_graphic_y( rect, rect.y );
	graphics[ graphics_count ].rect = rect;
	graphics[ graphics_count ].color = color;
	return graphics_count++;
}

sprite_id_t engine_add_sprite( rect pos, rect texcoords )
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
	pos.x = convert_graphic_x( pos, pos.x );
	pos.y = convert_graphic_y( pos, pos.y );
	sprites[ sprites_count ].rect = pos;
	texcoords.w /= 1024.0f;
	texcoords.h /= 1024.0f;
	texcoords.x /= 1024.0f;
	texcoords.y /= 1024.0f;
	sprites[ sprites_count ].texcoords = texcoords;
	sprites[ sprites_count ].pair = ( pair ){ 1.0f, 1.0f };
	return sprites_count++;
}

void engine_change_texture( const unsigned char * pixels )
{
	glActiveTexture( GL_TEXTURE0 );
	glGenTextures( 1, &sprite_texture );
	glBindTexture( GL_TEXTURE_2D, sprite_texture );
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

	init_rect_renderer();
	init_sprite_renderer();

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

	render_rects( camera );
	render_sprites( camera );

	SDL_GL_SwapWindow( window );
}

void engine_set_graphic_h( graphic_id_t graphic_id, float h )
{
	if ( graphic_id >= graphics_count )
	{
		fprintf( stderr, "Invalid graphic ID: %lu\n", graphic_id );
		return;
	}
	graphics_data[ graphic_id ].abspos.h = h;
	graphics[ graphic_id ].rect.h = convert_graphic_h( graphics[ graphic_id ].rect, h );
	graphics[ graphic_id ].rect.y = convert_graphic_y
	(
		graphics[ graphic_id ].rect, graphics_data[ graphic_id ].abspos.y
	);
}

void engine_set_graphic_x( graphic_id_t graphic_id, float x )
{
	if ( graphic_id >= graphics_count )
	{
		fprintf( stderr, "Invalid graphic ID: %lu\n", graphic_id );
		return;
	}
	graphics_data[ graphic_id ].abspos.x = x;
	graphics[ graphic_id ].rect.x = convert_graphic_x( graphics[ graphic_id ].rect, x );
}

void engine_set_graphic_y( graphic_id_t graphic_id, float y )
{
	if ( graphic_id >= graphics_count )
	{
		fprintf( stderr, "Invalid graphic ID: %lu\n", graphic_id );
		return;
	}
	graphics_data[ graphic_id ].abspos.y = y;
	graphics[ graphic_id ].rect.y = convert_graphic_y( graphics[ graphic_id ].rect, y );
}

void engine_set_palette_index( float index )
{
	sprite_palette_index = index;
}

void engine_set_palettes( unsigned char * colors, size_t palette_count )
{
	glActiveTexture( GL_TEXTURE1 );
	glGenTextures( 1, &sprite_palette_texture );
	glBindTexture( GL_TEXTURE_2D, sprite_palette_texture );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB5_A1, 8, palette_count, 0, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, colors );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
}

void engine_set_sprite_flip_x( sprite_id_t sprite_id, unsigned int flip_x )
{
	sprites[ sprite_id ].pair.x = flip_x ? -1.0f : 1.0f;
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
	sprites[ sprite_id ].rect.h = convert_graphic_h( sprites[ sprite_id ].rect, h );
	sprites[ sprite_id ].rect.y = convert_graphic_y
	(
		sprites[ sprite_id ].rect, sprites_data[ sprite_id ].pos.y
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
	sprites[ sprite_id ].rect.x = convert_graphic_x( sprites[ sprite_id ].rect, x );
}

void engine_set_sprite_y( sprite_id_t sprite_id, float y )
{
	if ( sprite_id >= sprites_count )
	{
		fprintf( stderr, "Invalid sprite ID: %lu\n", sprite_id );
		return;
	}
	sprites_data[ sprite_id ].pos.y = y;
	sprites[ sprite_id ].rect.y = convert_graphic_y( sprites[ sprite_id ].rect, y );
}

void engine_sleep( uint16_t ms )
{
	SDL_Delay( ms );
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

static float convert_graphic_h( rect rect, float h )
{
	return h /= WINDOW_HEIGHT_PIXELS_F;
}

static float convert_graphic_x( rect rect, float x )
{
	return ( ( x / WINDOW_WIDTH_PIXELS_F - 0.5f ) * 2.0f + rect.w );
}

static float convert_graphic_y( rect rect, float y )
{
	return ( ( ( y / WINDOW_HEIGHT_PIXELS_F - 0.5f ) * 2.0f + rect.h ) * -1.0f );
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

static void init_rect_renderer()
{
	const char * vertex_shader_src = "#version 330\n"
		"layout(location = 0) in vec2 position;\n"
		"layout(location = 1) in vec4 rect;\n"
		"layout(location = 2) in vec4 color;\n"
		"\n"
		"uniform vec2 u_camera;\n"
		"\n"
		"out vec4 o_color;\n"
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
		"	o_color = color;\n"
		"}\n";
	
	const char * fragment_shader_src = "#version 330\n"
		"\n"
		"in vec4 o_color;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	gl_FragColor = o_color;\n"
		"}\n";
	
	program = create_shader_program( vertex_shader_src, fragment_shader_src );

	glUseProgram( program );

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

	glGenVertexArrays( 1, &vao );
	glBindVertexArray( vao );
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

	glGenBuffers( 1, &instances_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, instances_vbo );
	glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, sizeof( graphic ), 0 );
	glEnableVertexAttribArray( 1 );
	glVertexAttribDivisor( 1, 1 );
	glVertexAttribPointer( 2, 4, GL_FLOAT, GL_FALSE, sizeof( graphic ), ( void * )( sizeof( rect ) ) );
	glEnableVertexAttribArray( 2 );
	glVertexAttribDivisor( 2, 1 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

	// Set up camera uniform.
	u_camera_location = glGetUniformLocation( program, "u_camera" );
	glUniform2f( u_camera_location, 0.0f, 0.0f );
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
		"	gl_Position = vec4( pos, 1.0 );\n"
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
		"	gl_FragColor = texture(\n"
		"		u_palette_texture,\n"
		"		vec2( texture( u_texture, o_texture_coords ).r, u_palette_index )\n"
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
	glVertexAttribPointer( 2, 4, GL_FLOAT, GL_FALSE, sizeof( sprite ), 0 );
	glEnableVertexAttribArray( 2 );
	glVertexAttribDivisor( 2, 1 );
	glVertexAttribPointer( 3, 4, GL_FLOAT, GL_FALSE, sizeof( sprite ), ( void * )( sizeof( rect ) ) );
	glEnableVertexAttribArray( 3 );
	glVertexAttribDivisor( 3, 1 );
	glVertexAttribPointer( 4, 2, GL_FLOAT, GL_FALSE, sizeof( sprite ), ( void * )( sizeof( rect ) * 2 ) );
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

static void render_rects( const camera_t * camera )
{
	glUseProgram( program );

	// Update camera.
	glUniform2f( u_camera_location, camera->x * 2.0f / WINDOW_WIDTH_PIXELS_F, camera->y * 2.0f / WINDOW_HEIGHT_PIXELS_F );

	// Update graphics data.
	glBindBuffer( GL_ARRAY_BUFFER, instances_vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof( graphic ) * graphics_count, graphics, GL_STATIC_DRAW );

	// Draw graphics.
	glBindVertexArray( vao );
	glDrawElementsInstanced( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, graphics_count );
}

static void render_sprites( const camera_t * camera )
{
	glUseProgram( sprite_program );

	// Update camera.
	glUniform2f( sprite_camera_location, camera->x * 2.0f / WINDOW_WIDTH_PIXELS_F, camera->y * 2.0f / WINDOW_HEIGHT_PIXELS_F );

	glUniform1f( sprite_palette_index_location, sprite_palette_index );

	// Update graphics data.
	glBindBuffer( GL_ARRAY_BUFFER, sprite_instances_vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof( sprite ) * sprites_count, sprites, GL_STATIC_DRAW );

	// Draw graphics.
	glBindVertexArray( sprite_vao );
	glDrawElementsInstanced( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, sprites_count );
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
}

static void update_viewport()
{
	float viewportw = WINDOW_WIDTH_PIXELS * magnification;
	float viewporth = WINDOW_HEIGHT_PIXELS * magnification;
	float viewportx = floor( ( double )( screen_width - viewportw ) / 2.0 );
	float viewporty = floor( ( double )( screen_height - viewporth ) / 2.0 );
	glViewport( viewportx, viewporty, viewportw, viewporth );
}
