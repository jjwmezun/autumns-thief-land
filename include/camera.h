#ifndef CAMERA_H
#define CAMERA_H

#include "map.h"

typedef struct camera_t
{
    float x;
    float y;
    float w;
    float h;
} camera_t;

struct sprite_t;

void camera_follow_sprite( camera_t * camera, const struct sprite_t * sprite, const map_t * map );

#endif // CAMERA_H