/*
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "PIPS_IOS_API.h"

int PIPS_rx_adv_data(const char *adv_data, int adv_len, int rssi, double tm_ms, const char *msg, pips_api_out *api_out)
{
  

    api_out->new_pos_ready = 1;
    api_out->x = 1.0;
	api_out->y = 2.0;
	api_out->z = 3.0;
    api_out->pos_id = DRIVER_SEAT;
    
    
    for(int i=0; i<WL_MAX_DISPLAY_NODES; i++) {
        api_out->node[i] = i + 1;
        api_out->rssi[i] = rssi + i;
    }
    
    api_out->new_curve_ready = 1;
    for(int i=0; i<WL_LOG_CURVE_NUM; i++) {
        api_out->curve_id[i] = i - 1;
        api_out->curve_x[i] = i;
        api_out->curve_y[i] = i;
    }

    api_out->new_msg_ready = 1;
    for(int i=0; i<10; i++) {
        sprintf(api_out->dbg_msg[i], "debug message %d", i+1);
    }

    api_out->new_comments_ready = 1;
    sprintf(api_out->cmt_msg, "comment message");


    return 1;
}
*/

int Test(int a)
{
    return a+1;
    
}
