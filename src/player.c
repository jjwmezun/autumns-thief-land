#include "config.h"
#include "dir.h"
#include <math.h>
#include "player.h"
#include <string.h>

#define HLXPOINT( x ) ( ( x ) + 1.0f )
#define HRXPOINT( x ) ( ( x ) + 15.0f )
#define HTYPOINT( y, h ) ( ( y ) - ( h ) + 5.0f )
#define HCYPOINT( y, h ) ( ( y ) - ( h ) / 2.0f )
#define HBYPOINT( y ) ( ( y ) - 3.0f )
#define VTYPOINT( y, h ) ( ( y ) - ( h ) + 1.0f )
#define VBYPOINT( y ) ( ( y ) )
#define VLXPOINT( x ) ( ( x ) + 3.0f )
#define VRXPOINT( x ) ( ( x ) + 12.0f )
#define VCXPOINT( x ) ( ( x ) + 7.5f )
#define VCBYPOINT( y ) ( ( y ) - 2.0f )

#define COLLISION_HLT   0
#define COLLISION_HLC   1
#define COLLISION_HLB   2
#define COLLISION_HRT   3
#define COLLISION_HRC   4
#define COLLISION_HRB   5
#define COLLISION_VTL   6
#define COLLISION_VTC   7
#define COLLISION_VTR   8
#define COLLISION_VBL   9
#define COLLISION_VBC  10
#define COLLISION_VBR  11
#define COLLISION_VBCC 12

#define COLLISION_COUNT 13

typedef struct collision_t {
	tile_t tile;
	uint16_t x;
	uint16_t y;
	unsigned int valid : 1;
} collision_t;

static void player_general_collision( const tile_t * map, player_t * player );
static collision_t player_generate_collision( const tile_t * map, const player_t * player, unsigned int type );
static unsigned int player_is_going_fast( const player_t * player );
static void player_jump( player_t * player );
static unsigned int player_slope_physics( const tile_t * map, player_t * player, float ypoint );
static void player_update_climbing( const tile_t * map, player_t * player );
static void player_update_normal( const tile_t * map, player_t * player );
static void player_update_sliding( const tile_t * map, player_t * player );
static void player_update_sliding_end( const tile_t * map, player_t * player );

player_t create_player( float x, float y )
{
	// Convert to pixel coordinates.
	x *= 16.0f;
	y *= 16.0f;

	float w = 16.0f;
	float h = 26.0f;
	return ( player_t ){
		.w = w,
		.h = h,
		.x = x,
		.y = y,
		.vx = 0.0f,
		.vy = 0.0f,
		.walkmaxspeedx = 2.0f,
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
		.bounce = 0.0f,
		.isjumping = 0,
		.onground = 0,
		.jump_padding = 0.0f,
		.jumplock = 0,
		.isducking = 0,
		.prevslidingdir = DIRX_NONE,
		.state = PLAYER_STATE_NORMAL,
		.graphics = {
			.rect = engine_add_graphic(
				( rect ){ x, y, w, h },
				( color ){ 1.0f, 0.0f, 1.0f, 1.0f }
			),
			.xtl_hitpoint = engine_add_graphic(
				( rect ){ HLXPOINT( x ), HTYPOINT( y, h ), 1.0f, 1.0f },
				( color ){ 1.0f, 1.0f, 0.0f, 1.0f }
			),
			.xtr_hitpoint = engine_add_graphic(
				( rect ){ HRXPOINT( x ), HTYPOINT( y, h ), 1.0f, 1.0f },
				( color ){ 1.0f, 1.0f, 0.0f, 1.0f }
			),
			.xml_hitpoint = engine_add_graphic(
				( rect ){ HLXPOINT( x ), HCYPOINT( y, h ), 1.0f, 1.0f },
				( color ){ 1.0f, 1.0f, 0.0f, 1.0f }
			),
			.xmr_hitpoint = engine_add_graphic(
				( rect ){ HRXPOINT( x ), HCYPOINT( y, h ), 1.0f, 1.0f },
				( color ){ 1.0f, 1.0f, 0.0f, 1.0f }
			),
			.xbl_hitpoint = engine_add_graphic(
				( rect ){ HLXPOINT( x ), HBYPOINT( y ), 1.0f, 1.0f },
				( color ){ 1.0f, 1.0f, 0.0f, 1.0f }
			),
			.xbr_hitpoint = engine_add_graphic(
				( rect ){ HRXPOINT( x ), HBYPOINT( y ), 1.0f, 1.0f },
				( color ){ 1.0f, 1.0f, 0.0f, 1.0f }
			),
			.ytl_hitpoint = engine_add_graphic(
				( rect ){ VLXPOINT( x ), VTYPOINT( y, h ), 1.0f, 1.0f },
				( color ){ 1.0f, 1.0f, 0.0f, 1.0f }
			),
			.ytr_hitpoint = engine_add_graphic(
				( rect ){ VRXPOINT( x ), VTYPOINT( y, h ), 1.0f, 1.0f },
				( color ){ 1.0f, 1.0f, 0.0f, 1.0f }
			),
			.ybl_hitpoint = engine_add_graphic(
				( rect ){ VLXPOINT( x ), VBYPOINT( y ), 1.0f, 1.0f },
				( color ){ 1.0f, 1.0f, 0.0f, 1.0f }
			),
			.ybr_hitpoint = engine_add_graphic(
				( rect ){ VRXPOINT( x ), VBYPOINT( y ), 1.0f, 1.0f },
				( color ){ 1.0f, 1.0f, 0.0f, 1.0f }
			),
			.ybc_hitpoint = engine_add_graphic(
				( rect ){ VCXPOINT( x ), VCBYPOINT( y ), 1.0f, 1.0f },
				( color ){ 1.0f, 1.0f, 0.0f, 1.0f }
			),
			.ybc2_hitpoint = engine_add_graphic(
				( rect ){ VCXPOINT( x ), VBYPOINT( y ), 1.0f, 1.0f },
				( color ){ 1.0f, 1.0f, 0.0f, 1.0f }
			),
			.ytm_hitpoint = engine_add_graphic(
				( rect ){ VCXPOINT( x ), VTYPOINT( y, h ), 1.0f, 1.0f },
				( color ){ 1.0f, 1.0f, 0.0f, 1.0f }
			)
		}
	};
}

