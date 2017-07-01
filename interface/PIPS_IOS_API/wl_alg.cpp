#include "stdafx.h"
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include "math.h"
#include "wl_config.h"
#include "wl_alg.h"

ue		g_ue;
node	g_nd[WL_MAX_SELECTED_NODES_PER_ROOM];
int		g_node_ind;

double wl_fix_to_double(uint8_t dh, uint8_t dl)
{
	double x;
	
	x = (double) (dh & 0x7F) + (0.00390625 * (dl & 0xFF) );
	if (dh & 0x80) {
		x = -x;
	}
		
	return x;
}


void wl_alg_init(void)
{	
	g_node_ind = 0;
	ResetNodes();
	ResetUE();
}

void wl_get_node(wl_ue_scan_st *adv_node)
{
	int n;
	
	if( (g_node_ind >= 0)&&(g_node_ind < WL_MAX_SELECTED_NODES_PER_ROOM) ) {
		g_nd[g_node_ind].x = wl_fix_to_double(adv_node->x[0], adv_node->x[1]);
		g_nd[g_node_ind].y = wl_fix_to_double(adv_node->y[0], adv_node->y[1]);
		g_nd[g_node_ind].z = wl_fix_to_double(adv_node->z[0], adv_node->z[1]);
		g_nd[g_node_ind].rss = adv_node->iir_frssi;
		g_nd[g_node_ind].max_r = adv_node->max_r;
		g_nd[g_node_ind].id = adv_node->crn_id[6];
		
		// Hard code node locations ================
		if(g_nd[g_node_ind].id == 1) {
			g_nd[g_node_ind].x = 0;
			g_nd[g_node_ind].y = 0;
		}
		else if(g_nd[g_node_ind].id == 2) {
			g_nd[g_node_ind].x = 0;
			g_nd[g_node_ind].y = 1.05;
		}
		else if(g_nd[g_node_ind].id == 3) {
			g_nd[g_node_ind].x = 0.98;
			g_nd[g_node_ind].y = -0.05;
		}
		else if(g_nd[g_node_ind].id == 4) {
			g_nd[g_node_ind].x = 0.98;
			g_nd[g_node_ind].y = 1.10;
		}

		// =========================================
		
		n = adv_node->tx_pow;
		if( n < WL_SCAN_RSSI_RANG_START) {
			n = WL_SCAN_RSSI_RANG_START;
		}
		else if(n >= WL_SCAN_RSSI_RANG) {
			n = WL_SCAN_RSSI_RANG-1;
		}
		else {
			// error in received tx_pow
		}
		g_nd[g_node_ind].E = rssi_lin[n - WL_SCAN_RSSI_RANG_START];
		//g_nd[g_node_ind].E = rssi_lin[50 - WL_SCAN_RSSI_RANG_START];

		if(g_nd[g_node_ind].rss >= rssi_lin[WL_SCAN_RSSI_RANG-1]) {
			g_nd[g_node_ind].pr = g_nd[g_node_ind].E/g_nd[g_node_ind].rss;
		}
		else {
			g_nd[g_node_ind].pr = 0;
			// msg = error
		}

		/*
		if( (g_nd[g_node_ind].id >= 1)&&(g_nd[g_node_ind].id <= 8) ) {
			g_nd[g_node_ind].E = g_e0[g_nd[g_node_ind].id];	
		}
		*/
		
		
		DBG_FILE(pf_log, "\n    g_node_ind = %d, node #%d, (x,y,z)=(%5.2f, %5.2f, %5.2f), rss=%3.1f, E=%3.1f, max_r = %d", g_node_ind, g_nd[g_node_ind].id,
					g_nd[g_node_ind].x, g_nd[g_node_ind].y, g_nd[g_node_ind].z, 10*log10(g_nd[g_node_ind].rss), 10*log10(g_nd[g_node_ind].E), g_nd[g_node_ind].max_r);
				
		g_node_ind++;

	}
}

bool wl_get_4d_position(double *x, double *y, double *z)
{
	bool pass;
	
	if( (g_node_ind >= 4)&&(g_node_ind < WL_MAX_SELECTED_NODES_PER_ROOM) ) {
		pass = wl_pips_4d(g_node_ind, x, y, z);
		return pass;
	}

	return false;
}

