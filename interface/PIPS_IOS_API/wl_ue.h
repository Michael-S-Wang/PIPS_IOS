#ifndef WLUE_H
#define WLUE_H

void wl_ue_init(void);
void wl_ue_for_next_pos(void);
void wl_new_scanned_data(uint8_t rssi, uint8_t *adv_data, int len);
void wl_timer(double elapse_ms);


#endif