void update_player( tile_t * map, player_t * player )
{
	switch ( player->state )
	{
		case PLAYER_STATE_NORMAL:
		{
			player_update_normal( map, player );
		}
		break;
		case ( PLAYER_STATE_SLIDING ):
		{
			player_update_sliding( map, player );
		}
		break;
		case PLAYER_STATE_SLIDING_END:
		{
			player_update_sliding_end( map, player );
		}
		break;
		case ( PLAYER_STATE_CLIMBING ):
		{
			player_update_climbing( map, player );
		}
		break;
	}

	// Update player graphics.
	engine_set_graphic_x( player->graphics.rect, player->x );
	engine_set_graphic_y( player->graphics.rect, player->y - player->h );
	engine_set_graphic_h( player->graphics.rect, player->h );
	
	engine_set_graphic_x( player->graphics.xtr_hitpoint, HRXPOINT( player->x ) - 1.0f );
	engine_set_graphic_y( player->graphics.xtr_hitpoint, HTYPOINT( player->y, player->h ) );
	
	engine_set_graphic_x( player->graphics.xmr_hitpoint, HRXPOINT( player->x ) - 1.0f );
	engine_set_graphic_y( player->graphics.xmr_hitpoint, HCYPOINT( player->y, player->h ) );
	
	engine_set_graphic_x( player->graphics.xbr_hitpoint, HRXPOINT( player->x ) - 1.0f );
	engine_set_graphic_y( player->graphics.xbr_hitpoint, HBYPOINT( player->y ) );

	engine_set_graphic_x( player->graphics.xtl_hitpoint, HLXPOINT( player->x ) );
	engine_set_graphic_y( player->graphics.xtl_hitpoint, HTYPOINT( player->y, player->h ) );

	engine_set_graphic_x( player->graphics.xml_hitpoint, HLXPOINT( player->x ) );
	engine_set_graphic_y( player->graphics.xml_hitpoint, HCYPOINT( player->y, player->h ) );

	engine_set_graphic_x( player->graphics.xbl_hitpoint, HLXPOINT( player->x ) );
	engine_set_graphic_y( player->graphics.xbl_hitpoint, HBYPOINT( player->y ) );

	engine_set_graphic_x( player->graphics.ybl_hitpoint, VLXPOINT( player->x ) );
	engine_set_graphic_y( player->graphics.ybl_hitpoint, VBYPOINT( player->y ) );

	engine_set_graphic_x( player->graphics.ybr_hitpoint, VRXPOINT( player->x ) );
	engine_set_graphic_y( player->graphics.ybr_hitpoint, VBYPOINT( player->y ) );

	engine_set_graphic_x( player->graphics.ytl_hitpoint, VLXPOINT( player->x ) );
	engine_set_graphic_y( player->graphics.ytl_hitpoint, VTYPOINT( player->y, player->h ) );

	engine_set_graphic_x( player->graphics.ytr_hitpoint, VRXPOINT( player->x ) );
	engine_set_graphic_y( player->graphics.ytr_hitpoint, VTYPOINT( player->y, player->h ) );

	engine_set_graphic_x( player->graphics.ybc_hitpoint, VCXPOINT( player->x ) );
	engine_set_graphic_y( player->graphics.ybc_hitpoint, VCBYPOINT( player->y ) );

	engine_set_graphic_x( player->graphics.ybc2_hitpoint, VCXPOINT( player->x ) );
	engine_set_graphic_y( player->graphics.ybc2_hitpoint, VBYPOINT( player->y ) );

	engine_set_graphic_x( player->graphics.ytm_hitpoint, VCXPOINT( player->x ) );
	engine_set_graphic_y( player->graphics.ytm_hitpoint, VTYPOINT( player->y, player->h ) );
}

