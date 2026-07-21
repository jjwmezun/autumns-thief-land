#include "config.h"
#include "sprite.h"
#include <string.h>

#define BOUNDLX( sprite ) ( ( ( sprite )->x ) + ( ( sprite )->hitbox.lpadding ) )
#define BOUNDRX( sprite ) ( ( ( sprite )->x ) + ( ( sprite )->w ) - ( ( sprite )->hitbox.rpadding ) )
#define BOUNDLRY( sprite ) ( ( ( sprite )->y ) - ( ( sprite )->h ) + ( ( sprite )->hitbox.tpadding ) + 3.0f )
#define BOUNDLRH( sprite ) ( ( ( sprite )->h ) - ( ( sprite )->hitbox.tpadding ) - ( ( sprite )->hitbox.bpadding ) - 6.0f )
#define BOUNDTY( sprite ) ( ( ( sprite )->y ) - ( ( sprite )->h ) + ( ( sprite )->hitbox.tpadding ) )
#define BOUNDBY( sprite ) ( ( ( sprite )->y ) - ( ( sprite )->hitbox.bpadding ) )
#define BOUNDTBX( sprite ) ( ( ( sprite )->x ) + ( ( sprite )->hitbox.lpadding ) + 3.0f )
#define BOUNDTBW( sprite ) ( ( ( sprite )->w ) - ( ( sprite )->hitbox.lpadding ) - ( ( sprite )->hitbox.rpadding ) - 6.0f )
#define SLOPEPOINTY( sprite ) ( ( ( sprite )->y ) - ( ( sprite )->hitbox.bpadding ) - 1.0f )
#define SLOPEPOINTX( sprite ) ( ( ( sprite )->x ) + ( ( sprite )->w ) / 2.0f )

typedef struct collision_t {
	tile_t tile;
	uint16_t x;
	uint16_t y;
	unsigned int valid : 1;
} collision_t;

static void sprite_jump( sprite_t * sprite );
static void sprite_jump_when_on_ground( sprite_t * sprite );
static unsigned int sprite_slope_physics( const tile_t * map, sprite_t * sprite, float ypoint );
static collision_t sprite_test_bottom_collision( const tile_t * map, sprite_t * sprite, unsigned int ( * test )( tile_t ) );
static collision_t sprite_test_horizontal_collision( const tile_t * map, sprite_t * sprite, unsigned int ( * test )( tile_t ), int x );
static collision_t sprite_test_left_collision( const tile_t * map, sprite_t * sprite, unsigned int ( * test )( tile_t ) );
static collision_t sprite_test_right_collision( const tile_t * map, sprite_t * sprite, unsigned int ( * test )( tile_t ) );
static collision_t sprite_test_top_collision( const tile_t * map, sprite_t * sprite, unsigned int ( * test )( tile_t ) );
static collision_t sprite_test_vertical_collision( const tile_t * map, sprite_t * sprite, unsigned int ( * test )( tile_t ), int y );
static void sprite_turn_on_collision( sprite_t * sprite );

