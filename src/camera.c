#include "camera.h"
#include "sprite.h"

void camera_follow_sprite( camera_t * camera, const struct sprite_t * sprite, const map_t * map )
{
    // If camera goes past 3/4 o’ the screen to the right, move it to follow the player.
	const float xdiff = ( sprite->x + sprite->w ) - ( camera->x + camera->w * 0.75f );
	if ( xdiff > 0.0f )
	{
		camera->x += xdiff;
        const float xdiff2 = ( camera->x + camera->w ) - map->width * 16.0f;
        if ( xdiff2 > 0.0f )
        {
            camera->x -= xdiff2;
        }
	}
    // If the camera goes past 1/4 o’ the screen to the left, move it to follow the player.
    else
    {
        const float xdiff = ( sprite->x ) - ( camera->x + camera->w * 0.25f );
        if ( xdiff < 0.0f )
        {
            camera->x += xdiff;
            if ( camera->x < 0.0f )
            {
                camera->x = 0.0f;
            }
        }
    }

    // If camera goes past 3/4 o’ the screen to the bottom, move it to follow the player.
    const float ydiff = ( sprite->y ) - ( camera->y + camera->h * 0.75f );
    if ( ydiff > 0.0f )
    {
        camera->y += ydiff;
        const float ydiff2 = ( camera->y + camera->h ) - map->height * 16.0f;
        if ( ydiff2 > 0.0f )
        {
            camera->y -= ydiff2;
        }
    }
    // If the camera goes past 1/4 o’ the screen to the top, move it to follow the player.
    else
    {
        const float ydiff = ( sprite->y + sprite->h ) - ( camera->y + camera->h * 0.25f );
        if ( ydiff < 0.0f )
        {
            camera->y += ydiff;
            if ( camera->y < 0.0f )
            {
                camera->y = 0.0f;
            }
        }
    }
}