bool wl_pips_4d(int num_nodes, double *x, double *y, double *z)
{
	Cal4DRedundentNodePos(num_nodes);
	if(g_ue.numRdntMeasure >= 1) {
		*x = g_ue.x;
		*y = g_ue.y;
		*z = g_ue.z;
	}
	else {
		return false;
	}
		
	return true;
}

int Cal4DRedundentNodePos(int NUM_NODES)
{
	int i, j, k, l, m=0, N=0;
	double mx=0, my=0, mz=0;
	node *ndIn[4];
	ue ue1;
	
	ue1.dim = 4;
	
	// calculate a ue position by all nodes
	for(i=0; i<NUM_NODES; i++) {
		for(j=i+1; j<NUM_NODES; j++) {
			for(k=j+1; k<NUM_NODES; k++) {
				for(l=k+1; l<NUM_NODES; l++) {

					// select 3 nodes
					ndIn[0] = &g_nd[i];
					ndIn[1] = &g_nd[j];
					ndIn[2] = &g_nd[k];
					ndIn[3] = &g_nd[l];

					m++;
				
					/*
					if(cfg.LOG_SINGLE_UE_POS) {
						fprintf(pf1, "\n%2d  nd1=(%5.2f, %5.2f, %5.2f)m, nd2=(%5.2f, %5.2f, %5.2f)m, nd3=(%5.2f, %5.2f, %5.2f)m:\n", m, ndIn[0]->x, ndIn[0]->y, ndIn[0]->z, ndIn[1]->x, ndIn[1]->y, ndIn[1]->z, ndIn[2]->x, ndIn[2]->y, ndIn[2]->z);
						fprintf(pf1, "   ideal: xyz=(%5.2f, %5.2f, %5.2f)m <=> ri=(%5.2f, %5.2f, %5.2f)m <=> rss=(%5.2e, %5.2e, %5.2e)mW <=> rssi=(%5.2f, %5.2f, %5.2f)dBm, RxSNR=(99.9dB, 99.9dB, 99.9dB), Err(dr, eq)=(%3.0f, %5.2f)cm\n", 
								  cfg.x, cfg.y, cfg.z, cfg.r[i], cfg.r[j], cfg.r[k], cfg.rss[i], cfg.rss[j], cfg.rss[k], cfg.rssi[i], cfg.rssi[j], cfg.rssi[k], 0, 0 );
					}
					*/

					// calculate ue position by RSS, in mWatt linear power
					if(!Cal4DSinglePos(ndIn[0], ndIn[1], ndIn[2], ndIn[2], &ue1) ) {
						N++;
						mx += ue1.x;
						my += ue1.y;
						mz += ue1.z;
					
						DBG_FILE(pf_alg, "\n    N = %d, nodes = (#%d, #%d, #%d), (x,y,z) = (%4.2f, %4.2f, %4.2f)", N, ndIn[0]->id, ndIn[1]->id, ndIn[2]->id, ue1.x, ue1.y, ue1.z);

						/*
						if(cfg.LOG_SINGLE_UE_POS) {
							drErr = 100 * sqrt( sq(ue1->x - cfg.x) + sq(ue1->y - cfg.y) + sq(ue1->z - cfg.z) );
							fprintf(pf1, "     rss: xyz=(%5.2f, %5.2f, %5.2f)m <=> ri=(%5.2f, %5.2f, %5.2f)m <=> rss=(%5.2e, %5.2e, %5.2e)mW <=> rssi=(%5.2f, %5.2f, %5.2f)dBm, RxSNR=(%4.1fdB, %4.1fdB, %4.1fdB), Err(dr, eq)=(%3.0f, %5.2f)cm\n", 
								  ue1->x, ue1->y, ue1->z, ndIn[0]->r, ndIn[1]->r, ndIn[2]->r, ndIn[0]->rss, ndIn[1]->rss, ndIn[2]->rss, powTodB(ndIn[0]->rss), powTodB(ndIn[1]->rss), powTodB(ndIn[2]->rss), ndIn[0]->snr, ndIn[1]->snr, ndIn[2]->snr, drErr, ue1->eqErr);
						}
						*/
					}
				}
			}
		}
	}

	if(N) {
		g_ue.x = mx/N;
		g_ue.y = my/N;
		g_ue.z = mz/N;
	}

	g_ue.numRdntMeasure = N;

	return 0;
}