sprite_t sprite_create( float x, float y, uint8_t type )
{
	// Convert to pixel coordinates.
	x *= 16.0f;
	y *= 16.0f;

	float w = 16.0f;
	float h = 16.0f;

	switch ( type )
	{
		case SPRITE_TYPE_APPLE:
		{
			w = 16.0f;
			h = 16.0f;
		}
		break;
		case SPRITE_TYPE_POLLO:
		{
			w = 16.0f;
			h = 26.0f;
		}
		break;
		default:
		break;
	}

	sprite_t sprite = ( sprite_t ){
		.w = w,
		.h = h,
		.x = x,
		.y = y,
		.accx = 0.0f,
		.vx = 0.0f,
		.accy = 0.0f,
		.vy = 0.0f,
		.startgravity = 0.1f,
		.maxgravity = 3.0f,
		.startspeed = 0.1f,
		.maxspeed = 1.0f,
		.startjump = 2.0f,
		.jumpacc = 0.2f,
		.maxjump = 3.0f,
		.bounce = 0.0f,
		.onground = 0,
		.isjumping = 0,
		.isunderwater = 0,
		.dirx = SPRITE_DIRX_LEFT,
		.isdead = 0,
		.collided_left_solid = 0,
		.collided_left_slope = 0,
		.collided_right_solid = 0,
		.collided_right_slope = 0,
        .type = type,
		.hitbox = {
			.lpadding = 1.0f,
			.rpadding = 1.0f,
			.tpadding = 1.0f,
			.bpadding = 0.0f
		},
	};
	sprite.graphics.rect = engine_add_graphic(
		( rect ){ x, y, w, h },
		( color ){ 1.0f, 0.0f, 0.0f, 1.0f }
	);
	sprite.graphics.lcollision = engine_add_graphic(
		( rect ){ BOUNDLX( &sprite ), BOUNDLRY( &sprite ), 1.0f, BOUNDLRH( &sprite ) },
		( color ){ 0.0f, 0.5f, 0.5f, 1.0f }
	);
	sprite.graphics.rcollision = engine_add_graphic(
		( rect ){ BOUNDRX( &sprite ) - 1.0f, BOUNDLRY( &sprite ), 1.0f, BOUNDLRH( &sprite ) },
		( color ){ 0.0f, 0.5f, 0.5f, 1.0f }
	);
	sprite.graphics.bcollision = engine_add_graphic(
		( rect ){ BOUNDTBX( &sprite ), BOUNDBY( &sprite ), BOUNDTBW( &sprite ), 1.0f },
		( color ){ 0.0f, 0.5f, 0.5f, 1.0f }
	);
	sprite.graphics.tcollision = engine_add_graphic(
		( rect ){ BOUNDTBX( &sprite ), BOUNDTY( &sprite ), BOUNDTBW( &sprite ), 1.0f },
		( color ){ 0.0f, 0.5f, 0.5f, 1.0f }
	);
	sprite.graphics.slopepoint = engine_add_graphic(
		( rect ){ SLOPEPOINTX( &sprite ), SLOPEPOINTY( &sprite ), 1.0f, 1.0f },
		( color ){ 0.0f, 0.5f, 0.5f, 1.0f }
	);
	return sprite;
}

