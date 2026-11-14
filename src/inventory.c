#include "inventory.h"

#define LV_PTS_INCREMENT 25

static uint_fast32_t lv_pts = 0;
static uint_fast32_t lv_pts_display = 0;

void inventory_add_lv_pts( uint_fast32_t amount )
{
    lv_pts += amount;
}

uint_fast32_t inventory_get_lv_pts_display()
{
	return lv_pts_display;
}

void inventory_update()
{
    if ( lv_pts_display < lv_pts )
    {
        if ( lv_pts_display + LV_PTS_INCREMENT < lv_pts )
        {
            lv_pts_display += LV_PTS_INCREMENT;
        }
        else
        {
            lv_pts_display = lv_pts;
        }
    }
    else if ( lv_pts_display > lv_pts )
    {
        if ( lv_pts_display >= LV_PTS_INCREMENT + lv_pts )
        {
            lv_pts_display -= LV_PTS_INCREMENT;
        }
        else
        {
            lv_pts_display = lv_pts;
        }
    }
}
