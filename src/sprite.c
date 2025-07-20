#include "config.h"
#include <math.h>
#include "sprite.h"

#define XL( x ) ( ( x ) + 1.0f )
#define XR( x ) ( ( x ) + 14.0f )
#define XT( y ) ( ( y ) + 4.0f )
#define XM( y ) ( ( y ) + 13.0f )
#define XB( y ) ( ( y ) + 22.0f )
#define YT( y ) ( ( y ) )
#define YB( y ) ( ( y ) + 25.0f )
#define YL( x ) ( ( x ) + 3.0f )
#define YR( x ) ( ( x ) + 12.0f )
#define YC( x ) ( ( x ) + 7.5f )
#define YCB( y ) ( ( y ) + 23.0f )

static unsigned int is_going_fast( const sprite_t * sprite );
static unsigned int slope_physics( const tile_t * map, sprite_t * sprite, float dti, float ypoint );

sprite_t create_player( float x, float y )
{
	// Convert to pixel coordinates.
	x *= 16.0f;
	y *= 16.0f;

	float w = 16.0f;
	float h = 26.0f;
	return ( sprite_t ){
		.x = x,
		.y = y,
		.vx = 0.0f,
		.vy = 0.0f,
		.accx = 0.0f,
		.accy = 0.0f,
		.maxspeedx = 2.0f,
		.maxspeedy = 2.0f,
		.startspeedx = 0.1f,
		.startspeedy = 0.1f,
		.friction = 0.25f,
		.startgravity = 0.1f,
		.maxgravity = 3.0f,
		.startjump = 2.0f,
		.jumpacc = 0.2f,
		.maxjump = 3.0f,
		.isjumping = 0,
		.onground = 0,
		.jump_padding = 0.0f,
		.jumplock = 0,
		.graphics = {
			.rect = engine_add_graphic(
				( rect ){ x, y, w, h },
				( color ){ 1.0f, 0.0f, 1.0f, 1.0f }
			),
			.xtl_hitpoint = engine_add_graphic(
				( rect ){ XL( x ), XT( y ), 1.0f, 1.0f },
				( color ){ 1.0f, 1.0f, 0.0f, 1.0f }
			),
			.xtr_hitpoint = engine_add_graphic(
				( rect ){ XR( x ), XT( y ), 1.0f, 1.0f },
				( color ){ 1.0f, 1.0f, 0.0f, 1.0f }
			),
			.xml_hitpoint = engine_add_graphic(
				( rect ){ XL( x ), XM( y ), 1.0f, 1.0f },
				( color ){ 1.0f, 1.0f, 0.0f, 1.0f }
			),
			.xmr_hitpoint = engine_add_graphic(
				( rect ){ XR( x ), XM( y ), 1.0f, 1.0f },
				( color ){ 1.0f, 1.0f, 0.0f, 1.0f }
			),
			.xbl_hitpoint = engine_add_graphic(
				( rect ){ XL( x ), XB( y ), 1.0f, 1.0f },
				( color ){ 1.0f, 1.0f, 0.0f, 1.0f }
			),
			.xbr_hitpoint = engine_add_graphic(
				( rect ){ XR( x ), XB( y ), 1.0f, 1.0f },
				( color ){ 1.0f, 1.0f, 0.0f, 1.0f }
			),
			.ytl_hitpoint = engine_add_graphic(
				( rect ){ YL( x ), YT( y ), 1.0f, 1.0f },
				( color ){ 1.0f, 1.0f, 0.0f, 1.0f }
			),
			.ytr_hitpoint = engine_add_graphic(
				( rect ){ YR( x ), YT( y ), 1.0f, 1.0f },
				( color ){ 1.0f, 1.0f, 0.0f, 1.0f }
			),
			.ybl_hitpoint = engine_add_graphic(
				( rect ){ YL( x ), YB( y ), 1.0f, 1.0f },
				( color ){ 1.0f, 1.0f, 0.0f, 1.0f }
			),
			.ybr_hitpoint = engine_add_graphic(
				( rect ){ YR( x ), YB( y ), 1.0f, 1.0f },
				( color ){ 1.0f, 1.0f, 0.0f, 1.0f }
			),
			.ybc_hitpoint = engine_add_graphic(
				( rect ){ YC( x ), YCB( y ), 1.0f, 1.0f },
				( color ){ 1.0f, 1.0f, 0.0f, 1.0f }
			),
			.ybc2_hitpoint = engine_add_graphic(
				( rect ){ YC( x ), YB( y ), 1.0f, 1.0f },
				( color ){ 1.0f, 1.0f, 0.0f, 1.0f }
			)
		}
	};
}