static void player_general_collision( const tile_t * map, player_t * player )
{
	// Solid X collision.
	const collision_t hrttile = player_generate_collision( map, player, COLLISION_HRT );
	if
	(
		is_tile_solid( hrttile.tile ) ||
		is_tile_solid( player_generate_collision( map, player, COLLISION_HRB ).tile ) ||
		is_tile_solid( player_generate_collision( map, player, COLLISION_HRC ).tile )
	)
	{
		player->x = ( float )( hrttile.x * 16 ) - 15.0f;
		if ( player->vx > 0.0f )
		{
			player->vx *= -0.25f;
		}
	}
	const collision_t hlttile = player_generate_collision( map, player, COLLISION_HLT );
	if
	(
		is_tile_solid( hlttile.tile ) ||
		is_tile_solid( player_generate_collision( map, player, COLLISION_HLB ).tile ) ||
		is_tile_solid( player_generate_collision( map, player, COLLISION_HLC ).tile )
	)
	{
		player->x = ( float )( ( hlttile.x + 1 ) * 16 ) - 1.0f;
		if ( player->vx < 0.0f )
		{
			player->vx *= -0.25f;
		}
	}

	// Handle slope collision.
	//
	// We need to check slope collision for both the bottommost player pixel ( so the player lands directly on the slope normally )
	// & slightly higher so the player moves up slopes when walking up to it.
	const unsigned int onslope = player_slope_physics( map, player, VCBYPOINT( player->y ) ) ||
		player_slope_physics( map, player, VBYPOINT( player->y ) );

	// If we’re sliding, we can skip the rest & go straing to sliding.
	if ( player->state == PLAYER_STATE_SLIDING )
	{
		return;
	}

	// Solid Y collision.

	// Handle climable floor special collision.
	const collision_t vbltile = player_generate_collision( map, player, COLLISION_VBL );
	const collision_t vbrtile = player_generate_collision( map, player, COLLISION_VBR );
	const collision_t vtltile = player_generate_collision( map, player, COLLISION_VTL );
	if
	(
		is_tile_bouncy( vbltile.tile ) ||
		is_tile_bouncy( vbrtile.tile )
	)
	{
		player->isjumping = 1;
		player->bounce = 0.5f;
		player->vy = -( player->startjump + player->bounce );
		player->accy = -( player->jumpacc + player->bounce );
	}
	else if (
		// Only collide with solid-top if falling & interacting with the top o’ the block.
		player->vy > 0.0f &&
		( int )( player->y ) % 16 < 4 &&
		(
			is_tile_climb_solid_top( vbltile.tile ) ||
			is_tile_climb_solid_top( vbrtile.tile )
		)
	)
	{
		player->y = ( float )( vbltile.y * 16 );
		player->vy = 0.0f;
		player->accy = 0.0f;
		player->onground = 1;
		player->jump_padding = player_is_going_fast( player ) ? 16.0f : 2.0f;

		// For this specific block, allow climbing down.
		if ( input_pressed_down() )
		{
			player->state = PLAYER_STATE_CLIMBING;
			player->isducking = 0;

			// We need to push down ’nough so the player is no longer touching the solid-top block,
			// which would immediately push the player back on top o’ it & end the climbing state.
			player->y += 4.25f;
		}
	}
	// Handle regular floor collision.
	else if (
		onslope == 0 &&
		(
			is_tile_solid( vbltile.tile ) ||
			is_tile_solid( vbrtile.tile ) ||
			// Only collide with solid-top if falling & interacting with the top o’ the block.
			(
				player->vy > 0.0f &&
				( int )( player->y ) % 16 < 4 &&
				(
					is_tile_solid_top( vbltile.tile ) ||
					is_tile_solid_top( vbrtile.tile )
				)
			)
		)
	)
	{
		player->y = ( float )( vbltile.y * 16 );
		player->vy = 0.0f;
		player->accy = 0.0f;
		player->onground = 1;
		player->jump_padding = player_is_going_fast( player ) ? 16.0f : 2.0f;
	}
	// Handle regular ceiling collision.
	else if (
		is_tile_solid( vtltile.tile ) ||
		is_tile_solid( player_generate_collision( map, player, COLLISION_VTR ).tile )
	)
	{
		player->y = ( float )( ( vtltile.y + 1 ) * 16 ) + player->h - 1.0f;
		if ( player->vy < 0.0f )
		{
			player->vy *= -0.25f;
		}
		player->accy = 0.0f;
		player->isjumping = 0;
	}
	else
	{
		// Handle sloped ceiling collision.
		const collision_t vtctile = player_generate_collision( map, player, COLLISION_VTC );
		if ( is_tile_ceiling_slope( vtctile.tile ) )
		{
			const unsigned int relativey = ( unsigned int )( VTYPOINT( player->y, player->h ) ) - vtctile.y * 16;
			const unsigned int relativex = ( unsigned int )( VCXPOINT( player->x ) ) % 16;
			const tile_t tile = vtctile.tile;
			const unsigned int slopey = 16 - ( unsigned int )( get_tile_slope_colision( tile, relativex ) );
			if ( relativey <= slopey )
			{
				player->y = ( float )( vtctile.y * 16 ) + ( float )( slopey ) + player->h;
				if ( player->vy < 0.0f )
				{
					player->vy *= -0.25f;
				}
				player->accy = 0.0f;
				player->isjumping = 0;
			}
		}
	}

	// If touching swimmable tile, set player to swimming.
	player->isswimming = is_tile_underwater( hrttile.tile ) ||
		is_tile_underwater( player_generate_collision( map, player, COLLISION_HLC ).tile ) ||
		is_tile_underwater( player_generate_collision( map, player, COLLISION_HRC ).tile ) ||
		is_tile_underwater( player_generate_collision( map, player, COLLISION_HLT ).tile );
}

