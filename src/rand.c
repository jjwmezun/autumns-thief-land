#include "rand.h"
#include <stdlib.h>
#include <time.h>

void rand_init( void )
{
    srand( ( unsigned int )( time( NULL ) ) );
}

float rand_range( float min, float max )
{
    return min + ( ( float )( rand() ) / ( float )( RAND_MAX ) ) * ( max - min );
}
