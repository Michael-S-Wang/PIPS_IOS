#ifndef WLAPI_H
#define WLAPI_H

#define WL_LOG_CURVE_NUM			4
#define WL_MAX_DISPLAY_MSG_CHAR		60
#define WL_MAX_DISPLAY_NODES		4
#define WL_NUMBER_DBG_MSG			10

enum  pips_position
{
    NO_NODES_ARE_FOUND,
    NOT_ENOUGH_NODES,
    FINDING_POSITION,
    DRIVER_SEAT,
    FRONT_PASSENGER,
    BACK_LEFT_PASSENGER,
    BACK_MIDDLE_PASSENGER,
    BACK_RIGHT_PASSENGER,
    OUT_OF_CAR_LEFT,
    OUT_OF_CAR_RIGHT,
    OUT_OF_CAR_FRONT,
    OUT_OF_CAR_BACK,
    OUT_OF_CAR_LEFT_FRONT,
    OUT_OF_CAR_LEFT_BACK,
    OUT_OF_CAR_RIGHT_FRONT,
    OUT_OF_CAR_RIGHT_BACK,
	CAR_CELL_NUMBER
};


typedef struct
{
    int cmd1;
    int cmd2;
    int cmd3;
    int cmd4;
    double para1;
    double para2;
    double para3;
    double para4;
} pips_api_cmd;

typedef struct
{
    int new_pos_ready;		// 1 = new position ready; 0 = old position
    double x;				// %3.1f in meter
    double y;				// %3.1f in meter
    double z;				// %3.1f in meter
    int pos_id;				// pos = pips_position
	char pos_name[WL_MAX_DISPLAY_MSG_CHAR];
    
    int node[WL_MAX_DISPLAY_NODES];
    double rssi[WL_MAX_DISPLAY_NODES];	// %3.1f in dBm
    
    int new_curve_ready;
    int curve_id[WL_LOG_CURVE_NUM];		//reang: 0 - (WL_LOG_CURVE_NUM-1); -1=NA
    int curve_x[WL_LOG_CURVE_NUM];
    int curve_y[WL_LOG_CURVE_NUM];
    
    int new_msg_ready;
    char dbg_msg[WL_NUMBER_DBG_MSG][WL_MAX_DISPLAY_MSG_CHAR];
    char dbg_msg_out[WL_NUMBER_DBG_MSG * WL_MAX_DISPLAY_MSG_CHAR];
    
    int new_comments_ready;
    char cmt_msg[WL_MAX_DISPLAY_MSG_CHAR];
    
} pips_api_out;

int PIPS_rx_init(void);
int PIPS_rx_adv_data(const char *adv_data, int len, int rssi, double time_ms, const char *msg, pips_api_out *api_out);
int PIPS_api_cmd(pips_api_cmd *api_cmd);

int Test(int a){return 0;};

#endif