static collision_t player_generate_collision( const tile_t * map, const player_t * player, unsigned int type )
{
	int tilex = 0;
	int tiley = 0;

	switch ( type )
	{
		case ( COLLISION_HLT ):
		{
			tilex = ( int )( HLXPOINT( player->x ) / 16.0f );
			tiley = ( int )( HTYPOINT( player->y, player->h ) / 16.0f );
		}
		break;
		case ( COLLISION_HLC ):
		{
			tilex = ( int )( HLXPOINT( player->x ) / 16.0f );
			tiley = ( int )( HCYPOINT( player->y, player->h ) / 16.0f );
		}
		break;
		case ( COLLISION_HLB ):
		{
			tilex = ( int )( HLXPOINT( player->x ) / 16.0f );
			tiley = ( int )( HBYPOINT( player->y ) / 16.0f );
		}
		break;
		case ( COLLISION_HRT ):
		{
			tilex = ( int )( HRXPOINT( player->x ) / 16.0f );
			tiley = ( int )( HTYPOINT( player->y, player->h ) / 16.0f );
		}
		break;
		case ( COLLISION_HRC ):
		{
			tilex = ( int )( HRXPOINT( player->x ) / 16.0f );
			tiley = ( int )( HCYPOINT( player->y, player->h ) / 16.0f );
		}
		break;
		case ( COLLISION_HRB ):
		{
			tilex = ( int )( HRXPOINT( player->x ) / 16.0f );
			tiley = ( int )( HBYPOINT( player->y ) / 16.0f );
		}
		break;
		case ( COLLISION_VTL ):
		{
			tilex = ( int )( VLXPOINT( player->x ) / 16.0f );
			tiley = ( int )( VTYPOINT( player->y, player->h ) / 16.0f );
		}
		break;
		case ( COLLISION_VTC ):
		{
			tilex = ( int )( VCXPOINT( player->x ) / 16.0f );
			tiley = ( int )( VTYPOINT( player->y, player->h ) / 16.0f );
		}
		break;
		case ( COLLISION_VTR ):
		{
			tilex = ( int )( VRXPOINT( player->x ) / 16.0f );
			tiley = ( int )( VTYPOINT( player->y, player->h ) / 16.0f );
		}
		break;
		case ( COLLISION_VBL ):
		{
			tilex = ( int )( VLXPOINT( player->x ) / 16.0f );
			tiley = ( int )( VBYPOINT( player->y ) / 16.0f );
		}
		break;
		case ( COLLISION_VBC ):
		{
			tilex = ( int )( VCXPOINT( player->x ) / 16.0f );
			tiley = ( int )( VBYPOINT( player->y ) / 16.0f );
		}
		break;
		case ( COLLISION_VBR ):
		{
			tilex = ( int )( VRXPOINT( player->x ) / 16.0f );
			tiley = ( int )( VBYPOINT( player->y ) / 16.0f );
		}
		break;
		case ( COLLISION_VBCC ):
		{
			tilex = ( int )( VCXPOINT( player->x ) / 16.0f );
			tiley = ( int )( VCBYPOINT( player->y ) / 16.0f );
		}
		break;
	}

	if
	(
		tilex < 0 ||
		tilex >= WINDOW_WIDTH_BLOCKS ||
		tiley < 0 ||
		tiley >= WINDOW_HEIGHT_BLOCKS
	)
	{
		return ( collision_t ){ 0 };
	}

	return ( collision_t ){
		.tile = map[ tiley * WINDOW_WIDTH_BLOCKS + tilex ],
		.x = ( uint16_t )( tilex ),
		.y = ( uint16_t )( tiley ),
		.valid = 1
	};
}

