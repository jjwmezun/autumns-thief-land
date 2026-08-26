#include "config.h"
#include <math.h>
#include "player.h"

static unsigned int sprite_player_going_fast( sprite_t * sprite );
static void player_update_normal( map_t * map, sprite_t * sprite );
static void player_update_sliding( map_t * map, sprite_t * player );
static void player_update_sliding_end( map_t * map, sprite_t * player );
static void player_update_climbing( map_t * map, sprite_t * player );

void player_interact( sprite_t * a, sprite_t * b )
{
    switch ( b->type )
    {
        case ( SPRITE_TYPE_APPLE ):
        {
            if ( !sprite_interact_test_bop( a, b, 4.0f ) )
            {
                sprite_interact_test_harm( a, b );
            }
        }
        break;
        case ( SPRITE_TYPE_POLLO_STILL ):
        case ( SPRITE_TYPE_POLLO_MOVE_HORIZONTAL ):
        case ( SPRITE_TYPE_POLLO_MOVE_VERTICAL ):
        {
            if ( !sprite_interact_test_bop( a, b, 8.0f ) )
            {
                sprite_interact_test_harm( a, b );
            }
        }
        break;
        case ( SPRITE_TYPE_POLLO_SPIN ):
        {
            if ( !sprite_interact_test_bop( a, b, 8.0f ) )
            {
                sprite_interact_test_harm( a, b );
            }
        }
        break;
        case ( SPRITE_TYPE_CRAB ):
        {
            sprite_interact_move_toward( a, b );
            sprite_interact_test_harm( a, b );
        }
        break;
        case ( SPRITE_TYPE_TRUCK ):
        {
            // Stand on truck if the player lands on it
            if ( sprite_interact_test_top_collision( a, b, 4.0f ) )
            {
                a->y = b->y - b->h;
                a->vy = 0.0f;
                a->accy = 0.0f;
                a->onground = 1;
                a->specific.player.jump_padding = sprite_player_going_fast( a ) ? 16.0f : 2.0f;

                // Move player with truck.
                a->x += b->vx;
            }
            else 
            {
                sprite_interact_test_harm( a, b );
            }
        }
        break;
        case ( SPRITE_TYPE_BEE_STILL ):
        case ( SPRITE_TYPE_BEE_SPIN ):
        case ( SPRITE_TYPE_BEE_MOVE_HORIZONTAL ):
        case ( SPRITE_TYPE_BEE_MOVE_VERTICAL ):
        {
            sprite_interact_test_harm( a, b );
        }
        break;
        case ( SPRITE_TYPE_HYDRANT ):
        {
            if ( b->specific.hydrant.timer >= 32.0f )
            {
                sprite_interact_move_toward( a, b );
                if ( !sprite_interact_test_bop( a, b, 8.0f ) )
                {
                    sprite_interact_test_harm( a, b );
                }
            }
            else
            {
                // Awaken when player gets near.
                if
                (
                    a->x + a->w > b->x - 64.0f &&
                    a->x < b->x + b->w + 64.0f &&
                    a->y + a->h > b->y - 64.0f &&
                    a->y < b->y + b->h + 64.0f
                )
                {
                    b->specific.hydrant.awake = 1;
                }
            }
        }
        break;
    }
};

void player_update( map_t * map, sprite_t * sprite, camera_t * camera )
{
    switch ( sprite->specific.player.state )
    {
        case ( SPRITE_PLAYER_STATE_NORMAL ):
        {
            player_update_normal( map, sprite );
        }
        break;
        case ( SPRITE_PLAYER_STATE_SLIDING ):
        {
            player_update_sliding( map, sprite );
        }
        break;
        case ( SPRITE_PLAYER_STATE_SLIDING_END ):
        {
            player_update_sliding_end( map, sprite );
        }
        break;
        case ( SPRITE_PLAYER_STATE_CLIMBING ):
        {
            player_update_climbing( map, sprite );
        }
        break;
    }

	// Keep player in X bounds.
	if ( sprite->x < 0.0f )
	{
		sprite->x = 0.0f;
		if ( sprite->vx < 0.0f )
		{
			sprite->vx *= -0.25f;
		}
	}
	else
	{
		const float xdiff = ( sprite->x + sprite->w ) - map->width * 16.0f;
		if ( xdiff > 0.0f )
		{
			sprite->x -= xdiff;
			if ( sprite->vx > 0.0f )
			{
				sprite->vx *= -0.25f;
			}
		}
	}

	camera_follow_sprite( camera, sprite, map );

	// Update graphics.
	engine_set_sprite_src_h( sprite->graphics, sprite->h );
	engine_set_sprite_h( sprite->graphics, sprite->h );
	engine_set_sprite_x( sprite->graphics, sprite->x );
	engine_set_sprite_y( sprite->graphics, sprite->y - sprite->h );
}