void sprite_update( tile_t * map, sprite_t * sprite )
{
	if ( sprite->isdead )
	{
		// If dead, just fall offscreen.
		sprite->accy = sprite->startgravity;
		sprite->vy += sprite->accy;
		if ( sprite->vy > sprite->maxgravity )
		{
			sprite->vy = sprite->maxgravity;
		}
		sprite->y += sprite->vy;
	}
	else
	{
		// Reset collision flags.
		sprite->collided_left_solid = 0;
		sprite->collided_left_slope = 0;
		sprite->collided_right_solid = 0;
		sprite->collided_right_slope = 0;
		sprite->onground = 0;

		// Handle falling & jumping.
		const float startgravity = sprite->isunderwater ? sprite->startgravity * 0.5f : sprite->startgravity;
		const float maxgravity = sprite->isunderwater ? sprite->maxgravity * 0.5f : sprite->maxgravity;
		const float maxjump = sprite->maxjump + sprite->bounce * 12.0f;
		if ( sprite->isjumping )
		{
			sprite->accy = -( sprite->jumpacc + sprite->bounce );
		}
		else
		{
			sprite->accy = startgravity;
		}
		sprite->vy += sprite->accy;
		if ( sprite->vy > maxgravity )
		{
			sprite->vy = maxgravity;
		}
		else if ( sprite->vy < -maxjump )
		{
			sprite->vy = -maxjump;
			sprite->isjumping = 0;
			sprite->accy = 0.0f;
			sprite->bounce = 0.0f;
		}
		sprite->y += sprite->vy;

		// Handle X movement.
		const float startspeed = sprite->isunderwater ? sprite->startspeed * 0.5f : sprite->startspeed;
		const float maxspeed = sprite->isunderwater ? sprite->maxspeed * 0.5f : sprite->maxspeed;
		sprite->accx = sprite->dirx == SPRITE_DIRX_RIGHT ? startspeed : -startspeed;
		sprite->vx += sprite->accx;
		if ( sprite->vx > maxspeed )
		{
			sprite->vx = maxspeed;
		}
		else if ( sprite->vx < -maxspeed )
		{
			sprite->vx = -maxspeed;
		}
		sprite->x += sprite->vx;

		// Handle X collision.
		collision_t left_solid_collision = sprite_test_left_collision( map, sprite, is_tile_solid );
		if ( left_solid_collision.valid )
		{
			sprite->x = ( float )( ( left_solid_collision.x + 1 ) * 16 ) - 1.0f;
			if ( sprite->vx < 0.0f )
			{
				sprite->vx *= -0.25f;
			}
			sprite->collided_left_solid = 1;
		}
		else
		{
			collision_t right_solid_collision = sprite_test_right_collision( map, sprite, is_tile_solid );
			if ( right_solid_collision.valid )
			{
				sprite->x = ( float )( right_solid_collision.x * 16 ) - sprite->w + 1.0f;
				if ( sprite->vx > 0.0f )
				{
					sprite->vx *= -0.25f;
				}
				sprite->collided_right_solid = 1;
			}
		}

		// Handle slope collision.
		//
		// We need to check slope collision for both the bottommost sprite pixel ( so the sprite lands directly on the slope normally )
		// & slightly higher so the sprite moves up slopes when walking up to it.
		const unsigned int onslope = sprite_slope_physics( map, sprite, SLOPEPOINTY( sprite ) ) ||
			sprite_slope_physics( map, sprite, BOUNDBY( sprite ) );

		// Handle bottom collision.
		collision_t bottom_bouncy_collision = sprite_test_bottom_collision( map, sprite, is_tile_bouncy );
		if ( bottom_bouncy_collision.valid )
		{
			sprite->isjumping = 1;
			sprite->bounce = 0.5f;
			sprite->vy = -( sprite->startjump + sprite->bounce );
			sprite->accy = -( sprite->jumpacc + sprite->bounce );
		}
		else if ( ! onslope )
		{
			collision_t bottom_solid_collision = sprite_test_bottom_collision( map, sprite, is_tile_solid );
			if ( bottom_solid_collision.valid )
			{
				sprite->y = ( float )( bottom_solid_collision.y * 16 );
				sprite->vy = 0.0f;
				sprite->accy = 0.0f;
				sprite->onground = 1;
			}
			else if ( sprite->vy > 0.0f && ( int )( sprite->y ) % 16 < 4 )
			{
				collision_t bottom_top_solid_collision = sprite_test_bottom_collision( map, sprite, is_tile_solid_top );
				if ( bottom_top_solid_collision.valid )
				{
					sprite->y = ( float )( bottom_top_solid_collision.y * 16 );
					sprite->vy = 0.0f;
					sprite->accy = 0.0f;
					sprite->onground = 1;
				}
			}
		}

		// Handle top collision.
		const collision_t top_solid_collision = sprite_test_top_collision( map, sprite, is_tile_solid );
		if ( top_solid_collision.valid )
		{
			sprite->y = ( float )( (top_solid_collision.y + 1 ) * 16 ) + sprite->h - 1.0f;
			if ( sprite->vy < 0.0f )
			{
				sprite->vy *= -0.25f;
			}
			sprite->accy = 0.0f;
			sprite->isjumping = 0;
		}
		else
		{
			// Handle sloped ceiling collision.
			const collision_t ceiling_top_collision = sprite_test_top_collision( map, sprite, is_tile_ceiling_slope );
			if ( ceiling_top_collision.valid )
			{
				const unsigned int relativey = ( unsigned int )( SLOPEPOINTX( sprite ) ) - ceiling_top_collision.y * 16;
				const unsigned int relativex = ( unsigned int )( BOUNDTY( sprite ) ) % 16;
				const tile_t tile = ceiling_top_collision.tile;
				const unsigned int slopey = 16 - ( unsigned int )( get_tile_slope_colision( tile, relativex ) );
				if ( relativey <= slopey )
				{
					sprite->y = ( float )( ceiling_top_collision.y * 16 ) + ( float )( slopey ) + sprite->h;
					if ( sprite->vy < 0.0f )
					{
						sprite->vy *= -0.25f;
					}
					sprite->accy = 0.0f;
					sprite->isjumping = 0;
				}
			}
		}

		// Test if underwater.
		sprite->isunderwater = sprite_test_bottom_collision( map, sprite, is_tile_underwater ).valid ||
			sprite_test_left_collision( map, sprite, is_tile_underwater ).valid ||
			sprite_test_right_collision( map, sprite, is_tile_underwater ).valid;

		switch ( sprite->type )
		{
			case ( SPRITE_TYPE_APPLE ):
			{
				sprite_turn_on_collision( sprite );
			}
			break;
			case ( SPRITE_TYPE_POLLO ):
			{
				sprite_turn_on_collision( sprite );
				sprite_jump_when_on_ground( sprite );
			}
			break;
		}
	}

	// Update sprite graphics.
	engine_set_graphic_x( sprite->graphics.rect, sprite->x );
	engine_set_graphic_y( sprite->graphics.rect, sprite->y - sprite->h );
	engine_set_graphic_h( sprite->graphics.rect, sprite->h );

	engine_set_graphic_x( sprite->graphics.lcollision, BOUNDLX( sprite ) );
	engine_set_graphic_y( sprite->graphics.lcollision, BOUNDLRY( sprite ) );
	
	engine_set_graphic_x( sprite->graphics.rcollision, BOUNDRX( sprite ) - 1.0f );
	engine_set_graphic_y( sprite->graphics.rcollision, BOUNDLRY( sprite ) );

	engine_set_graphic_x( sprite->graphics.bcollision, BOUNDTBX( sprite ) );
	engine_set_graphic_y( sprite->graphics.bcollision, BOUNDBY( sprite ) );

	engine_set_graphic_x( sprite->graphics.tcollision, BOUNDTBX( sprite ) );
	engine_set_graphic_y( sprite->graphics.tcollision, BOUNDTY( sprite ) );

	engine_set_graphic_x( sprite->graphics.slopepoint, SLOPEPOINTX( sprite ) );
	engine_set_graphic_y( sprite->graphics.slopepoint, SLOPEPOINTY( sprite ) );
}