static unsigned int player_is_going_fast( const player_t * player )
{
	return fabs( player->vx ) >= player->walkmaxspeedx * 1.5f;
}

static void player_jump( player_t * player )
{
	player->isjumping = 1;
	player->vy = -player->startjump;
	player->accy = -player->jumpacc;
}

static unsigned int player_slope_physics( const tile_t * map, player_t * player, float ypoint )
{
	// Get tile coordinates for player’s position.
	const int tiley = ( int )( ypoint / 16.0f );
	const int tilex = ( int )( VCXPOINT( player->x ) / 16.0f );

	// Avoid out-o’-bounds array access.
	if
	(
		tiley >= WINDOW_HEIGHT_BLOCKS ||
		tiley < 0 ||
		tilex >= WINDOW_WIDTH_BLOCKS ||
		tilex < 0
	)
	{
		return 0;
	}

	const tile_t tile = map[ tiley * WINDOW_WIDTH_BLOCKS + tilex ];

	// Skip non-slope tiles.
	if ( !is_tile_slope( tile ) )
	{
		return 0;
	}

	// Get the slope column under for player’s position’s y top.
	const unsigned int relativex = ( unsigned int )( VCXPOINT( player->x ) ) % 16; // Player’s x position relative to tile.
	const unsigned int slopey = ( unsigned int )( get_tile_slope_colision( tile, relativex ) );

	// Skip empty slope columns.
	if ( slopey >= 16 )
	{
		return 0;
	}

	// Player’s y position relative to tile.
	const unsigned int relativey = ( unsigned int )( ypoint ) - tiley * 16;

	// Give jump leeway on slopes.
	if ( relativey >= slopey - 2.0f ) 
	{
		player->onground = 1;
		player->jump_padding = player_is_going_fast( player ) ? 16.0f : 2.0f;
	}

	// Skip if player is ’bove the slope column.
	if ( relativey < slopey )
	{
		return 0;
	}

	// Force player onto slope column to prevent clipping into it & stop downward movement.
	player->y = ( float )( tiley * 16 ) + ( float )( slopey );
	player->vy = 0.0f;
	player->accy = 0.0f;

	// If slope is not slideable, we are done now.
	if ( get_tile_slope_steepness( tile ) == TILE_FLAT )
	{
		return 1;
	}

	// Slide down slope if pressing down, and set acceleration & max speed based on slope steepness.
	if ( input_pressed_down() )
	{
		static const float SLIDESPEEDS[ 4 ] = { 0.0f, 0.3f, 0.5f, 0.35f };
		static const float MAXSLIDESPEEDYMULTIPLIER[ 4 ] = { 0.0f, 0.5f, 1.0f, 2.0f };

		player->state = PLAYER_STATE_SLIDING;
		player->dirx = get_tile_slope_dirx( tile );
		player->accx = SLIDESPEEDS[ get_tile_slope_steepness( tile ) ] * ( get_tile_slope_dirx( tile ) == TILE_LEFT ? -1.0f : 1.0f );
		player->accy = SLIDESPEEDS[ get_tile_slope_steepness( tile ) ] * MAXSLIDESPEEDYMULTIPLIER[ get_tile_slope_steepness( tile ) ];
		player->maxslidespeedx = SLIDESPEEDS[ get_tile_slope_steepness( tile ) ] * 10.0f;
		player->maxslidespeedy = SLIDESPEEDS[ get_tile_slope_steepness( tile ) ] * MAXSLIDESPEEDYMULTIPLIER[ get_tile_slope_steepness( tile ) ] * 10.0f;
		player->maxspeedx = player->maxslidespeedx;

		// Change player to sliding height.
		player->h = 16.0f;

		/*
		if ( player->prevslidingdir > DIRX_NONE && player->prevslidingdir != player->dirx )
		{
			printf( "AHHH!\n" );
			printf( "accx: %f, maxslidespeedx: %f, accy: %f, maxslidespeedy: %f\n", player->accx, player->maxslidespeedx, player->accy, player->maxslidespeedy );
			player->accx *= -10.0f;
			player->maxslidespeedx *= 10.0f;
			player->accy *= -10.0f;
			player->maxslidespeedy *= 10.0f;
			printf( "accx: %f, maxslidespeedx: %f, accy: %f, maxslidespeedy: %f\n", player->accx, player->maxslidespeedx, player->accy, player->maxslidespeedy );
		}*/

		// We can move directly into sliding state & skip the rest o’ this function.
		return 1;
	}

	// Calculate resistance & fall based on slope steepness ( defaults apply to low slopes ).
	float resistance = 0.1f;
	float fall = 0.0f;
	float fally = 0.0f;
	switch ( get_tile_slope_steepness( tile ) )
	{
		case ( TILE_MEDIUM ):
		{
			fall = 0.1f;
			fally = 0.1f;
			resistance = 0.2f;
		}
		break;
		case ( TILE_HIGH ):
		{
			fall = 1.25f;
			fally = 1.25f;
			resistance = 0.3f;
		}
		break;
	}

	// Slow player when swimming.
	if ( player->isswimming )
	{
		resistance *= 2.0f;
		fall *= 0.5f;
		fally *= 0.5f;
	}

	// Apply movement resistance to slow down player when moving gainst slopes
	// & make player slide down slopes a li’l automatically.
	if ( get_tile_slope_dirx( tile ) == TILE_LEFT )
	{
		if ( player->vx > 0.0f )
		{
			player->vx /= ( 1.0 + resistance );
		}
		else if ( player->vx < 0.0f )
		{
			player->vx *= ( 1.0 + resistance );
		}
		player->x -= fall;
		player->y += fally;
	}
	else
	{
		if ( player->vx > 0.0f )
		{
			player->vx *= ( 1.0 + resistance );
		}
		else if ( player->vx < 0.0f )
		{
			player->vx /= ( 1.0 + resistance );
		}
		player->x += fall;
		player->y += fally;
	}

	return 1;
}