int Cal4DSinglePos(node *nd1, node *nd2, node *nd3, node *nd4, ue *ue1)
{
	int i, j, n, rt, m=0;
	double rssi, rssj;
	node *nd[4], nda, ndb, ndc;

	ue1->dim = 4;

	nd[0] = nd1;
	nd[1] = nd2;
	nd[2] = nd3;
	nd[3] = nd4;

	for (i=0; i<4; i++) {
		for(j=i+1; j<4; j++) {
			rssi = nd[i]->rss;
			rssj = nd[j]->rss;

			if( fabs(rssi - rssj) > 0.01 * rssi ) {
				m++;
				if( m == 1 ) {
					nda.x = (rssi * nd[i]->x - rssj * nd[j]->x)/(rssi - rssj);
					nda.y = (rssi * nd[i]->y - rssj * nd[j]->y)/(rssi - rssj);
					nda.z = nd[i]->z;
					nda.pr = ( sq(nd[i]->x - nd[j]->x) + sq(nd[i]->y - nd[j]->y) + sq(nd[i]->z - nd[j]->z) ) * rssi * rssj/sq(rssi - rssj);
					//fprintf(pf1, "            m=%d, (rssi-rssj) = %06.2e, nda_xyz=(%6.2f, %6.2f, %6.2f), (rssi*rssj)/sq(rssi-rssj)=%6.2e, nda_pr=%6.2f\n", m, (rssi - rssj), nda.x, nda.y, nda.z, rssi * rssj/sq(rssi - rssj), nda.pr);
				}
				else if( m == 2 ) {
					ndb.x = (rssi * nd[i]->x - rssj * nd[j]->x)/(rssi - rssj);
					ndb.y = (rssi * nd[i]->y - rssj * nd[j]->y)/(rssi - rssj);
					ndb.z = nd[i]->z;
					ndb.pr = ( sq(nd[i]->x - nd[j]->x) + sq(nd[i]->y - nd[j]->y) + sq(nd[i]->z - nd[j]->z) ) *rssi * rssj/sq(rssi - rssj);
					//fprintf(pf1, "            m=%d, (rssi-rssj) = %06.2e, ndb_xyz=(%6.2f, %6.2f, %6.2f), (rssi*rssj)/sq(rssi-rssj)=%6.2e, ndb_pr=%6.2f\n", m, (rssi - rssj), ndb.x, ndb.y, ndb.z, rssi * rssj/sq(rssi - rssj), ndb.pr);
				}
 				else if( m == 3 ) {
					ndc.x = (rssi * nd[i]->x - rssj * nd[j]->x)/(rssi - rssj);
					ndc.y = (rssi * nd[i]->y - rssj * nd[j]->y)/(rssi - rssj);
					ndc.z = nd[i]->z;
					ndc.pr = ( sq(nd[i]->x - nd[j]->x) + sq(nd[i]->y - nd[j]->y) + sq(nd[i]->z - nd[j]->z) ) *rssi * rssj/sq(rssi - rssj);
					//fprintf(pf1, "            m=%d, (rssi-rssj) = %06.2e, ndc_xyz=(%6.2f, %6.2f, %6.2f), (rssi*rssj)/sq(rssi-rssj)=%6.2e, ndc_pr=%6.2f\n", m, (rssi - rssj), ndc.x, ndc.y, ndc.z, rssi * rssj/sq(rssi - rssj), ndc.pr);
					rt = Cal3DSinglePos(&nda, &ndb, &ndc, ue1);
					if(rt == 0) {
						for(n=0; n<4; n++) {
							nd[n]->r = sqrt( sq(ue1->x - nd[n]->x) + sq(ue1->y - nd[n]->y) + sq(ue1->z - nd[n]->z) );
						}
						break;
					}
					else {
						m = 2;
						//fprintf(pf1, "            Cal3DSinglePos() failed at ue1.pass=%d\n", n);
					}
				}
			}
		}
		if(m == 3)
			break;
	}

	if(m < 3 )
		return 1;

	return 0;
}

