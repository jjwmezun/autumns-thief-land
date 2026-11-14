#ifndef INVENTORY_H
#define INVENTORY_H

#include <stdint.h>

void inventory_add_lv_pts( uint_fast32_t amount );
uint_fast32_t inventory_get_lv_pts_display();
void inventory_update();

#endif // INVENTORY_H