static void player_update_climbing( const tile_t * map, player_t * player )
{
	const float climb_max_speed = player->isswimming ? 0.5f * 0.75f : 0.75f;

	// Leave climbing state on jumping.
	if ( input_pressed_jump() )
	{
		player_jump( player );
		player->state = PLAYER_STATE_NORMAL;
		player->onground = 0;
		return;
	}

	// Move up & down ladder & slow to a stop when not moving.
	if ( input_pressed_up() )
	{
		player->accy = -player->startspeedx;
	}
	else if ( input_pressed_down() )
	{
		player->accy = player->startspeedx;
	}
	else
	{
		player->accy = 0.0f;
		player->vy /= 1.0f + 0.5f;
	}

	// Update player y speed & position..
	player->vy += player->accy;
	if ( player->vy > climb_max_speed )
	{
		player->vy = climb_max_speed;
	}
	else if ( player->vy < -climb_max_speed )
	{
		player->vy = -climb_max_speed;
	}
	player->y += player->vy;

	// Move left & right on ladder & slow to a stop when not moving.
	if ( input_pressed_left() )
	{
		player->accx = -player->startspeedx;
	}
	else if ( input_pressed_right() )
	{
		player->accx = player->startspeedx;
	}
	else
	{
		player->accx = 0.0f;
		player->vx /= 1.0f + 0.5f;
	}

	// Update player x speed & position.
	player->vx += player->accx;
	if ( player->vx > climb_max_speed )
	{
		player->vx = climb_max_speed;
	}
	else if ( player->vx < -climb_max_speed )
	{
		player->vx = -climb_max_speed;
	}
	player->x += player->vx;

	player_general_collision( map, player );

	// Make player leave climable state if not touching a climbable tile anymo’ — handle falling off ladder.
	if
	(
		!is_tile_climbable( player_generate_collision( map, player, COLLISION_HLT ).tile ) &&
		!is_tile_climbable( player_generate_collision( map, player, COLLISION_HLB ).tile ) &&
		!is_tile_climbable( player_generate_collision( map, player, COLLISION_HRT ).tile ) &&
		!is_tile_climbable( player_generate_collision( map, player, COLLISION_HRB ).tile ) &&
		!is_tile_climbable( player_generate_collision( map, player, COLLISION_HLC ).tile ) &&
		!is_tile_climbable( player_generate_collision( map, player, COLLISION_HRC ).tile )
	)
	{
		player->state = PLAYER_STATE_NORMAL;
	}
}

