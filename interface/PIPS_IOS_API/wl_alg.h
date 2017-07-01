#ifndef WL_ALG_H
#define WL_ALG_H

#include "wl_config.h"

typedef struct
{
	int id;				// node ID
	double x, y, z;		// in meter, the niode locaton
	int max_r;			// max range, in meter
	double E;			// in mWatt, calibrated E=RSS*R0 of the node
	int tx_pow;			// in dBm
	int ble_ch;

	double r;			// in meter, distance from the UE to a node
	double rss;			// in mWatt, received RSS at distance r
	double idea_rss;	// calculated rssi based on node lopcation and measured ue location
	double w;			// weight for MRC
	double pr;			// E/RSS
	int pass;			// passed in final pos calculation

	//double rssi;		// in dBm, RSSI = 10log(RSS)
	//double pri;			// E/RSS
	//int passi;			// passed in final pos calculation

	//double nm;			// noise mean
	//double nv;			// noise varance, simga
	//double snr;			// s=1, n=nv, snr = 20log10(1/nv)
	//double gn[MAX_RSS_FILTER_TAP];
}node;

typedef struct
{
	int id;					// the UE ID
	double x, y, z;			// measured xyz, in meter
	int numRdntMeasure;		// the total number of pos calculation using all available nodes
	int dim;				// dim=2,3,4
	//double E;				// in mWatt, calibrated E=RSS*R0 from TX BLE
	//double r1, r2, r3;		// in meter, measured distance from UE to each node
	//double rss1, rss2,rss3;	// in mWatt, measured receiving signal sthrength from TX BLE
	double eqErr;			// equation error
	double w;				// averageed w of 3 nodes
	//double rssi1, rssi2, rssi3;// in dBm, 0-100, RSSI = 10log10(RSS)
	int pass;				// past the measurement
	int ueMode;				// 1=inside car, 2 = out of car
	int max_rssi_id;
;
}ue;

extern ue		g_ue;
extern			node	g_nd[WL_MAX_SELECTED_NODES_PER_ROOM];
extern int		g_node_ind;
extern wl_car_cell_circular_buff g_car_cb;

double wl_fix_to_double(uint8_t dh, uint8_t dl);
bool wl_get_2d_position(double *x, double *y);
int Cal2DSinglePos(node *nd1, node *nd2, ue *ue1);
int Cal2DRedundentNodePos(int NUM_NODES);
int Cal3DSinglePos(node *nd1, node *nd2, node *nd3, ue *ue1);
int Cal3DRedundentNodePos(int NUM_NODES);
//int Cal4DRedundentNodePos(int NUM_NODES, node nd[], ue *ue1, bool RSS);
void ResetUE(void);
void ResetNodes(void);
bool wl_pips_3d(int num_nodes, double *x, double *y, double *z);
void wl_alg_init(void);
void wl_get_node(wl_ue_scan_st *adv_node);
bool wl_get_3d_position(double *x, double *y, double *z);
bool wl_get_4d_position(double *x, double *y, double *z);
bool wl_pips_4d(int num_nodes, double *x, double *y, double *z);
int Cal4DRedundentNodePos(int NUM_NODES);
int Cal4DSinglePos(node *nd1, node *nd2, node *nd3, node *nd4, ue *ue1);
char* wl_m2f(double x);


#endif
