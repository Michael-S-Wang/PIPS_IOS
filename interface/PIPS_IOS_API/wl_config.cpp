#include "stdafx.h"
#include "stdio.h"
#include "wl_config.h"

FILE* pf_log;
FILE* pf_rssi;
FILE* pf_alg;
FILE* pf_api_out;
FILE* pf_ant;

wl_car_cell_circular_buff g_car_cb;

char g_dbg_msg[10][60];

const char *pos_name[]= {
    "NO_NODES_ARE_FOUND",
    "NOT_ENOUGH_NODES",
    "FINDING_POSITION",
    "DRIVER_SEAT",
    "FRONT_PASSENGER",
    "BACK_LEFT_PASSENGER",
    "BACK_MIDDLE_PASSENGER",
    "BACK_RIGHT_PASSENGER",
    "OUT_OF_CAR_LEFT",
    "OUT_OF_CAR_RIGHT",
    "OUT_OF_CAR_FRONT",
    "OUT_OF_CAR_BACK",
    "OUT_OF_CAR_LEFT_FRONT",
    "OUT_OF_CAR_LEFT_BACK",
    "OUT_OF_CAR_RIGHT_FRONT",
    "OUT_OF_CAR_RIGHT_BACK"
};

/*
uint8_t wl_node_adv_data[WL_MAX_ADV_DATA_LEN]={		// adv packet format for Atmel: 28 bytes data + 3 bytes PDU header = total 31 bytes
	0x1B, 0x09,						//  0: 2 bytes for data len and type, after PDU header of 0x02, 0x01, 0x06(Atmel)/0x1A(iPhone) 
	0x57, 0x4C, 0x41, 0x42,			//  2: 4 bytes for "WLAB" 
	0x01, 0x02,						//  6: 2 bytes for x, 16-bit in Q8 format, in meter 	
	0x03, 0x04,						//  8: 2 bytes for y, 16-bit in Q8 format, 1-bit for sign, 7-bit for integer, 8-bit for decimal 
	0x05, 0x06,						// 10: 2 bytes for z, 16-bit in Q8 format, 1-bit for sign: 0="+", 1="-" 
	0x2E,							// 12: 1 byte for tx_pow, 8-bit, in -dBm 
	0xAB, 0xCD, 0xEF, 				// 13: 3 bytes for customer id, 0-16,777,215 
	0x11, 0x22,  					// 16: 2 bytes for room id, 0-65,535 
	0xFF, 0x01, 					// 18: 2 bytes for node id, 0-65,535 
	0x20, 0x21, 0x22, 0x23,			// 20: 8 bytes are reserved for future prove 
	0x24, 0x25, 0x26, 0x27
};	
*/

uint8_t wl_node_adv_data[WL_MAX_ADV_DATA_LEN]={		// avd packet format for iBeacon: 27 bytes data + 3 bytes PDU header = total 30 bytes
	// iBeacon header
	0x1A, 0xFF, 					// 0: 2 bytes for length and type (manufacturer specific data), after PDU header of 0x02, 0x01, 0x04(Nordic, 0x1A-iPhone)
	0x4C, 0x00,						// 2: 2 bytes for company ID;  0x5900-Nordic
	0x02, 0x15,						// 4: 2 bytes for beacon type, data length

	// UUID
	0x57, 0x4C, 0x41, 0x42,			// 6: 4 bytes for "WLAB", part of UUID
	0x10, 0x11,						// 10: 2 bytes for x, 16-bit in Q8 format, in meter 
	0x12, 0x13,						// 12: 2 bytes for y, 16-bit in Q8 format, 1-bit for sign, 7-bit for integer, 8-bit for decimal
	0x14, 0x15,						// 14: 2 bytes for z, 16-bit in Q8 format, 1-bit for sign: 0="+", 1="-"
	0x16, 0x17,						// 16: 6 bytes. for car, 6 bytes for vehicle plate number 
	0x18, 0x19,						//              for indoor, 4 bytes for customer id, and 2 bytes for room id 
	0x20, 0x21,						
	
	// Major
	0x22, 							// 22: 1 byte for node id			
	0x23,							// 23: 1 byte for BLE chasnnel for current RSSI: 0=FH, 1=CH37, 2=CH38, 3=CH39

	// Minor
	0x24, 							// 24: 2 bytes. reserved
	0x25,							
	
	// TX power
	0x26							// 26: 1 byte for node transmitted power
};	


double rssi_lin[WL_SCAN_RSSI_RANG] = {
		0.01,
		0.007943282,
		0.006309573,
		0.005011872,
		0.003981072,
		0.003162278,
		0.002511886,
		0.001995262,
		0.001584893,
		0.001258925,
		0.001,
		0.000794328,
		0.000630957,
		0.000501187,
		0.000398107,
		0.000316228,
		0.000251189,
		0.000199526,
		0.000158489,
		0.000125893,
		0.0001,
		7.94328E-05,
		6.30957E-05,
		5.01187E-05,
		3.98107E-05,
		3.16228E-05,
		2.51189E-05,
		1.99526E-05,
		1.58489E-05,
		1.25893E-05,
		0.00001,
		7.94328E-06,
		6.30957E-06,
		5.01187E-06,
		3.98107E-06,
		3.16228E-06,
		2.51189E-06,
		1.99526E-06,
		1.58489E-06,
		1.25893E-06,
		0.000001,
		7.94328E-07,
		6.30957E-07,
		5.01187E-07,
		3.98107E-07,
		3.16228E-07,
		2.51189E-07,
		1.99526E-07,
		1.58489E-07,
		1.25893E-07,
		0.0000001,
		7.94328E-08,
		6.30957E-08,
		5.01187E-08,
		3.98107E-08,
		3.16228E-08,
		2.51189E-08,
		1.99526E-08,
		1.58489E-08,
		1.25893E-08,
		0.00000001,
		7.94328E-09,
		6.30957E-09,
		5.01187E-09,
		3.98107E-09,
		3.16228E-09,
		2.51189E-09,
		1.99526E-09,
		1.58489E-09,
		1.25893E-09,
		0.000000001,
		7.94328E-10,
		6.30957E-10,
		5.01187E-10,
		3.98107E-10,
		3.16228E-10
};

/*
char g_st[30];

double g_e0[9] = {0,
	1.26908E-05,
	1.56793E-05,
	4.01317E-05,
	2.57281E-05,
	1.30311E-05,
	2.78106E-05,
	1.78589E-05,
	1.41383E-05
};

char* wl_m2f(double x) 
{
	int ft;
	uint8_t c1= 0x27, c2 = 0x22;
	double a, b;
	
	a = 3.28084 * wl_fabs(x);
	ft = (int)a;
	b = (a - ft)*12;
	
	if(x < 0) {
		ft = - ft;
	}
		
	sprintf(g_st, "%d%c%1.0f%c", ft, c1, b, c2);
	
	return g_st;
}
*/

void ClearCarCBuf(void)
{
	int i;

	for(i=0; i<CAR_CELL_CBUF; i++) {
		g_car_cb.carCellCBuf[i] = 0;
	}
	g_car_cb.wpt = 0;
	g_car_cb.rpt = 0;
	g_car_cb.deep = 0;
}

int memcmp(uint8_t* data1, uint8_t* data2, int len)
{
	int i;

	for(i=0; i<len; i++) {
		if(data1[i] != data2[i]) {
			return 1;
		}
	}

	return 0;
}