void update_player( tile_t * map, sprite_t * player, float dt )
{
	// Check if running.
	const float startspeedx = input_pressed_run() ? player->startspeedx * 2.0f : player->startspeedx;
	const float maxspeedx = input_pressed_run() ? player->maxspeedx * 2.0f : player->maxspeedx;


	// Handle Y movement.
	// Falling & Jumping
	const unsigned int going_fast = is_going_fast( player );
	const float startgravity = input_pressed_jump() ? player->startgravity / 1.5f : player->startgravity;
	const float maxgravity = going_fast ? player->maxgravity * 1.1f : player->maxgravity;
	const float maxjump = going_fast ? player->maxjump * 1.1f : player->maxjump;
	const unsigned int can_start_jump = player->jump_padding > 0.0f &&
		!player->jumplock &&
		input_pressed_jump();

	// Continue jump.
	if ( player->isjumping )
	{
		if ( input_pressed_jump() )
		{
			player->accy = -player->jumpacc;
		}
		else
		{
			player->isjumping = 0;
			player->accy = 0.0f;
		}
	}
	// Start jump.
	else if ( can_start_jump )
	{
		player->isjumping = 1;
		player->vy = -player->startjump;
		player->accy = -player->jumpacc;
	}
	// Else, fall.
	else
	{
		player->accy = startgravity;
	}

	// Update player y speed.
	player->vy += player->accy * dt;
	if ( player->vy > maxgravity )
	{
		player->vy = maxgravity;
	}
	else if ( player->vy < -maxjump )
	{
		player->vy = -maxjump;
		player->isjumping = 0;
		player->accy = 0.0f;
	}

	// Reset on ground.
	player->onground = 0;

	// Update jump padding.
	player->jump_padding = player->jump_padding - 1.0f * dt;
	if ( player->jump_padding < 0.0f )
	{
		player->jump_padding = 0.0f;
	}

	// Update jump lock.
	player->jumplock = input_pressed_jump();


	// Handle X movement.
	if ( input_pressed_left() )
	{
		player->accx = -startspeedx;
	}
	else if ( input_pressed_right() )
	{
		player->accx = startspeedx;
	}
	else
	{
		player->accx = 0.0f;
	}

	if ( player->accx == 0.0f )
	{
		player->vx /= 1.0f + player->friction * dt;
	}
	else
	{
		player->vx += player->accx * dt;
		if ( player->vx > maxspeedx )
		{
			player->vx = maxspeedx;
		}
		else if ( player->vx < -maxspeedx )
		{
			player->vx = -maxspeedx;
		}
	}

	
	// Handle collisions.
	//
	// Loop collisions per second spent to prevent bug with o’erly big movement going past tiles.
	const unsigned int loop = dt > 1.0f ? ( unsigned int )( dt ) : 1;
	const float dti = dt / ( float )( loop );
	for ( unsigned int i = 0; i < loop; ++i )
	{
		player->x += player->vx * dti;
		player->y += player->vy * dti;

		const int yt = ( int )( YT( player->y ) / 16.0f );
		const int yb = ( int )( YB( player->y ) / 16.0f );
		const int yl = ( int )( YL( player->x ) / 16.0f );
		const int yr = ( int )( YR( player->x ) / 16.0f );
		unsigned int onslope = 0;

		// Slope collision.
		onslope = slope_physics( map, player, dti, YCB( player->y ) );
		onslope = slope_physics( map, player, dti, YB( player->y ) ) || onslope;

		// Solid Y collision.
		if (
			onslope == 0 &&
			yb < WINDOW_HEIGHT_BLOCKS && yb >= 0 &&
			( ( yl < WINDOW_WIDTH_BLOCKS && yl >= 0 && map[ yb * WINDOW_WIDTH_BLOCKS + yl ].type == TILE_SOLID ) ||
			( yr < WINDOW_WIDTH_BLOCKS && yr >= 0 && map[ yb * WINDOW_WIDTH_BLOCKS + yr ].type == TILE_SOLID ) )
		)
		{
			player->y = ( float )( yb * 16 ) - 25.0f;
			player->vy = 0.0f;
			player->accy = 0.0f;
			player->onground = 1;
			player->jump_padding = is_going_fast( player ) ? 16.0f : 2.0f;
		}
		else if (
			yt < WINDOW_HEIGHT_BLOCKS && yt >= 0 &&
			( ( yl < WINDOW_WIDTH_BLOCKS && yl >= 0 && map[ yt * WINDOW_WIDTH_BLOCKS + yl ].type == TILE_SOLID ) ||
			( yr < WINDOW_WIDTH_BLOCKS && yr >= 0 && map[ yt * WINDOW_WIDTH_BLOCKS + yr ].type == TILE_SOLID ) )
		)
		{
			player->y = ( float )( ( yt + 1 ) * 16 );
			if ( player->vy < 0.0f )
			{
				player->vy *= -0.25f;
			}
			player->accy = 0.0f;
			player->isjumping = 0;
		}

		const unsigned int xl = ( unsigned int )( XL( player->x ) / 16.0f );
		const unsigned int xr = ( unsigned int )( XR( player->x ) / 16.0f );
		const unsigned int xm = ( unsigned int )( XM( player->y ) / 16.0f );
		const unsigned int xb = ( unsigned int )( XB( player->y ) / 16.0f );
		const unsigned int xt = ( unsigned int )( XT( player->y ) / 16.0f );

		// Solid X collision.
		if
		(
			xr < WINDOW_WIDTH_BLOCKS && xr >= 0 &&
			( ( xt < WINDOW_HEIGHT_BLOCKS && xt >= 0 && map[ xt * WINDOW_WIDTH_BLOCKS + xr ].type == TILE_SOLID ) ||
			( xm < WINDOW_HEIGHT_BLOCKS && xm >= 0 && map[ xm * WINDOW_WIDTH_BLOCKS + xr ].type == TILE_SOLID ) ||
			( xb < WINDOW_HEIGHT_BLOCKS && xb >= 0 && map[ xb * WINDOW_WIDTH_BLOCKS + xr ].type == TILE_SOLID ) )
		)
		{
			player->x = ( float )( xr * 16 ) - 15.0f;
			if ( player->vx > 0.0f )
			{
				player->vx *= -0.25f;
			}
		}
		else if
		(
			xl < WINDOW_WIDTH_BLOCKS && xl >= 0 &&
			( ( xt < WINDOW_HEIGHT_BLOCKS && xt >= 0 && map[ xt * WINDOW_WIDTH_BLOCKS + xl ].type == TILE_SOLID ) ||
			( xm < WINDOW_HEIGHT_BLOCKS && xm >= 0 && map[ xm * WINDOW_WIDTH_BLOCKS + xl ].type == TILE_SOLID ) ||
			( xb < WINDOW_HEIGHT_BLOCKS && xb >= 0 && map[ xb * WINDOW_WIDTH_BLOCKS + xl ].type == TILE_SOLID ) )
		)
		{
			player->x = ( float )( ( xl + 1 ) * 16 ) - 1.0f;
			if ( player->vx < 0.0f )
			{
				player->vx *= -0.25f;
			}
		}
	}

	// Update player graphics.
	engine_set_graphic_x( player->graphics.rect, player->x );
	engine_set_graphic_y( player->graphics.rect, player->y );
	
	engine_set_graphic_x( player->graphics.xtr_hitpoint, XR( player->x ) );
	engine_set_graphic_y( player->graphics.xtr_hitpoint, XT( player->y ) );
	
	engine_set_graphic_x( player->graphics.xmr_hitpoint, XR( player->x ) );
	engine_set_graphic_y( player->graphics.xmr_hitpoint, XM( player->y ) );
	
	engine_set_graphic_x( player->graphics.xbr_hitpoint, XR( player->x ) );
	engine_set_graphic_y( player->graphics.xbr_hitpoint, XB( player->y ) );

	engine_set_graphic_x( player->graphics.xtl_hitpoint, XL( player->x ) );
	engine_set_graphic_y( player->graphics.xtl_hitpoint, XT( player->y ) );

	engine_set_graphic_x( player->graphics.xml_hitpoint, XL( player->x ) );
	engine_set_graphic_y( player->graphics.xml_hitpoint, XM( player->y ) );

	engine_set_graphic_x( player->graphics.xbl_hitpoint, XL( player->x ) );
	engine_set_graphic_y( player->graphics.xbl_hitpoint, XB( player->y ) );

	engine_set_graphic_x( player->graphics.ybl_hitpoint, YL( player->x ) );
	engine_set_graphic_y( player->graphics.ybl_hitpoint, YB( player->y ) );

	engine_set_graphic_x( player->graphics.ybr_hitpoint, YR( player->x ) );
	engine_set_graphic_y( player->graphics.ybr_hitpoint, YB( player->y ) );

	engine_set_graphic_x( player->graphics.ytl_hitpoint, YL( player->x ) );
	engine_set_graphic_y( player->graphics.ytl_hitpoint, YT( player->y ) );

	engine_set_graphic_x( player->graphics.ytr_hitpoint, YR( player->x ) );
	engine_set_graphic_y( player->graphics.ytr_hitpoint, YT( player->y ) );

	engine_set_graphic_x( player->graphics.ybc_hitpoint, YC( player->x ) );
	engine_set_graphic_y( player->graphics.ybc_hitpoint, YCB( player->y ) );

	engine_set_graphic_x( player->graphics.ybc2_hitpoint, YC( player->x ) );
	engine_set_graphic_y( player->graphics.ybc2_hitpoint, YB( player->y ) );
}