bool wl_get_3d_position(double *x, double *y, double *z)
{
	bool pass;
	int i, n, m, max_id;
	double max_rssi; 
	double inCarThr = pow(10, WL_IN_CAR_THR_DB/10.0);
	double rssiThr = pow(10, WL_MAX_IN_CAR_RSSI_THR_DB/10.0);
	
	if( (g_node_ind >= 3)&&(g_node_ind < WL_MAX_SELECTED_NODES_PER_ROOM) ) {
		g_ue.max_rssi_id = 0;
		n = 0;
		max_rssi = 0;
		for(i=0; i<g_node_ind; i++) {
			if(g_nd[i].rss >= inCarThr) {
				n++;
			}
			if(max_rssi < g_nd[i].rss) {
				max_rssi = g_nd[i].rss;
				max_id = i;
		}
		}

		if(g_node_ind <= 4) {			//car mode
			if(n >= 3) {
				g_ue.ueMode = UE_IN_CAR;
				for(i=0; i<g_node_ind; i++) {
					g_nd[i].E = rssi_lin[50 - WL_SCAN_RSSI_RANG_START];
					g_nd[i].pr = g_nd[i].E/g_nd[i].rss;
				}

				m = 0;
				if( (max_rssi != 0) && (n == 4) ) {
					for(i=0; i<4; i++) {
						if( (g_nd[i].rss/max_rssi) <= rssiThr) {
							m++;
						}
					}
				}
				if(m == 3) {
					g_ue.max_rssi_id = g_nd[max_id].id;
				}
			}
			else {
				g_ue.ueMode = UE_OUT_OF_CAR;
				for(i=0; i<g_node_ind; i++) {
					g_nd[i].E = rssi_lin[60 - WL_SCAN_RSSI_RANG_START];
					g_nd[i].pr = g_nd[i].E/g_nd[i].rss;
				}
			}
		}

		pass = wl_pips_3d(g_node_ind, x, y, z);
		return pass;
	}

	return false;
}

bool wl_pips_3d(int num_nodes, double *x, double *y, double *z)
{
	Cal3DRedundentNodePos(num_nodes);
	if(g_ue.numRdntMeasure >= 1) {
		*x = g_ue.x;
		*y = g_ue.y;
		*z = g_ue.z;
	}
	else {
		return false;
	}
		
	return true;
}

