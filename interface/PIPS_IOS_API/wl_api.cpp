#include "stdafx.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"

#include "wl_config.h"
#include "wl_alg.h"
#include "wl_ue.h"

#if WL_CODE_TYPE == WL_RELEASE
extern "C" {
//===============================================================================================
#endif

#include "wl_api.h"

pips_api_cmd g_api_cmd;
int init_flag=0;
    
int PIPS_rx_init(void)
{
	wl_ue_init();
    init_flag = 101;
	ClearCarCBuf();
    
	return 0;
}

int PIPS_api_cmd(pips_api_cmd *api_cmd)
{
    g_api_cmd.cmd1 = api_cmd->cmd1;
    g_api_cmd.cmd2 = api_cmd->cmd2;
    g_api_cmd.cmd3 = api_cmd->cmd3;
    g_api_cmd.cmd4 = api_cmd->cmd4;
    g_api_cmd.para1 = api_cmd->para1;
    g_api_cmd.para2 = api_cmd->para2;
    g_api_cmd.para3 = api_cmd->para3;
    g_api_cmd.para4 = api_cmd->para4;
    
	return 0;
}

int CarCellDetection(int pos_id)
{
	int i, n, max_id=0, id = 0;

	if( (pos_id < 0) || (pos_id >= CAR_CELL_NUMBER) ) {
		return FINDING_POSITION;
	}

	// update car cell cbuf
	g_car_cb.carCellCBuf[g_car_cb.wpt++] = pos_id;
	if(g_car_cb.wpt >= CAR_CELL_CBUF) {
		g_car_cb.wpt = 0;
	}
	if(g_car_cb.deep < CAR_CELL_CBUF) {
		g_car_cb.deep++;
	}

	// get car cell distribution
	int car_cell_dist[CAR_CELL_NUMBER];

	for(i=0; i<CAR_CELL_NUMBER; i++) {
		car_cell_dist[i] = 0;
	}
	g_car_cb.rpt = (g_car_cb.wpt - g_car_cb.deep + CAR_CELL_CBUF)%CAR_CELL_CBUF;
	for(i=0; i<g_car_cb.deep; i++) {
		n = g_car_cb.carCellCBuf[g_car_cb.rpt++];
		if(g_car_cb.rpt >= CAR_CELL_CBUF) {
			g_car_cb.rpt = 0;
		}
		car_cell_dist[n]++;
	}

	// detect car cell
	max_id = 0;
	for(i=0; i<CAR_CELL_NUMBER; i++) {
		if( max_id < car_cell_dist[i] ) {
			max_id = car_cell_dist[i];
			id = i;
		}
	}

	if(max_id < 2) {
		id = FINDING_POSITION;
	}

	return id;
}

int GetPositionId_1(void)
{
	int i, id=2;

	double thr[16][4] = {
		{0, 0, 0, 0},						// posId = 0,	"NO_NODES_ARE_FOUND",
		{0, 0, 0, 0},						// posId = 1,   "NOT_ENOUGH_NODES",
 		{0, 0, 0, 0},						// posId = 2,	"FINDING_POSITION",
		{-0.3, 0.4, -0.1, 0.7},			// posId = 3,	"DRIVER_SEAT",
 		{-0.3, 0.4,  0.7, 1.3},			// posId = 4,	"FRONT_PASSENGER",
  		{ 0.4, 1.5, -0.1, 0.4},				// posId = 5,	"BACK_LEFT_PASSENGER",
  		{ 0.4, 1.5, 0.4, 0.6},				// posId = 6,   "BACK_MIDDLE_PASSENGER",
  		{ 0.4, 1.5, 0.6, 1.3},				// posId = 7,   "BACK_RIGHT_PASSENGER",
 		{-0.3, 1.5, -20, -0.1},				// posId = 8,    "OUT_OF_CAR_LEFT",
 		{-0.3, 1.5, 1.3, 20},				// posId = 9,    "OUT_OF_CAR_RIGHT",
 		{-20, -0.3, -1.5, 2.5},				// posId =10,    "OUT_OF_CAR_FRONT",
 		{1.5, 20.0, -1.5, 2.5},				// posId =11,    "OUT_OF_CAR_BACK",
 		{-20, -0.3, -20, -0.1},				// posId =12,    "OUT_OF_CAR_LEFT_FRONT",
 		{1.5, 20.0, -20, -0.1},				// posId =13,    "OUT_OF_CAR_LEFT_BACK",
 		{-20, -0.3,  1.3, 20},				// posId =14,    "OUT_OF_CAR_RIGHT_FRONT",
 		{1.5, 20.0,  1.3, 20}				// posId =15,    "OUT_OF_CAR_RIGHT_BACK"
	};

	if( (g_user.x == 0)&&(g_user.y==0) ) {
		id = 2;
	}
	
	for(i=3; i<16; i++) {
		if( (g_user.x >= thr[i][0]) && (g_user.x < thr[i][1]) && (g_user.y >= thr[i][2]) && (g_user.y < thr[i][3]) ) {
			id = i;
			break;
		}
	}

	id = CarCellDetection(id);

	return id;
}

int GetPositionId(void)
{
	int i, id=2;

	double thr[16][4] = {
		{0, 0, 0, 0},						// posId = 0,	"NO_NODES_ARE_FOUND",
		{0, 0, 0, 0},						// posId = 1,   "NOT_ENOUGH_NODES",
 		{0, 0, 0, 0},						// posId = 2,	"FINDING_POSITION",

		// in car thr:
		{-20, 0.4, -20, 0.7},				// posId = 3,	"DRIVER_SEAT",
 		{-20, 0.4,  0.7, 20},				// posId = 4,	"FRONT_PASSENGER",
  		{ 0.4, 20, -20, 0.4},				// posId = 5,	"BACK_LEFT_PASSENGER",
  		{ 0.4, 20, 0.4, 0.7},				// posId = 6,   "BACK_MIDDLE_PASSENGER",
  		{ 0.4, 20, 0.7, 20},				// posId = 7,   "BACK_RIGHT_PASSENGER",

		// out of car thr:
 		{-1.0, 3.0, -20, 0.7},				// posId = 8,    "OUT_OF_CAR_LEFT",
 		{-1.0, 3.0, 0.7, 20},				// posId = 9,    "OUT_OF_CAR_RIGHT",
 		{-20, 0.7, -0.5, 2.0},				// posId =10,    "OUT_OF_CAR_FRONT",
 		{0.7, 20.0, -0.5, 2.0},				// posId =11,    "OUT_OF_CAR_BACK",
 		{-20, -1.0, -20, -0.5},				// posId =12,    "OUT_OF_CAR_LEFT_FRONT",
 		{3.0, 20.0, -20, -0.5},				// posId =13,    "OUT_OF_CAR_LEFT_BACK",
 		{-20, -1.0,  2.0, 20},				// posId =14,    "OUT_OF_CAR_RIGHT_FRONT",
 		{3.0, 20.0,  2.0, 20}				// posId =15,    "OUT_OF_CAR_RIGHT_BACK"
	};

	if( (g_user.x == 0)&&(g_user.y==0) ) {
		id = 2;
	}
	else {
		if(g_user.ueMode == UE_IN_CAR) {
			for(i=3; i<8; i++) {
				if( (g_user.x >= thr[i][0]) && (g_user.x < thr[i][1]) && (g_user.y >= thr[i][2]) && (g_user.y < thr[i][3]) ) {
					id = i;
					break;
				}
			}

			if(g_user.max_rssi_id) {
				if(g_user.max_rssi_id == 1)
					id = DRIVER_SEAT;
				else if(g_user.max_rssi_id == 2)
					id = FRONT_PASSENGER;
				else if(g_user.max_rssi_id == 3)
					id = BACK_LEFT_PASSENGER;
				else if(g_user.max_rssi_id == 4)
					id = BACK_RIGHT_PASSENGER;
			}
			if(g_user.max_rssi_id) {
				sprintf_s(g_dbg_msg[8], "%s, max_id=%d", g_dbg_msg[8], g_user.max_rssi_id);
				g_user.max_rssi_id = 0;
			}
		}
		else if(g_user.ueMode == UE_OUT_OF_CAR) {
			for(i=8; i<16; i++) {
				if( (g_user.x >= thr[i][0]) && (g_user.x < thr[i][1]) && (g_user.y >= thr[i][2]) && (g_user.y < thr[i][3]) ) {
					id = i;
					break;
				}
			}
		}
	}

	id = CarCellDetection(id);

	return id;
}

void update_api_out()
{

}

int rx_adv_data(int *adv_data, int len, int rssi, double time_ms, const char *msg, pips_api_out *api_out)
{
	int i, nd=0, nd_id, id;
	double pd;
	uint8_t data[31];
	static double start_time = 0;
	static int nd_good[WL_MAX_SCAN_NODES+1], nd_bad[WL_MAX_SCAN_NODES+1];
	const char *ble_ch[]={"FH", "37", "38", "39"};

	// initial
	api_out->new_msg_ready = 0;
	api_out->new_curve_ready = 0;
	g_user.new_pos_ready = 0;
	g_user.new_msg_ready = 0;

	// update timer
	wl_timer(time_ms);
	if(start_time <= 1) {
		start_time = g_timer.current_ms;
	}

	// calculate new position
	DBG_FILE(pf_log, "\n%12s,   dt=%5.1f,  rssi= %3d,  len= %2d,  adv_data= 0x ", g_timer.str, g_timer.dt_ms, rssi, len, adv_data, msg);
	for(i=0; i<len; i++) {
		data[i] = (uint8_t)adv_data[i];
		DBG_FILE(pf_log, "%02x ", data[i]);
	}
	if(len > WL_NODE_INDEX) {
	nd = data[WL_NODE_INDEX];
	DBG_FILE(pf_log, "   node= %d,  ble_ch=%s,   ", nd, ble_ch[data[WL_CH_INDEX]]);
	}
	if(msg) {
		DBG_FILE(pf_log, "msg= %s ", msg);
	}

	if(len < WL_NODE_INDEX) {
		return 0;
	}

	if( (nd >= 1) || (nd <= WL_MAX_SCAN_NODES) ) {
		if(rssi < 0) {
			nd_good[nd]++;
			wl_new_scanned_data(-rssi, data, len);
		}
		else {
			nd_bad[nd]++;
		}
	}
	else {
		DBG_FILE(pf_log, "\n Error: node_id=%d", nd);
	}


	// creat new api_out
	api_out->new_pos_ready = g_user.new_pos_ready;
	if(api_out->new_pos_ready) {
		api_out->x = g_user.x;
		api_out->y = g_user.y;
		api_out->z = g_user.z;
		api_out->pos_id = GetPositionId();
		strcpy_s(api_out->pos_name, pos_name[api_out->pos_id]);

		nd = 0;
		for(i=0; i<WL_MAX_SCAN_NODES; i++) {
			if(g_ue_scan_active[i].active == true) {
				nd_id = g_ue_scan_active[i].crn_id[6];
				if( (nd_id >= 1) && (nd_id <= WL_MAX_SCAN_NODES) ) {
					id = (nd_id - 1)%4;
					api_out->node[id] = nd_id;
					api_out->rssi[id] = 10*log10(g_ue_scan_active[i].iir_frssi);
					  DBG_FILE(pf_log, "\n        nd_id=%d, rssi=%3.1f", api_out->node[id], api_out->rssi[id]);
				
				if(g_timer.current_ms - start_time > 1) {
						pd = 100*nd_good[nd_id]*100/(g_timer.current_ms - start_time);
				}
				else {
						pd = 100*nd_good[nd_id]*100/WL_MEASURE_WIN_MS;
				}
				
					sprintf_s(api_out->dbg_msg[id], "nd%d pkt: good=%3d, bad=%3d, Pd=%4.1f%%, dt=%4.0fs", 
													nd_id, nd_good[nd_id], nd_bad[nd_id], 
													pd, (g_timer.current_ms - start_time)/1000 );

					nd_good[nd_id] = 0;
					nd_bad[nd_id] = 0;
				if(nd++ >= WL_MAX_DISPLAY_NODES) {
					break;
				}
			}
		}
		}
		start_time = g_timer.current_ms;
		DBG_FILE(pf_log, "\n    (x, y)=(%3.1f, %3.1f)", api_out->x, api_out->y);
        
		/*
        sprintf_s(api_out->dbg_msg[5], "cmd1=%d, cmd2=%d, cmd3=%d, cmd4=%d", g_api_cmd.cmd1,
                  g_api_cmd.cmd2, g_api_cmd.cmd3, g_api_cmd.cmd4);
        sprintf_s(api_out->dbg_msg[6], "par1=%5.2f, par2=%5.2f, par3=%5.2f, par4=%5.2f", g_api_cmd.para1,
                  g_api_cmd.para2, g_api_cmd.para3, g_api_cmd.para4);
        
        if(init_flag) {
			strcpy_s(api_out->dbg_msg[7], "pips is initialized");
        }
        else {
			strcpy_s(api_out->dbg_msg[7], "pips is NOT initialized");
        }
		*/
		for(i=4; i<9; i++) {
			strcpy_s(api_out->dbg_msg[i], g_dbg_msg[i]);
		}

        sprintf_s(api_out->dbg_msg[9], WL_VERSION);
		sprintf_s(api_out->dbg_msg_out, "%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s", api_out->dbg_msg[0], api_out->dbg_msg[1], api_out->dbg_msg[2], api_out->dbg_msg[3], api_out->dbg_msg[4], api_out->dbg_msg[5],
                            api_out->dbg_msg[6], api_out->dbg_msg[7], api_out->dbg_msg[8], api_out->dbg_msg[9]);
        g_user.new_msg_ready = 1;
	}

	// output msg
	api_out->new_msg_ready = g_user.new_msg_ready;
	if(api_out->new_msg_ready) {
		for(i=0; i<10; i++) {
			DBG_FILE(pf_log, "\nmsg[i]=%s", api_out->dbg_msg[i]);
		}
	}

	if(api_out->new_curve_ready) {
		//tbd
	}

	// Clean memory
	if(g_timer.timer_due == 1) {
	//if(api_out->new_pos_ready) {
		wl_ue_for_next_pos();
	}


	if( (api_out->new_pos_ready)||(api_out->new_msg_ready)||(api_out->new_curve_ready) ) {
		DBG_FILE(pf_log, "\n");
		return 1;
	}

	return 0;
}

int PIPS_rx_adv_data(const char *adv_data, int len, int rssi, double time_ms, const char *msg, pips_api_out *api_out)
{
	int i, byte_len;
	//char adv_str[100]="59000215574c4142000000000000abbccddeeff00100abcdbb";
	char onebyte[2];
	int len_offset = 2;		//(27-25)=2
	int status, data[31]; 

	byte_len = len/2;

	for(i=0; i<byte_len; i++) {
		//onebyte[0]= adv_str[2*i];
		//onebyte[1]= adv_str[2*i+1];
		onebyte[0]= adv_data[2*i];
		onebyte[1]= adv_data[2*i+1];
		data[len_offset + i] = (int)strtoul(onebyte, NULL, 16);
		//DBG_FILE(pf_log, "%02x ", data[i]);
	}

	status = rx_adv_data(data, len_offset + byte_len, rssi, time_ms, msg, api_out);

	return status;
}

#if WL_CODE_TYPE == WL_RELEASE
//===============================================================================================
}
#endif