static unsigned int is_going_fast( const sprite_t * sprite )
{
	return fabs( sprite->vx ) >= sprite->maxspeedx * 1.5f;
};

static unsigned int slope_physics( const tile_t * map, sprite_t * sprite, float dti, float ypoint )
{
	const int yb = ( int )( ypoint / 16.0f );
	const int yc = ( int )( YC( sprite->x ) / 16.0f );
	if (
		yb < WINDOW_HEIGHT_BLOCKS && yb >= 0 &&
		( yc < WINDOW_WIDTH_BLOCKS && yc >= 0 &&
			( map[ yb * WINDOW_WIDTH_BLOCKS + yc ].type == TILE_SLOPE ) )
	)
	{
		const unsigned int rely = ( unsigned int )( ypoint ) - yb * 16;
		const unsigned int relx = ( unsigned int )( YC( sprite->x ) ) % 16;
		const tile_t * tile = &map[ yb * WINDOW_WIDTH_BLOCKS + yc ];
		const unsigned int sy = ( unsigned int )( get_tile_slope_colision( tile, relx ) );
		if ( sy >= 16 )
		{
			return 0;
		}
		const unsigned int sh = 16 - sy;
		if ( rely >= sy )
		{
			sprite->y = ( float )( yb * 16 ) + ( float )( sy ) - 25.0f;
			sprite->vy = 0.0f;
			sprite->accy = 0.0f;
			sprite->onground = 1;
			sprite->jump_padding = is_going_fast( sprite ) ? 16.0f : 2.0f;

			if ( tile->steepness > TILE_FLAT )
			{
				const float resistance = tile->steepness == TILE_HIGH ? 0.1f
					: tile->steepness == TILE_MEDIUM ? 0.2f
					: 0.1f;
				const float fall = tile->steepness == TILE_HIGH ? 1.0f : tile->steepness == TILE_MEDIUM ? 0.1f : 0.0f;
				if ( tile->dirx == TILE_LEFT )
				{
					if ( sprite->vx > 0.0f )
					{
						sprite->vx /= ( 1.0 + resistance * dti );
					}
					else if ( sprite->vx < 0.0f )
					{
						sprite->vx *= ( 1.0 + resistance * dti );
					}
					sprite->vx -= fall * dti;
				}
				else
				{
					if ( sprite->vx > 0.0f )
					{
						sprite->vx *= ( 1.0 + resistance * dti );
					}
					else if ( sprite->vx < 0.0f )
					{
						sprite->vx /= ( 1.0 + resistance * dti );
					}
					sprite->vx += fall * dti;
				}
			}
			return 1;
		}
	}
	return 0;
}