int Cal3DSinglePos(node *nd1, node *nd2, node *nd3, ue *ue1)
{

	double a1, b1, c1, a2, b2, c2, pr1, pr2, pr3, w, u, md;
	double rss1, rss2, rss3;
	double r1, r2, r3;
		
	ue1->pass = 0;

	if(ue1->dim ==3) {
		rss1 = nd1->rss;
		rss2 = nd2->rss;
		rss3 = nd3->rss;

		pr1 = nd1->E/rss1;
		pr2 = nd2->E/rss2;
		pr3 = nd3->E/rss3;

		nd1->pr = pr1;
		nd2->pr = pr2;
		nd3->pr = pr3;
	}
	else if(ue1->dim == 4) {
		pr1 = nd1->pr;
		pr2 = nd2->pr;
		pr3 = nd3->pr;
	}
	else {
		return 4;
	}

	r1 = sqrt(pr1);
	r2 = sqrt(pr2);
	r3 = sqrt(pr3);


	a1 = -2*(nd1->x - nd2->x);
	b1 = -2*(nd1->y - nd2->y);
	c1 = (pr1 - pr2) - (nd1->x * nd1->x - nd2->x * nd2->x) - (nd1->y * nd1->y - nd2->y * nd2->y);

	a2 = -2*(nd1->x - nd3->x);
	b2 = -2*(nd1->y - nd3->y);
	c2 = (pr1 - pr3) - (nd1->x * nd1->x - nd3->x * nd3->x) - (nd1->y * nd1->y - nd3->y *nd3->y);

	w = a1 * b2 - a2 * b1;
	if ( fabs(w) <= 0.01 * fabs(a1 * b2) ) {
		DBG_FILE(pf_alg, "\n    Cal3DSinglePos fail: w = %f, a1*b2 = %f", w, (a1*b2));
		return 1;
	}

	ue1->x = (c1*b2 - c2*b1)/w;
	ue1->y = (a1*c2 - a2*c1)/w;

	u = pr1 + pr2 + pr3;
	u = u - (ue1->x - nd1->x)*(ue1->x - nd1->x) - (ue1->x - nd2->x)*(ue1->x - nd2->x) - (ue1->x - nd3->x)*(ue1->x - nd3->x);
	u = u - (ue1->y - nd1->y)*(ue1->y - nd1->y) - (ue1->y - nd2->y)*(ue1->y - nd2->y) - (ue1->y - nd3->y)*(ue1->y - nd3->y);
	if(u < 0) {
		u = 0;
	}
	ue1->z  = nd1->z - sqrt( u/3 );

	nd1->r = sqrt( (ue1->x - nd1->x)*(ue1->x - nd1->x) + (ue1->y - nd1->y)*(ue1->y - nd1->y) + (ue1->z - nd1->z)*(ue1->z - nd1->z) );
	nd2->r = sqrt( (ue1->x - nd2->x)*(ue1->x - nd2->x) + (ue1->y - nd2->y)*(ue1->y - nd2->y) + (ue1->z - nd2->z)*(ue1->z - nd2->z) );
	nd3->r = sqrt( (ue1->x - nd3->x)*(ue1->x - nd3->x) + (ue1->y - nd3->y)*(ue1->y - nd3->y) + (ue1->z - nd3->z)*(ue1->z - nd3->z) );

	nd1->idea_rss = nd1->E/(nd1->r * nd1->r);
	nd2->idea_rss = nd2->E/(nd2->r * nd2->r);
	nd3->idea_rss = nd3->E/(nd3->r * nd3->r);

	if(fabs(nd1->idea_rss - nd1->rss) < 1e-8) {
			nd1->w = 100;
	}
	else {
			nd1->w = nd1->idea_rss/fabs(nd1->idea_rss - nd1->rss);
	}

	if(fabs(nd2->idea_rss - nd2->rss) < 1e-8) {
			nd2->w = 100;
	}
	else {
			nd2->w = nd2->idea_rss/fabs(nd2->idea_rss - nd2->rss);
	}

	if(fabs(nd3->idea_rss - nd3->rss) < 1e-8) {
			nd3->w = 100;
	}
	else {
			nd3->w = nd3->idea_rss/fabs(nd3->idea_rss - nd3->rss);
	}

	ue1->w = (nd1->w + nd2->w + nd3->w)/3;
	/*
	ue1->w = nd1->w;
	if(ue1->w > nd2->w) {
		ue1->w = nd2->w;
	}
	if(ue1->w > nd3->w) {
		ue1->w = nd3->w;
	}
	*/


	ue1->eqErr = sqrt( (nd1->r - r1)*(nd1->r - r1) + (nd2->r - r2)*(nd2->r - r2) + (nd3->r - r3)*(nd3->r - r3) );
	ue1->eqErr = ue1->eqErr * 100;

	// check equation error
	//if(ue1->dim ==3) {		
		md = nd1->r;
		if(nd2->r > md) {
			md = nd2->r;
		}
		if(nd3->r > md) {
			md = nd3->r;
		}
		md = 0.3 * md * 100;					//in cm, default=1cm
		//md = 1;
	//}
	//else {
	//	md = 1;
	//}

/* WangLabs
	if( ue1->eqErr > md ) {
		DBG_LOG("Cal3DSinglePos fail: eqErr = %f, md = %f", ue1->eqErr, md);
		return 3;
	}
*/	
	//DBG_FILE(pf_alg, "\n    Cal3DSinglePos fail: eqErr = %f, md = %f", ue1->eqErr, md);

	ue1->pass = 1;

	// check for rang limitation
	if( (nd1->r > nd1->max_r)||(nd2->r > nd2->max_r)||(nd3->r > nd3->max_r) ) {
		DBG_FILE(pf_alg, "\n    Cal3DSinglePos fail: nodes(#%d, #%d, #%d), ", nd1->id, nd2->id, nd3->id);
		DBG_FILE(pf_alg, "\nr/Max_r = (%f/%d, %f/%d, %f/%d)", nd1->r, nd1->max_r, nd2->r, nd2->max_r, nd3->r, nd3->max_r);	
		return 2;
	}

	ue1->pass = 2;

/*
	if(RSS) {
		nd1->pass = 1;
		nd2->pass = 1;
		nd3->pass = 1;
	}
	else {
		nd1->passi = 1;
		nd2->passi = 1;
		nd3->passi = 1;
	}
*/

	return 0;
}

