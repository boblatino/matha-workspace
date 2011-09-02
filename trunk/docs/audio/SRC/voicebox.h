#ifndef __VOICEBOX_H__
#define __VOICEBOX_H__

#include "matrices.h"

/*typedef struct
{
	double taca;
	double tamax;
	double taminh;
	double tpfall;
	double tbmax;
	double qeqmin;
	double qeqmax;
	double av;
	double td;
	double nu;
	Matrix qith;
	Matrix nsmdb;
} alg_param;
*/
typedef struct
{
	double taca;
	double tamax;
	double taminh;
	double tpfall;
	double tbmax;
	double qeqmin;
	double qeqmax;
	double av;
	double td;
	double nu;
	Matrix qith;
	Matrix nsmdb;
	double of;
	double ti;
	double ri;
	double g;
	double e;
	double am;
	double b;
	double al;
	double ah;
	double bt;
	double mx;
	double gh;
} alg_param;

typedef struct
{
	double nrcum;
	Matrix p;
	Matrix ac;
	Matrix sn2;
	Matrix pb;
	Matrix pb2;
	Matrix pminu;
	Matrix actmin;
	Matrix actminsub;
	double subwc;
	Matrix actbuf;
	double ibuf;
	Matrix lminflag;
	double tinc;
	alg_param qq;
} estnoisem_out_stat;

typedef struct
{
	double nrcum;
	Matrix p;
	Matrix ac;
	Matrix sn2;
	Matrix pb;
	Matrix pb2;
	Matrix pminu;
	Matrix actmin;
	Matrix actminsub;
	double subwc;
	Matrix actbuf;
	double ibuf;
	Matrix lminflag;
	double tinc;
	alg_param qq;
	double fs;
	Matrix si;
	Matrix ssv;
	alg_param qp;
	estnoisem_out_stat ze;
}specsub_out_stat;

void init_specsub_out_stat( specsub_out_stat x );
void init_estnoisem_out_stat( estnoisem_out_stat x );
void init_alg_param( alg_param x );

/* estnoisem(). */
void estnoisem( Matrix yf, double tz, alg_param *pp, Matrix *out_x, estnoisem_out_stat *out_zo, Matrix *out_xs );
void estnoisem( Matrix yf, estnoisem_out_stat *tz, alg_param *pp, Matrix *out_x, estnoisem_out_stat *out_zo, Matrix *out_xs );

/* specsub(). */
void specsub( Matrix si, /*struct*/ double fsz, alg_param *pp, Matrix *out_ss, specsub_out_stat *out_zo );
void specsub( Matrix si, specsub_out_stat *fsz, alg_param *pp, Matrix *out_ss, specsub_out_stat *out_zo );

/* rfft(). */
Matrix rfft( Matrix x );
Matrix rfft( Matrix x, size_t n );
Matrix rfft( Matrix x, size_t n, size_t d );

/* irfft(). */
Matrix irfft( Matrix y );
Matrix irfft( Matrix y, size_t n );
Matrix irfft( Matrix y, size_t n, size_t d );

/* enframe(). */
Matrix enframe( Matrix x, Matrix win );
Matrix enframe( Matrix x, Matrix win, size_t len );


#endif//__VOICEBOX_H__
