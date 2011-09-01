#ifndef __VOICEBOX_H__
#define __VOICEBOX_H__

#include "matrices.h"

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
} estnoisem_alg_param;

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
	estnoisem_alg_param qq;
} estnoisem_out_stat;

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
} specsub_alg_param;

/* estnoisem(). */
void estnoisem( Matrix yf, double tz, estnoisem_alg_param *pp, Matrix *out_x, estnoisem_out_stat *out_zo, Matrix *out_xs );
void estnoisem( Matrix yf, estnoisem_out_stat *tz, estnoisem_alg_param *pp, Matrix *out_x, estnoisem_out_stat *out_zo, Matrix *out_xs );

/* specsub(). */
void specsub();

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