int Cal3DRedundentNodePos(int NUM_NODES)
{
	int i, j, k, m=0, N=0;
	double mx=0, my=0, mz=0, drss1, drss2, drss3, drss_thr, w_sum=0;
	node *ndIn[3];
	ue ue1;
	
	ue1.dim = 3;
	
	// calculate a ue position by all nodes
	for(i=0; i<NUM_NODES; i++) {
		for(j=i+1; j<NUM_NODES; j++) {
			for(k=j+1; k<NUM_NODES; k++) {

				// select 3 nodes
				ndIn[0] = &g_nd[i];
				ndIn[1] = &g_nd[j];
				ndIn[2] = &g_nd[k];

				m++;
				
				/*
				if(cfg.LOG_SINGLE_UE_POS) {
					fprintf(pf1, "\n%2d  nd1=(%5.2f, %5.2f, %5.2f)m, nd2=(%5.2f, %5.2f, %5.2f)m, nd3=(%5.2f, %5.2f, %5.2f)m:\n", m, ndIn[0]->x, ndIn[0]->y, ndIn[0]->z, ndIn[1]->x, ndIn[1]->y, ndIn[1]->z, ndIn[2]->x, ndIn[2]->y, ndIn[2]->z);
					fprintf(pf1, "   ideal: xyz=(%5.2f, %5.2f, %5.2f)m <=> ri=(%5.2f, %5.2f, %5.2f)m <=> rss=(%5.2e, %5.2e, %5.2e)mW <=> rssi=(%5.2f, %5.2f, %5.2f)dBm, RxSNR=(99.9dB, 99.9dB, 99.9dB), Err(dr, eq)=(%3.0f, %5.2f)cm\n", 
							  cfg.x, cfg.y, cfg.z, cfg.r[i], cfg.r[j], cfg.r[k], cfg.rss[i], cfg.rss[j], cfg.rss[k], cfg.rssi[i], cfg.rssi[j], cfg.rssi[k], 0, 0 );
				}
				*/

				// calculate ue position by RSS, in mWatt linear power
				if(!Cal3DSinglePos(ndIn[0], ndIn[1], ndIn[2], &ue1) ) {
					/* old alg
					N++;
					mx += ue1.x;
					my += ue1.y;
					mz += ue1.z;
					*/
					
					drss_thr = 30;
					drss1 = 10*log(ndIn[0]->rss/ndIn[0]->idea_rss);
					drss2 = 10*log(ndIn[1]->rss/ndIn[1]->idea_rss);
					drss3 = 10*log(ndIn[2]->rss/ndIn[2]->idea_rss);
					
					DBG_FILE(pf_alg, "\n    N = %d, nodes = (#%d, #%d, #%d), (x,y,z) = (%4.2f, %4.2f, %4.2f)", N, ndIn[0]->id, ndIn[1]->id, ndIn[2]->id, ue1.x, ue1.y, ue1.z);
					DBG_FILE(pf_alg, ",  drssi={%5.2f, %5.2f, %5.2f}", drss1,  drss2,  drss3); 
					DBG_FILE(pf_alg, ",  nd_w={%5.3f, %5.3f, %5.3f}, ue_w=%5.3f, drssi_thr=%4.1f", ndIn[0]->w, ndIn[1]->w, ndIn[2]->w, ue1.w, drss_thr);

					if( (fabs(drss1) < drss_thr) && (fabs(drss2) < drss_thr) && (fabs(drss3) < drss_thr) ) {
						if( (g_ue.ueMode != UE_IN_CAR) || ((g_ue.ueMode == UE_IN_CAR)&&(ue1.x > 0)&&(ue1.x < 2)&&(ue1.y > 0)&&(ue1.y < 1.5)) ) {
						N++;
						w_sum += ue1.w;
						mx += ue1.w * ue1.x;
						my += ue1.w * ue1.y;
						mz += ue1.w * ue1.z;
					}
						else {
							ue1.w = 0;
						}
					}
					else {
						ue1.w = 0;
					}

					if(m < 5) {
						sprintf_s(g_dbg_msg[3+m], "nd={%d,%d,%d} xy={%4.2f,%4.2f} drss={%4.1f,%4.1f,%4.1f} w=%4.2f",
										ndIn[0]->id, ndIn[1]->id, ndIn[2]->id, ue1.x, ue1.y, drss1,  drss2,  drss3, ue1.w);
					}					

				}
			}
		}
	}

	if(w_sum) {
		g_ue.x = mx/w_sum;
		g_ue.y = my/w_sum;
		g_ue.z = mz/w_sum;

		char carMode[20];
		if(g_ue.ueMode == UE_IN_CAR)
			sprintf_s(carMode, "IN_CAR");
		else if(g_ue.ueMode == UE_OUT_OF_CAR)
			sprintf_s(carMode, "OUT_OF_CAR");
		else {
			sprintf_s(carMode, "CarMode error");
		}
		sprintf_s(g_dbg_msg[8], "xyz={%4.2f,%4.2f,%4.2f} thr=%2.0f, %s ", g_ue.x, g_ue.y, g_ue.z, drss_thr, carMode);
	}

	/* old alg
	if(N) {
		g_ue.x = mx/N;
		g_ue.y = my/N;
		g_ue.z = mz/N;
	}
	*/

	g_ue.numRdntMeasure = N;

	return 0;
}