static void sprite_jump( sprite_t * sprite )
{
	sprite->isjumping = 1;
	sprite->vy = -sprite->startjump;
	sprite->accy = -sprite->jumpacc;
}

static void sprite_jump_when_on_ground( sprite_t * sprite )
{
	if ( sprite->onground )
	{
		sprite_jump( sprite );
	}
}

static unsigned int sprite_slope_physics( const tile_t * map, sprite_t * sprite, float ypoint )
{
	// Get tile coordinates for sprite’s position.
	const int tiley = ( int )( ypoint / 16.0f );
	const int tilex = ( int )( SLOPEPOINTX( sprite ) / 16.0f );

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

	// Get the slope column under for sprite’s position’s y top.
	const unsigned int relativex = ( unsigned int )( SLOPEPOINTX( sprite ) ) % 16; // sprite’s x position relative to tile.
	const unsigned int slopey = ( unsigned int )( get_tile_slope_colision( tile, relativex ) );

	// Skip empty slope columns.
	if ( slopey >= 16 )
	{
		return 0;
	}

	// sprite’s y position relative to tile.
	const unsigned int relativey = ( unsigned int )( ypoint ) - tiley * 16;

	// Give jump leeway on slopes.
	if ( relativey >= slopey - 2.0f ) 
	{
		sprite->onground = 1;
	}

	// Skip if sprite is ’bove the slope column.
	if ( relativey < slopey )
	{
		return 0;
	}

	// Force sprite onto slope column to prevent clipping into it & stop downward movement.
	sprite->y = ( float )( tiley * 16 ) + ( float )( slopey );
	sprite->vy = 0.0f;
	sprite->accy = 0.0f;

	// If slope is not slideable, we are done now.
	if ( get_tile_slope_steepness( tile ) == TILE_FLAT )
	{
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

	// Slow sprite when swimming.
	if ( sprite->isunderwater )
	{
		resistance *= 2.0f;
		fall *= 0.5f;
		fally *= 0.5f;
	}

	// Apply movement resistance to slow down sprite when moving gainst slopes
	// & make sprite slide down slopes a li’l automatically.
	if ( get_tile_slope_dirx( tile ) == TILE_LEFT )
	{
		if ( sprite->vx > 0.0f )
		{
			sprite->vx /= ( 1.0 + resistance );
		}
		else if ( sprite->vx < 0.0f )
		{
			sprite->vx *= ( 1.0 + resistance );
		}
		sprite->x -= fall;
		sprite->y += fally;

		if ( get_tile_slope_steepness( tile ) == TILE_HIGH )
		{
			sprite->collided_right_slope = 1;
		}
	}
	else
	{
		if ( sprite->vx > 0.0f )
		{
			sprite->vx *= ( 1.0 + resistance );
		}
		else if ( sprite->vx < 0.0f )
		{
			sprite->vx /= ( 1.0 + resistance );
		}
		sprite->x += fall;
		sprite->y += fally;

		if ( get_tile_slope_steepness( tile ) == TILE_HIGH )
		{
			sprite->collided_left_slope = 1;
		}
	}

	return 1;
}

static collision_t sprite_test_bottom_collision( const tile_t * map, sprite_t * sprite, unsigned int ( * test )( tile_t ) )
{
	return sprite_test_vertical_collision( map, sprite, test, BOUNDBY( sprite ) );
}

static collision_t sprite_test_horizontal_collision( const tile_t * map, sprite_t * sprite, unsigned int ( * test )( tile_t ), int x )
{
	collision_t collision = { 0 };

	// Avoid out-o’-bounds array access.
	if ( x < 0 || x >= WINDOW_WIDTH_PIXELS )
	{
		return collision;
	}

	int y = BOUNDLRY( sprite );
	int endy = ( int )( BOUNDLRY( sprite ) + BOUNDLRH( sprite ) );

	// Avoid out-o’-bounds array access.
	if ( endy < 0 || endy >= WINDOW_HEIGHT_PIXELS )
	{
		endy = WINDOW_HEIGHT_PIXELS - 1;
	}

	unsigned int tilex = 0;
	unsigned int tiley = 0;
	tile_t tile = create_empty_tile();

	while ( y < endy )
	{
		// Avoid out-o’-bounds array access.
		if ( y < 0 )
		{
			y += 16;
			continue;
		}

		// Since we just tested that x isn’t negative, we can safely cast to unsigned int here.
		tilex = ( unsigned int )( x / 16.0f );
		tiley = ( unsigned int )( y / 16.0f );

		// Grab corresponding tile & test for collision.
		// If colliding, we can just return now.
		tile = map[ tiley * WINDOW_WIDTH_BLOCKS + tilex ];
		if ( test( tile ) )
		{
			collision.tile = tile;
			collision.valid = 1;
			collision.x = tilex;
			collision.y = tiley;
			return collision;
		}
		y += 16;
	}

	// To ensure we get endx, we need to test 1 mo’ time on endx.
	// Note that in some cases this may end up making a redundant extra failed check
	// ( if the last check in the loop ’bove hit the same tile as endx )
	// but the extra check required to avoid that would probably be e’en slower & less consistent.
	tilex = ( unsigned int )( x / 16.0f );
	tiley = ( unsigned int )( endy / 16.0f );
	tile = map[ tiley * WINDOW_WIDTH_BLOCKS + tilex ];
	if ( test( tile ) )
	{
		collision.tile = tile;
		collision.valid = 1;
		collision.x = tilex;
		collision.y = tiley;
		return collision;
	}

	return collision;
}

static collision_t sprite_test_left_collision( const tile_t * map, sprite_t * sprite, unsigned int ( * test )( tile_t ) )
{
	return sprite_test_horizontal_collision( map, sprite, test, BOUNDLX( sprite ) );
}

static collision_t sprite_test_right_collision( const tile_t * map, sprite_t * sprite, unsigned int ( * test )( tile_t ) )
{
	return sprite_test_horizontal_collision( map, sprite, test, BOUNDRX( sprite ) );
}

static collision_t sprite_test_top_collision( const tile_t * map, sprite_t * sprite, unsigned int ( * test )( tile_t ) )
{
	return sprite_test_vertical_collision( map, sprite, test, BOUNDTY( sprite ) );
}

static collision_t sprite_test_vertical_collision( const tile_t * map, sprite_t * sprite, unsigned int ( * test )( tile_t ), int y )
{
	collision_t collision = { 0 };

	// Avoid out-o’-bounds array access.
	if ( y < 0 || y >= WINDOW_HEIGHT_PIXELS )
	{
		return collision;
	}

	int x = BOUNDTBX( sprite );
	int endx = ( int )( BOUNDTBX( sprite ) + BOUNDTBW( sprite ) );

	// Avoid out-o’-bounds array access.
	if ( endx < 0 || endx >= WINDOW_WIDTH_PIXELS )
	{
		endx = WINDOW_WIDTH_PIXELS - 1;
	}

	unsigned int tilex = 0;
	unsigned int tiley = 0;
	tile_t tile = create_empty_tile();

	// Generate hitpoints 16 pixels apart from x to endx & test each corresponding tile for collision.
	while ( x < endx )
	{
		// Avoid out-o’-bounds array access.
		if ( x < 0 )
		{
			x += 16;
			continue;
		}

		// Since we just tested that x isn’t negative, we can safely cast to unsigned int here.
		tilex = ( unsigned int )( x / 16.0f );
		tiley = ( unsigned int )( y / 16.0f );

		// Grab corresponding tile & test for collision.
		// If colliding, we can just return now.
		tile = map[ tiley * WINDOW_WIDTH_BLOCKS + tilex ];
		if ( test( tile ) )
		{
			collision.tile = tile;
			collision.valid = 1;
			collision.x = tilex;
			collision.y = tiley;
			return collision;
		}

		// Go to next hitpoint 16 pixels onward.
		x += 16;
	}

	// To ensure we get endx, we need to test 1 mo’ time on endx.
	// Note that in some cases this may end up making a redundant extra failed check
	// ( if the last check in the loop ’bove hit the same tile as endx )
	// but the extra check required to avoid that would probably be e’en slower & less consistent.
	tilex = ( unsigned int )( endx / 16.0f );
	tiley = ( unsigned int )( y / 16.0f );
	tile = map[ tiley * WINDOW_WIDTH_BLOCKS + tilex ];
	if ( test( tile ) )
	{
		collision.tile = tile;
		collision.valid = 1;
		collision.x = tilex;
		collision.y = tiley;
		return collision;
	}

	return collision;
}

static void sprite_turn_on_collision( sprite_t * sprite )
{
	if
	(
		sprite->collided_left_solid ||
		sprite->collided_right_solid ||
		sprite->collided_left_slope ||
		sprite->collided_right_slope
	)
	{
		sprite->dirx = !sprite->dirx;
	}
}