static void player_update_normal( const tile_t * map, player_t * player )
{
	// Handle ducking behavior.
	if ( player->onground && input_pressed_down() )
	{
		player->isducking = 1;
	}
	else if ( !input_pressed_down() )
	{
		player->isducking = 0;
	}

	// Set max speed based on various aspects o’ player state.
	player->maxspeedx = player->isswimming
		? ( player->isducking
			? 0.0f
			: ( input_pressed_run() ? player->walkmaxspeedx : player->walkmaxspeedx / 2.0f ) )
		: ( player->isducking
			? player->walkmaxspeedx / 4.0f
			: ( input_pressed_run() ? player->walkmaxspeedx * 2.0f : player->walkmaxspeedx ) );

	// Check if running.
	const float startspeedx = player->isswimming
		? input_pressed_run() ? player->startspeedx : player->startspeedx / 2.0f
		: input_pressed_run() ? player->startspeedx * 2.0f : player->startspeedx;


	// Handle Y movement.
	// Falling & Jumping
	const unsigned int going_fast = player_is_going_fast( player );
	const float startgravity = player->isswimming
		? ( input_pressed_jump() ? player->startgravity / 6.0f : player->startgravity / 4.0f )
		: ( input_pressed_jump() ? player->startgravity / 1.25f : player->startgravity );
	const float maxgravity = player->isswimming
		? ( input_pressed_jump() ? player->maxgravity / 3.0f : player->maxgravity / 2.0f )
		: ( input_pressed_jump() ? player->maxgravity / 1.25f : player->maxgravity );
	const float maxjump = ( player->isswimming
		? player->maxjump / 3.0f
		: going_fast ? player->maxjump * 1.1f : player->maxjump ) + player->bounce * 12.0f;
	const unsigned int can_start_jump = 
		( player->isswimming || player->jump_padding > 0.0f ) &&
		!player->jumplock &&
		input_pressed_jump();

	// Continue jump.
	if ( player->isjumping )
	{
		if ( input_pressed_jump() )
		{
			player->accy = -( player->jumpacc + player->bounce );
		}
		else
		{
			player->isjumping = 0;
			player->accy = 0.0f;
			player->bounce = 0.0f;
		}
	}
	// Start jump.
	else if ( can_start_jump )
	{
		player_jump( player );
	}
	// Else, fall.
	else
	{
		player->accy = startgravity;
	}

	// Update player y speed.
	player->vy += player->accy;
	if ( player->vy > maxgravity )
	{
		player->vy = maxgravity;
	}
	else if ( player->vy < -maxjump )
	{
		player->vy = -maxjump;
		player->isjumping = 0;
		player->accy = 0.0f;
		player->bounce = 0.0f;
	}

	// Update jump padding.
	player->jump_padding = player->jump_padding - 1.0f;
	if ( player->jump_padding < 0.0f )
	{
		player->jump_padding = 0.0f;
	}

	// Update jump lock.
	player->jumplock = input_pressed_jump();


	// Handle X movement.
	// Only allow movement if not both ducking & on the ground ( no crawling ).
	if ( ! ( player->isducking && player->onground ) && input_pressed_left() )
	{
		player->accx = -startspeedx;
	}
	else if ( ! ( player->isducking && player->onground ) && input_pressed_right() )
	{
		player->accx = startspeedx;
	}
	else
	{
		player->accx = 0.0f;
	}

	// Gradually slow to a stop if not moving so the player doesn’t endlessly slide around when not pressing anything.
	if ( player->accx == 0.0f )
	{
		player->vx /= 1.0f + player->friction;
	}

	// Update & cap player x speed.
	player->vx += player->accx;
	if ( player->vx > player->maxspeedx )
	{
		player->vx -= player->vx - player->maxspeedx;
	}
	else if ( player->vx < -player->maxspeedx )
	{
		player->vx += -player->maxspeedx - player->vx;
	}

	// Reset on ground.
	player->onground = 0;
	
	// Handle collisions.
	player->x += player->vx;
	player->y += player->vy;
	
	// We need to preserve the previous swimming state before it’s updated this frame to use for reference later.
	const unsigned int prev_swimming = player->isswimming;

	player_general_collision( map, player );

	// Climbable collision.
	// If touching climbable tile, and pressing up, start climbing.
	if
	(
		!player->isducking &&
		input_pressed_up() &&
		(
			is_tile_climbable( player_generate_collision( map, player, COLLISION_HLT ).tile ) ||
			is_tile_climbable( player_generate_collision( map, player, COLLISION_HRT ).tile ) ||
			is_tile_climbable( player_generate_collision( map, player, COLLISION_HLC ).tile ) ||
			is_tile_climbable( player_generate_collision( map, player, COLLISION_HRC ).tile )
		)
	)
	{
		player->state = PLAYER_STATE_CLIMBING;
		player->vy = 0.0f;
		player->accy = 0.0f;
	}

	// Handle jumping out o’ water.
	// This should only run once when leaving the water, so we check if the player was swimming last frame, but is no longer swimming this frame.
	// This should also only run when bopping upward, so we check if the player is moving upward ( vy < 0 ).
	// This should also only apply when the player is touching water from the bottom ( which is not part o’ the main swimming state check ),
	// so we test that the player is leaving the water from the top.
	if
	(
		prev_swimming &&
		!player->isswimming &&
		player->vy < 0.0f &&
		(
			is_tile_underwater( player_generate_collision( map, player, COLLISION_HLB ).tile ) ||
			is_tile_underwater( player_generate_collision( map, player, COLLISION_HRB ).tile )
		)
	)
	{
		player_jump( player );
	}

	// Shrink player height if ducking or sliding.
	player->h = player->isducking ? 16.0f : 26.0f;
}