bool wl_get_2d_position(double *x, double *y)
{
	if( (g_node_ind >= 2)&&(g_node_ind < WL_MAX_SELECTED_NODES_PER_ROOM) ) {
		//Cal2DRedundentNodePos(g_node_ind);
		Cal2DRedundentNodePos(2);
		if(g_ue.numRdntMeasure >= 1) {
			*x = g_ue.x;
			*y = g_ue.y;
			return true;
		}
		else {
			return false;
		}
	}

	return false;
}

int Cal2DRedundentNodePos(int NUM_NODES)
{
	int i, j, m=0, N=0;
	double mx=0, my=0;
	node *ndIn[2];
	ue ue1;
	
	ue1.dim = 3;
	
	// calculate a ue position by all nodes
	for(i=0; i<NUM_NODES; i++) {
		for(j=i+1; j<NUM_NODES; j++) {

			// select 3 nodes
			ndIn[0] = &g_nd[i];
			ndIn[1] = &g_nd[j];

			m++;
			
			/*
			if(cfg.LOG_SINGLE_UE_POS) {
				fprintf(pf1, "\n%2d  nd1=(%5.2f, %5.2f, %5.2f)m, nd2=(%5.2f, %5.2f, %5.2f)m, nd3=(%5.2f, %5.2f, %5.2f)m:\n", m, ndIn[0]->x, ndIn[0]->y, ndIn[0]->z, ndIn[1]->x, ndIn[1]->y, ndIn[1]->z, ndIn[2]->x, ndIn[2]->y, ndIn[2]->z);
				fprintf(pf1, "   ideal: xyz=(%5.2f, %5.2f, %5.2f)m <=> ri=(%5.2f, %5.2f, %5.2f)m <=> rss=(%5.2e, %5.2e, %5.2e)mW <=> rssi=(%5.2f, %5.2f, %5.2f)dBm, RxSNR=(99.9dB, 99.9dB, 99.9dB), Err(dr, eq)=(%3.0f, %5.2f)cm\n", 
						  cfg.x, cfg.y, cfg.z, cfg.r[i], cfg.r[j], cfg.r[k], cfg.rss[i], cfg.rss[j], cfg.rss[k], cfg.rssi[i], cfg.rssi[j], cfg.rssi[k], 0, 0 );
			}
			*/

			// calculate ue position by RSS, in mWatt linear power
			if(!Cal2DSinglePos(ndIn[0], ndIn[1], &ue1) ) {
				N++;
				mx += ue1.x;
				my += ue1.y;
				
				//DBG_FILE(pf_alg, "\n    N = %d, nodes = (#%d, #%d), (x, y) = (%4.1f, %4.1f)", N, ndIn[0]->id, ndIn[1]->id, ue1.x, ue1.y);
				DBG_FILE(pf_alg, "\n    nodes = (#%d, #%d), (x, y) = (%4.1f, %4.1f)", ndIn[0]->id, ndIn[1]->id, ue1.x, ue1.y);

				/*
				if(cfg.LOG_SINGLE_UE_POS) {
					drErr = 100 * sqrt( sq(ue1->x - cfg.x) + sq(ue1->y - cfg.y) + sq(ue1->z - cfg.z) );
					fprintf(pf1, "     rss: xyz=(%5.2f, %5.2f, %5.2f)m <=> ri=(%5.2f, %5.2f, %5.2f)m <=> rss=(%5.2e, %5.2e, %5.2e)mW <=> rssi=(%5.2f, %5.2f, %5.2f)dBm, RxSNR=(%4.1fdB, %4.1fdB, %4.1fdB), Err(dr, eq)=(%3.0f, %5.2f)cm\n", 
						  ue1->x, ue1->y, ue1->z, ndIn[0]->r, ndIn[1]->r, ndIn[2]->r, ndIn[0]->rss, ndIn[1]->rss, ndIn[2]->rss, powTodB(ndIn[0]->rss), powTodB(ndIn[1]->rss), powTodB(ndIn[2]->rss), ndIn[0]->snr, ndIn[1]->snr, ndIn[2]->snr, drErr, ue1->eqErr);
				}
				*/
			}
		}
	}

	if(N) {
		g_ue.x = mx/N;
		g_ue.y = my/N;
	}

	g_ue.numRdntMeasure = N;

	return 0;
}