static unsigned int sprite_player_going_fast( sprite_t * sprite )
{
	return fabs( sprite->vx ) >= sprite->specific.player.maxspeed * 1.5f;
}

static void player_update_climbing( map_t * map, sprite_t * player )
{
	const float climb_max_speed = player->isunderwater ? 0.5f * 0.75f : 0.75f;

	// Leave climbing state on jumping.
	if ( input_pressed_jump() )
	{
		sprite_jump( player );
		player->specific.player.state = SPRITE_PLAYER_STATE_NORMAL;
		player->onground = 0;
		return;
	}

	// Move up & down ladder & slow to a stop when not moving.
	if ( input_pressed_up() )
	{
		player->accy = -player->startspeed;
	}
	else if ( input_pressed_down() )
	{
		player->accy = player->startspeed;
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
		player->accx = -player->startspeed;
	}
	else if ( input_pressed_right() )
	{
		player->accx = player->startspeed;
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

	sprite_map_interaction( map, player );

	// Make player leave climable state if not touching a climbable tile anymo’ — handle falling off ladder.
	if
	(
		!sprite_test_left_collision( map, player, is_tile_climbable ).valid &&
		!sprite_test_right_collision( map, player, is_tile_climbable ).valid &&
		!sprite_test_top_collision( map, player, is_tile_climbable ).valid &&
		!sprite_test_bottom_collision( map, player, is_tile_climbable ).valid
	)
	{
		player->specific.player.state = SPRITE_PLAYER_STATE_NORMAL;
	}
}

static void player_update_normal( map_t * map, sprite_t * sprite )
{
	// Set max speed based on various aspects o’ player state.
	sprite->maxspeed = sprite->isunderwater
		? ( sprite->specific.player.isducking
			? 0.0f
			: ( input_pressed_run() ? sprite->specific.player.maxspeed : sprite->specific.player.maxspeed / 2.0f ) )
		: ( sprite->specific.player.isducking
			? sprite->specific.player.maxspeed / 4.0f
			: ( input_pressed_run() ? sprite->specific.player.maxspeed * 2.0f : sprite->specific.player.maxspeed ) );

	// Check if running.
	const float startspeedx = sprite->specific.player.isducking
		? 0.0f
		: ( sprite->isunderwater
			? input_pressed_run() ? sprite->specific.player.startspeed : sprite->specific.player.startspeed / 2.0f
			: input_pressed_run() ? sprite->specific.player.startspeed * 2.0f : sprite->specific.player.startspeed );


	// Handle Y movement.
	// Falling & Jumping
	const unsigned int going_fast = sprite_player_going_fast( sprite );
	const float startgravity = sprite->isunderwater
		? ( input_pressed_jump() ? sprite->startgravity / 6.0f : sprite->startgravity / 4.0f )
		: ( input_pressed_jump() ? sprite->startgravity / 1.25f : sprite->startgravity );
	const float maxgravity = sprite->isunderwater
		? ( input_pressed_jump() ? sprite->maxgravity / 3.0f : sprite->maxgravity / 2.0f )
		: ( input_pressed_jump() ? sprite->maxgravity / 1.25f : sprite->maxgravity );
	const float maxjump = ( sprite->isunderwater
		? sprite->maxjump / 3.0f
		: going_fast ? sprite->maxjump * 1.1f : sprite->maxjump ) + sprite->bounce * 12.0f;
	const unsigned int can_start_jump = 
		( sprite->isunderwater || sprite->specific.player.jump_padding > 0.0f ) &&
		!sprite->specific.player.jumplock &&
		input_pressed_jump();

	// Continue jump.
	if ( sprite->isjumping )
	{
		if ( input_pressed_jump() )
		{
			sprite->accy = -( sprite->jumpacc + sprite->bounce );
		}
		else
		{
			sprite->isjumping = 0;
			sprite->accy = 0.0f;
			sprite->bounce = 0.0f;
		}
	}
	// Start jump.
	else if ( can_start_jump )
	{
		sprite_jump( sprite );
	}
	// Else, fall.
	else
	{
		sprite->accy = startgravity;
	}

	// Update player y speed.
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

	// Update jump padding.
	sprite->specific.player.jump_padding = sprite->specific.player.jump_padding - 1.0f;
	if ( sprite->specific.player.jump_padding < 0.0f )
	{
		sprite->specific.player.jump_padding = 0.0f;
	}

	// Update jump lock.
	sprite->specific.player.jumplock = input_pressed_jump();


	// Handle X movement.
	// Only allow movement if not both ducking & on the ground ( no crawling ).
	if ( ! ( sprite->specific.player.isducking && sprite->onground ) && input_pressed_left() )
	{
		sprite->accx = -startspeedx;
	}
	else if ( ! ( sprite->specific.player.isducking && sprite->onground ) && input_pressed_right() )
	{
		sprite->accx = startspeedx;
	}
	else
	{
		sprite->accx = 0.0f;
	}

	// Gradually slow to a stop if not moving so the player doesn’t endlessly slide around when not pressing anything.
	if ( sprite->accx == 0.0f )
	{
		sprite->vx /= 1.0f + sprite->friction;
	}

	// Update & cap player x speed.
	sprite->vx += sprite->accx;
	if ( sprite->vx > sprite->maxspeed )
	{
		sprite->vx -= sprite->vx - sprite->maxspeed;
	}
	else if ( sprite->vx < -sprite->maxspeed )
	{
		sprite->vx += -sprite->maxspeed - sprite->vx;
	}

	// Reset on ground.
	sprite->onground = 0;
	
	// Handle collisions.
	sprite->x += sprite->vx;
	sprite->y += sprite->vy;
	
	// We need to preserve the previous swimming state before it’s updated this frame to use for reference later.
	const unsigned int prev_swimming = sprite->isunderwater;

	sprite->specific.player.prev_underwater = sprite->isunderwater;
	sprite_map_interaction( map, sprite );

	if ( sprite->onground )
	{
		sprite->specific.player.jump_padding = sprite_player_going_fast( sprite ) ? 16.0f : 2.0f;
	}

	// Handle ducking behavior.
	if ( sprite->onground && input_pressed_down() )
	{
		sprite->specific.player.isducking = 1;
	}
	else if ( !input_pressed_down() )
	{
		sprite->specific.player.isducking = 0;
	}

	if
	(
		sprite->collided_bottom_slope
		&& sprite->collided_bottom_slope_steepness > TILE_FLAT
		&& sprite->specific.player.isducking
	)
	{
		static const float SLIDESPEEDS[ 4 ] = { 0.0f, 0.3f, 0.5f, 0.35f };
		static const float MAXSLIDESPEEDYMULTIPLIER[ 4 ] = { 0.0f, 0.5f, 1.0f, 2.0f };

		sprite->specific.player.state = SPRITE_PLAYER_STATE_SLIDING;
		sprite->dirx = sprite->collided_bottom_slope_dir;
		sprite->accx = SLIDESPEEDS[ sprite->collided_bottom_slope_steepness ] * ( sprite->collided_bottom_slope_dir == TILE_LEFT ? -1.0f : 1.0f );
		sprite->accy = SLIDESPEEDS[ sprite->collided_bottom_slope_steepness ] * MAXSLIDESPEEDYMULTIPLIER[ sprite->collided_bottom_slope_steepness ];
		sprite->specific.player.maxslidespeedx = SLIDESPEEDS[ sprite->collided_bottom_slope_steepness ] * 10.0f;
		sprite->specific.player.maxslidespeedy = SLIDESPEEDS[ sprite->collided_bottom_slope_steepness ] * MAXSLIDESPEEDYMULTIPLIER[ sprite->collided_bottom_slope_steepness ] * 10.0f;
		sprite->maxspeed = sprite->specific.player.maxslidespeedx;
	}

	// Climbable collision.
	// If touching climbable tile, and pressing up, start climbing.
	if
	(
		!sprite->specific.player.isducking &&
		input_pressed_up() &&
		(
			sprite_test_left_collision( map, sprite, is_tile_climbable ).valid ||
			sprite_test_right_collision( map, sprite, is_tile_climbable ).valid ||
			sprite_test_top_collision( map, sprite, is_tile_climbable ).valid
		)
	)
	{
		sprite->specific.player.state = SPRITE_PLAYER_STATE_CLIMBING;
		sprite->vy = 0.0f;
		sprite->accy = 0.0f;
	}
	else if
	(
		sprite_test_bottom_collision( map, sprite, is_tile_climb_solid_top ).valid &&
		input_pressed_down()
	)
	{
		sprite->specific.player.state = SPRITE_PLAYER_STATE_CLIMBING;
		sprite->vy = 0.0f;
		sprite->accy = 0.0f;
		sprite->y += 4.0f;
		sprite->specific.player.isducking = 0;
	}

	// Handle jumping out o’ water.
	// This should only run once when leaving the water, so we check if the player was swimming last frame, but is no longer swimming this frame.
	// This should also only run when bopping upward, so we check if the player is moving upward ( vy < 0 ).
	// This should also only apply when the player is touching water from the bottom ( which is not part o’ the main swimming state check ),
	// so we test that the player is leaving the water from the top.
	if
	(
		sprite->specific.player.prev_underwater &&
		!sprite->isunderwater &&
		sprite->vy < 0.0f
	)
	{
		sprite_jump( sprite );
	}

	// Shrink player height if ducking or sliding.
	sprite->h = sprite->specific.player.isducking ? 16.0f : 28.0f;
}

static void player_update_sliding( map_t * map, sprite_t * player )
{
	// Update X movement based on max speed & acceleration determined by slope size on 1st entering slope state,
	// halved if swimming.
	const float accx = ( player->isunderwater ? 0.5f * player->accx : player->accx );
	const float maxslidespeedx = ( player->isunderwater ? 0.5f * player->specific.player.maxslidespeedx : player->specific.player.maxslidespeedx );
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
	const float accy = ( player->isunderwater ? 0.5f * player->accy : player->accy );
	const float maxslidespeedy = ( player->isunderwater ? 0.5f * player->specific.player.maxslidespeedy : player->specific.player.maxslidespeedy );
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

	if ( !player->specific.player.jumplock && input_pressed_jump() )
	{
		sprite_jump( player );
		player->specific.player.state = SPRITE_PLAYER_STATE_NORMAL;
		player->onground = 0;
		return;
	}

	// Test if player is still on a slope, and if so, adjust player y position to match slope.
	int onslope = 0;
	float i = 0.0f;
	while ( i < 16.0f )
	{
		const float ypoint = SLOPEPOINTY( player ) + i;
		const int vbtiley = ( int )( ypoint / 16.0f );
		const int vctilex = ( int )( SLOPEPOINTX( player ) / 16.0f );
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

		const tile_t tile = map_get_tile( map, vctilex, vbtiley );

		if ( !is_tile_slope( tile ) )
		{
			continue;
		}

		// Calculate the relative position of the player to the slope tile and determine if the player is on or above the slope.
		const unsigned int relativex = ( unsigned int )( SLOPEPOINTX( player ) ) % 16;
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
		//player->prevslidingdir = player->dirx;
		player->specific.player.state = SPRITE_PLAYER_STATE_SLIDING_END;

	}
}

static void player_update_sliding_end( map_t * map, sprite_t * player )
{
	// End sliding early if jumping.
	if ( player->onground > 0.0f && !player->specific.player.jumplock && input_pressed_jump() )
	{
		sprite_jump( player );
		//player->prevslidingdir = DIRX_NONE;
		player->specific.player.state = SPRITE_PLAYER_STATE_NORMAL;
		player->onground = 0;
		return;
	}

	// Handle X movement.
	player->vx /= 1.0f + ( player->friction / 2.0f );

	// End sliding state if player is moving slow enough.
	if ( ( player->vx < 0.1f && player->vx > 0.0f ) || ( player->vx > -0.1f && player->vx < 0.0f ) )
	{
		player->specific.player.isducking = input_pressed_down();
		//player->prevslidingdir = DIRX_NONE;
		player->specific.player.state = SPRITE_PLAYER_STATE_NORMAL;
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

	sprite_map_interaction( map, player );
}
