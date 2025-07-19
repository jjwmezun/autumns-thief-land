#include "config.h"
#include "engine.h"
#include <stdio.h>
#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>

#define MAX_GRAPHICS 1000

static float convert_graphic_x( rect rect, float x );
static float convert_graphic_y( rect rect, float y );
static void update_screen();
static void update_viewport();

static unsigned int magnification = 4;
static unsigned int screen_width;
static unsigned int screen_height;
static graphic_id_t graphics_count = 2;
static SDL_Window * window;
static GLuint program;
static GLuint vao;
static GLuint graphics_vbo;
static graphic graphics[ MAX_GRAPHICS ];
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

	rect.w /= WINDOW_WIDTH_PIXELS_F;
	rect.h /= WINDOW_HEIGHT_PIXELS_F;
	rect.x = convert_graphic_x( rect, rect.x );
	rect.y = convert_graphic_y( rect, rect.y );
	graphics[ graphics_count ].rect = rect;
	graphics[ graphics_count ].color = color;
	return graphics_count++;
};

float engine_get_ticks()
{
	return ( float )( SDL_GetTicks() ) / 1000.f;
};

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

	const char * vertex_shader = "#version 330\n"
		"layout(location = 0) in vec2 position;\n"
		"layout(location = 1) in vec4 rect;\n"
		"layout(location = 2) in vec4 color;\n"
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
		"	gl_Position = vec4( vec3( position, 1.0 ) * model, 1.0 );\n"
		"	o_color = color;\n"
		"}\n";
	
	const char * fragment_shader = "#version 330\n"
		"\n"
		"in vec4 o_color;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	gl_FragColor = o_color;\n"
		"}\n";
	
	GLuint vs = glCreateShader( GL_VERTEX_SHADER );
	glShaderSource( vs, 1, &vertex_shader, NULL );
	glCompileShader( vs );
	GLuint fs = glCreateShader( GL_FRAGMENT_SHADER );
	glShaderSource( fs, 1, &fragment_shader, NULL );
	glCompileShader( fs );
	program = glCreateProgram();
	glAttachShader( program, vs );
	glAttachShader( program, fs );
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

	glGenBuffers( 1, &graphics_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, graphics_vbo );
	glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, sizeof( graphic ), 0 );
	glEnableVertexAttribArray( 1 );
	glVertexAttribDivisor( 1, 1 );
	glVertexAttribPointer( 2, 4, GL_FLOAT, GL_FALSE, sizeof( graphic ), ( void * )( sizeof( rect ) ) );
	glEnableVertexAttribArray( 2 );
	glVertexAttribDivisor( 2, 1 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

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

void engine_render()
{
	// Clear the screen.
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glUseProgram( program );

	// Update graphics data.
	glBindBuffer( GL_ARRAY_BUFFER, graphics_vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof( graphic ) * graphics_count, graphics, GL_STATIC_DRAW );

	// Draw graphics.
	glBindVertexArray( vao );
	glDrawElementsInstanced( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, graphics_count );

	SDL_GL_SwapWindow( window );
}

void engine_set_graphic_x( graphic_id_t graphic_id, float x )
{
	if ( graphic_id >= graphics_count )
	{
		fprintf( stderr, "Invalid graphic ID: %lu\n", graphic_id );
		return;
	}
	graphics[ graphic_id ].rect.x = convert_graphic_x( graphics[ graphic_id ].rect, x );
}

void engine_set_graphic_y( graphic_id_t graphic_id, float y )
{
	if ( graphic_id >= graphics_count )
	{
		fprintf( stderr, "Invalid graphic ID: %lu\n", graphic_id );
		return;
	}
	graphics[ graphic_id ].rect.y = convert_graphic_y( graphics[ graphic_id ].rect, y );
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

static float convert_graphic_x( rect rect, float x )
{
	return ( ( x / WINDOW_WIDTH_PIXELS_F - 0.5f ) * 2.0f + rect.w );
}

static float convert_graphic_y( rect rect, float y )
{
	return ( ( ( y / WINDOW_HEIGHT_PIXELS_F - 0.5f ) * 2.0f + rect.h ) * -1.0f );
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