int Cal2DSinglePos(node *nd1, node *nd2, ue *ue1)
{
	// only for case of node1 xy=(0, 0); node2 xy = (0, d)
	double rss1, rss2, pr1, pr2, u;

	rss1 = nd1->rss;
	rss2 = nd2->rss;

	pr1 = nd1->E/rss1;
	pr2 = nd2->E/rss2;

	nd1->pr = pr1;
	nd2->pr = pr2;

	ue1->y = (pr1 - pr2 + (nd2->y)*(nd2->y))/(2 * nd2->y);
	u = pr1 - (ue1->y)*(ue1->y);
	if(u < 0)
		u = 0;
	ue1->x = sqrt(u);

	return 0;
}


void ResetUE(void)
{
	g_ue.dim = 0;
	g_ue.eqErr = 0;
	g_ue.id = 0;
	g_ue.numRdntMeasure = 0;
	g_ue.pass = 0;
	g_ue.x = 0;
	g_ue.y = 0;
	g_ue.z = 0;
	g_ue.ueMode = UE_SEARCH;
	g_ue.max_rssi_id = 0;
}


void ResetNodes(void)
{
	int i;

	for(i=0; i<WL_MAX_SELECTED_NODES_PER_ROOM; i++) {
		g_nd[i].id = 0;
		g_nd[i].pr = 0;
		g_nd[i].r = 0;		
		g_nd[i].x = 0;
		g_nd[i].y = 0;
		g_nd[i].z = 0;
		g_nd[i].rss = 0;
		g_nd[i].E = 0;
		g_nd[i].max_r = 0;
		g_nd[i].tx_pow = 0;
		g_nd[i].ble_ch = 0;
		g_nd[i].pass = 0;
	}
	
}