static void player_update_sliding( const tile_t * map, player_t * player )
{
	// Update X movement based on max speed & acceleration determined by slope size on 1st entering slope state,
	// halved if swimming.
	const float accx = ( player->isswimming ? 0.5f * player->accx : player->accx );
	const float maxslidespeedx = ( player->isswimming ? 0.5f * player->maxslidespeedx : player->maxslidespeedx );
	player->vx += accx;
	if ( player->vx > maxslidespeedx )
	{
		player->vx = maxslidespeedx;
	}
	else if ( player->vx < -maxslidespeedx )
	{
		player->vx = -maxslidespeedx;
	}
	player->x += player->vx;

	// Update Y movement based on max speed & acceleration determined by slope size on 1st entering slope state,
	// halved if swimming.
	const float accy = ( player->isswimming ? 0.5f * player->accy : player->accy );
	const float maxslidespeedy = ( player->isswimming ? 0.5f * player->maxslidespeedy : player->maxslidespeedy );
	player->vy += accy;
	if ( player->vy > maxslidespeedy )
	{
		player->vy = maxslidespeedy;
	}
	else if ( player->vy < -maxslidespeedy )
	{
		player->vy = -maxslidespeedy;
	}
	player->y += player->vy;

	if ( !player->jumplock && input_pressed_jump() )
	{
		player_jump( player );
		player->state = PLAYER_STATE_NORMAL;
		player->onground = 0;
		return;
	}

	// Test if player is still on a slope, and if so, adjust player y position to match slope.
	int onslope = 0;
	float i = 0.0f;
	while ( i < 16.0f )
	{
		const float ypoint = VBYPOINT( player->y ) + i;
		const int vbtiley = ( int )( ypoint / 16.0f );
		const int vctilex = ( int )( VCXPOINT( player->x ) / 16.0f );
		i += 1.0f;

		// Avoid out-of-bounds access to the map array.
		if (
			!(
				vbtiley < WINDOW_HEIGHT_BLOCKS && vbtiley >= 0 &&
				vctilex < WINDOW_WIDTH_BLOCKS && vctilex >= 0
			)
		)
		{
			continue;
		}

		const tile_t tile = map[ vbtiley * WINDOW_WIDTH_BLOCKS + vctilex ];

		if ( !is_tile_slope( tile ) )
		{
			continue;
		}

		// Calculate the relative position of the player to the slope tile and determine if the player is on or above the slope.
		const unsigned int relativex = ( unsigned int )( VCXPOINT( player->x ) ) % 16;
		const unsigned int slopey = ( unsigned int )( get_tile_slope_colision( tile, relativex ) );
		if ( slopey < 16 )
		{
			onslope = 1;
			const unsigned int relativey = ( unsigned int )( ypoint ) - vbtiley * 16;
			if ( relativey >= slopey )
			{
				// Force player ’bove slope to prevent clipping into it.
				player->y = ( float )( vbtiley * 16 ) + ( float )( slopey );
			}
			break;
		}
	}

	// If not on or near ’bove slope, end sliding state & reset player acceleration & speed.
	if ( !onslope )
	{
		player->accx = 0.0f;
		player->accy = 0.0f;
		player->vy = 0.0f;
		player->prevslidingdir = player->dirx;
		player->state = PLAYER_STATE_SLIDING_END;

	}
}

static void player_update_sliding_end( const tile_t * map, player_t * player )
{
	// End sliding early if jumping.
	if ( player->onground > 0.0f && !player->jumplock && input_pressed_jump() )
	{
		player_jump( player );
		player->prevslidingdir = DIRX_NONE;
		player->state = PLAYER_STATE_NORMAL;
		player->onground = 0;
		return;
	}

	// Handle X movement.
	player->vx /= 1.0f + ( player->friction / 2.0f );

	// End sliding state if player is moving slow enough.
	if ( ( player->vx < 0.1f && player->vx > 0.0f ) || ( player->vx > -0.1f && player->vx < 0.0f ) )
	{
		player->isducking = input_pressed_down();
		player->prevslidingdir = DIRX_NONE;
		player->state = PLAYER_STATE_NORMAL;
	}

	// Update player y speed.
	player->accy = player->startgravity;
	player->vy += player->accy;
	if ( player->vy > player->maxgravity )
	{
		player->vy = player->maxgravity;
	}

	// Reset on ground.
	player->onground = 0;

	// Handle collisions.
	player->x += player->vx;
	player->y += player->vy;

	player_general_collision( map, player );
}
