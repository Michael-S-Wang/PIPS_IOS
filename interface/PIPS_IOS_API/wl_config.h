#ifndef WL_CONFIG_H
#define WL_CONFIG_H

//#include <stddef.h>
#include "stdio.h"
//#include "string.h"

//--------------------------------------------------------------------------------------
#define WL_VERSION				"PIPS API v1.00 for iOS"

#define WL_APP_PLATFORM			IBEACON				//IBEACON, ATMEL
#define WL_PIPS					PIPS_3D				//PIPS_2D, PIPS_3D, PIPS_4D
#define WL_CODE_TYPE			WL_RELEASE			//WL_RELEASE, WL_DEBUG

#define WL_MEASURE_WIN_MS		(3*1000)
#define WL_RSSI_ALPHA			0.5
#define WL_POS_ALPHA			0.5
#define WL_IN_CAR_THR_DB		-60
#define WL_MAX_IN_CAR_RSSI_THR_DB	-2.0
#define WL_MAX_USER_MSG_CHAR	100
#define WL_MAX_ADV_DATA_LEN		27					// 27 for iBeacon, 28 for Atmel

//---------------------------------------------------------------------------------------
#define PIPS_2D		2
#define PIPS_3D		3
#define PIPS_4D		4

#define IBEACON		11
#define ATMEL		12

#define WL_RELEASE	21
#define WL_DEBUG	22

#define UE_SEARCH				0
#define UE_IN_CAR				1
#define UE_OUT_OF_CAR			2
#define UE_IN_BUILDING			3
#define UE_OUT_OF_BUILDING		4

#define WL_MAX_SCAN_NODES				16		/* can't be more than 16, otherwise need code change */
#define WL_MAX_SELECTED_NODES_PER_ROOM	12
#define WL_SCAN_RSSI_RANG				76		/* rssi = 20-95 */
#define WL_SCAN_RSSI_RANG_START			20
#define WL_RSSI_CBUF_SIZE				20

#if WL_APP_PLATFORM == IBEACON
	#define WL_WLABS_INDEX				6
	#define WL_WLABS_BYTEs				4

	#define WL_X_INDEX					10
	#define WL_X_BYTES					2

	#define WL_Y_INDEX					12
	#define WL_Y_BYTES					2

	#define WL_Z_INDEX					14
	#define WL_Z_BYTES					2

	#define WL_CRN_INDEX				16
	#define WL_CRN_BYTES				7

	#define WL_NODE_INDEX				22
	#define WL_NODE_BYTES				1

	#define WL_CH_INDEX					23
	#define WL_CH_BYTES					1

	#define WL_TXPOW_INDEX				26
	#define WL_TXPOW_BYTES				1
#else
	#error "invalid WL_APP_PLATFORM"
#endif

#if WL_CODE_TYPE == WL_DEBUG
#define DBG	printf
#define DBG_FILE fprintf
#else
#define DBG
#define DBG_FILE
#endif

#define dbg_log		"data\\dbg_log.txt"
#define dbg_rssi	"data\\dbg_rssi.txt"
#define dbg_alg		"data\\dbg_alg.txt"
#define dbg_api_out	"data\\dbg_api_out.txt"
#define dbg_ant		"data\\ant_pattern.txt"
//--------------------------------------------------------------------------------------
#if WL_CODE_TYPE == WL_RELEASE
#define sprintf_s sprintf
#define strcpy_s strcpy
#endif

#define sq(x) (x*x)

typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

#define CAR_CELL_CBUF		5
typedef struct
{
	int carCellCBuf[CAR_CELL_CBUF];
	int wpt;
	int rpt;
	int deep;
} wl_car_cell_circular_buff;

typedef struct
{
	char		str[30];
	double		current_ms;
	double		start_ms;
	double		dt_ms;
	int			timer_due;
	int			ms;
	int			sec;
	int			min;
	int			hr;
	//uint16_t	timer_100ms;
	//uint16_t	timer_sec;
} wl_elapse_time;

/*
typedef struct {
	uint8_t len;
	uint8_t data[31];
	double x, y, z;
	uint8_t tx_pow;
} wl_node_adv_st;
	
typedef struct {
	uint8_t len;
	uint8_t data[31];
	uint8_t rssi;
	double tm_ms;
} wl_node_scan_st;
*/

typedef struct {
	uint8_t len;
	uint8_t data[31];
	double x, y, z;
	uint8_t tx_pow;
} wl_ue_adv_st;

typedef struct {
	bool	active;
	uint16_t acc_cnt;
	uint16_t current_cnt;
	uint8_t crn_id[WL_CRN_BYTES];		/* c=customer, r=room, n=node */
	uint8_t x[WL_X_BYTES];
	uint8_t y[WL_Y_BYTES];
	uint8_t z[WL_Z_BYTES];
	uint8_t ch;
	//uint16_t rssi[WL_SCAN_RSSI_RANG];
	uint8_t tx_pow;
	double frssi;
	double iir_frssi;
	uint16_t max_r;

	uint16_t rssi_cbuf[WL_RSSI_CBUF_SIZE];
	uint16_t rpt_rssi_cbuf;
	uint16_t wpt_rssi_cbuf;
	uint16_t deep_rssi_cbuf;
} wl_ue_scan_st;

typedef struct {
	char	user_name[30];
	int		new_pos_ready;
	double	x;
	double	y;
	double	z;
	int		ueMode;
	int		max_rssi_id;
	//double	iir_a;
	int		new_msg_ready;
} wl_user_info;

/*
extern wl_node_adv_st wl_node_adv;
extern wl_node_scan_st wl_node_scan;
extern WL_elapse_time wl_time;
extern float rssi_lin[WL_SCAN_RSSI_RANG];

char* wl_get_elapse_time(void);

void wl_float_to_fix(float x, uint8_t *dh, uint8_t *dl);
float wl_fix_to_float(uint8_t dh, uint8_t dl);
bool wl_node_init(void);
void f2s(float x, char* st);
*/

extern FILE* pf_log;
extern FILE* pf_rssi;
extern FILE* pf_alg;
extern FILE* pf_api_out;
extern FILE* pf_ant;
extern const char *pos_name[];
extern char g_dbg_msg[10][60];

extern double	rssi_lin[WL_SCAN_RSSI_RANG];
extern uint8_t	wl_node_adv_data[];
extern wl_elapse_time	g_timer;
extern wl_ue_scan_st	g_ue_scan_active[WL_MAX_SCAN_NODES];
extern wl_user_info		g_user;

/* ue functions */
int memcmp(uint8_t* data1, uint8_t* data2, int len);
void wl_ue_init(void);
void wl_init_scan_data(wl_ue_scan_st *wl_ue_scan_data);
void wl_new_scanned_data(uint8_t rssi, uint8_t *adv_data);
void wl_rssi_filter(wl_ue_scan_st *wl_ue_scan_data);
void wl_proc_ue_data(void);
void wl_proc_rooms(void);
void cal_pos_2d(void);
void cal_pos_3d(void);
void cal_pos_4d(void);
void wl_room_init(void);
void ClearCarCBuf(void);


#endif
