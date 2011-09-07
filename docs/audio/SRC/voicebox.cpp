#include "voicebox.h"
#include <cmath>
#include <cstring>

#include <iostream>

using namespace std;

void init_alg_param( alg_param x )
{
	Matrix empty;
	x.taca = NAN;
	x.tamax = NAN;
	x.taminh = NAN;
	x.tpfall = NAN;
	x.tbmax = NAN;
	x.qeqmin = NAN;
	x.qeqmax = NAN;
	x.av = NAN;
	x.td = NAN;
	x.nu = NAN;
	x.qith = empty;
	x.nsmdb = empty;
	x.of = NAN;
	x.ti = NAN;
	x.ri = NAN;
	x.g = NAN;
	x.e = NAN;
	x.am = NAN;
	x.b = NAN;
	x.al = NAN;
	x.ah = NAN;
	x.bt = NAN;
	x.mx = NAN;
	x.gh = NAN;
}

void init_estnoisem_out_stat( estnoisem_out_stat x )
{
	Matrix empty;
	x.nrcum = NAN;
	x.p = empty;
	x.ac = empty;
	x.sn2 = empty;
	x.pb = empty;
	x.pb2 = empty;
	x.pminu = empty;
	x.actmin = empty;
	x.actminsub = empty;
	x.subwc = NAN;
	x.actbuf = empty;
	x.ibuf = NAN;
	x.lminflag = empty;
	x.tinc = NAN;
	init_alg_param( x.qq );
}

void init_specsub_out_stat( specsub_out_stat x )
{
	Matrix empty;
	x.nrcum = NAN;
	x.p = empty;
	x.ac = empty;
	x.sn2 = empty;
	x.pb = empty;
	x.pb2 = empty;
	x.pminu = empty;
	x.actmin = empty;
	x.actminsub = empty;
	x.subwc = NAN;
	x.actbuf = empty;
	x.ibuf = NAN;
	x.lminflag = empty;
	x.tinc = NAN;
	init_alg_param( x.qq );
	x.fs = NAN;
	x.si = empty;
	x.ssv = empty;
	init_alg_param( x.qp );
	init_estnoisem_out_stat( x.ze );
}

void mhvals( double d, double *out_m, double *out_h  )
{
	static double dmh_d[][ 18 ] =
	{
		 {1,2,5,8,10,15,20,30,40,60,80,120,140,160,180,220,260,300},
		 {0,0.26,0.48,0.58,0.61,0.668,0.705,0.762,0.8,0.841,0.865,0.89,0.9,0.91,0.92,0.93,0.935,0.94},
		 {0,0.15,0.48,0.78,0.98,1.55,2,2.3,2.52,3.1,3.38,4.15,4.35,4.25,3.9,4.1,4.7,5}
	};

	static Matrix dmh;
	dmh.addColumn( dmh_d[ 0 ], 18 );
	dmh.addColumn( dmh_d[ 1 ], 18 );
	dmh.addColumn( dmh_d[ 2 ], 18 );

	complex <double> temp, temp2;
	size_t i, j;

	Matrix mat_i = find( dmh.getSubMatrix( 0, 0, 0, 17) >= d );
	if( mat_i.isempty() )
	{
		dmh.size( &j, &i);
		j = --i;
	}
	else
	{
		mat_i.getElement( &temp, 0, 0 );
		i = temp.real();
		j = i-1;
	}
	
	dmh.getElement( &temp, 0, i	);
	if( d == temp.real() )
	{
		if( out_m != NULL )
		{
			dmh.getElement( &temp, 1, i );
			*out_m = temp.real();
		}
		if( out_h != NULL )
		{
			dmh.getElement( &temp, 2, i );
			*out_h = temp.real();
		}
	}
	else
	{
		dmh.getElement( &temp, 0, i - 1 );
		double qj = sqrt( temp.real() );		 // interpolate using sqrt(d)
		dmh.getElement( &temp, 0, i );
		double qi = sqrt( temp.real() );
		double q = sqrt(d);

		if( out_h != NULL )
		{
			dmh.getElement( &temp, 2, i );
			dmh.getElement( &temp2, 2, j );
			*out_h = temp.real() + ( q - qi ) * ( temp2.real() - temp.real() ) / ( qj - qi );
		}
		if( out_m != NULL )
		{
			dmh.getElement( &temp, 1, i );
			dmh.getElement( &temp2, 1, j );
			*out_m = temp.real() + ( qi * qj / q - qj ) * ( temp2.real() - temp.real() ) / ( qi - qj );
		}
	}

}


void estnoisem( Matrix yf, estnoisem_out_stat *tz, alg_param *pp, Matrix *out_x, estnoisem_out_stat *out_zo, Matrix *out_xs )
{
	size_t nr, nrf;
	alg_param qq;
	init_alg_param( qq );
	double nrcum, subwc, ibuf, tinc;
	Matrix actmin, actminsub, actbuf, lminflag, sn2, p, pb, pb2, pminu, ac;
	double taca, tamax, taminh, tpfall, tbmax, qeqmin, qeqmax, av, td, nu;
	Matrix qith, nsmdb;


	yf.size( &nrf, &nr);		 // number of frames and freq bins
	if( out_x != NULL )
		*out_x = zeros(nr,nrf);			 // initialize output arrays
	if( out_xs != NULL )
		*out_xs = zeros(nr,nrf);			 // will hold std error in the future
								 // no real data
	if( yf.isempty() && out_zo != NULL )
		*out_zo = *tz;				 // just keep the same state
	else  // take parameters from a previous call
	{
		nrcum = tz->nrcum;
		p = tz->p;				 // smoothed power spectrum
		ac = tz->ac;				 // correction factor (9)
		sn2 = tz->sn2;			 // estimated noise power
		pb = tz->pb;				 // smoothed noisy speech power (20)
		pb2 = tz->pb2;
		pminu = tz->pminu;
		actmin = tz->actmin;		 // Running minimum estimate
		actminsub = tz->actminsub;// sub-window minimum estimate
		subwc = tz->subwc;		 // force a buffer switch on first loop
		actbuf = tz->actbuf;		 // buffer to store subwindow minima
		ibuf = tz->ibuf;
		lminflag = tz->lminflag;	 // flag to remember local minimum
		tinc = tz->tinc;			 // frame increment
		qq = tz->qq;				 // parameter structure
	}

	// unpack parameter structure

	taca = qq.taca;				 // smoothing time constant for alpha_c  =  -tinc/log(0.7) in equ (11)
	tamax = qq.tamax;			 // max smoothing time constant in (3)  =  -tinc/log(0.96)
	taminh = qq.taminh;			 // min smoothing time constant (upper limit) in (3)  =  -tinc/log(0.3)
	tpfall = qq.tpfall;			 // time constant for P to fall (12)
	tbmax = qq.tbmax;			 // max smoothing time constant in (20)  =  -tinc/log(0.8)
	qeqmin = qq.qeqmin;			 // minimum value of Qeq (23)
	qeqmax = qq.qeqmax;			 // max value of Qeq per frame
	av = qq.av;					 // fudge factor for bc calculation (23 + 13 lines)
	td = qq.td;					 // time to take minimum over
	nu = qq.nu;					 // number of subwindows
	qith = qq.qith;				 // noise slope thresholds in dB/s
	nsmdb = qq.nsmdb;			 // maximum permitted +ve noise slope in dB/s

	// derived algorithm constants

	double aca = exp( -tinc / taca );		 // smoothing constant for alpha_c in equ (11)  =  0.7
	double acmax = aca;				 // min value of alpha_c  =  0.7 in equ (11) also  =  0.7
	double amax = exp( -tinc / tamax );	 // max smoothing constant in (3)  =  0.96
	double aminh = exp( -tinc / taminh );	 // min smoothing constant (upper limit) in (3)  =  0.3
	double bmax = exp( -tinc / tbmax );	 // max smoothing constant in (20)  =  0.8
	double snrexp  =  -tinc / tpfall;
	double nv = round( td / ( tinc * nu ) );	 // length of each subwindow in frames
	double nd, md, hd, mv, hv;

	if( nv < 4 )				 // algorithm doesn't work for miniscule frames
	{
		nv = 4;
		nu = max( round( td / ( tinc * nv ) ), 1 );
	}
	nd = nu * nv;					 // length of total window in frames
	mhvals(nd, &md, &hd );		 // calculate the constants M(D) and H(D) from Table III
	mhvals(nv, &mv, &hv );		 // calculate the constants M(D) and H(D) from Table III
								 // [8 4 2 1.2] in paper
	Matrix nsms = power( 10, ( nsmdb * nv * tinc / 10 ) );
	double qeqimax = 1 / qeqmin;			 // maximum value of Qeq inverse (23)
	double qeqimin = 1 / qeqmax;			 // minumum value of Qeq per frame inverse

	if( yf.isempty() )			 // provide dummy initialization
	{
		ac = 1;					 // correction factor (9)
		subwc = nv;				 // force a buffer switch on first loop
		ibuf = 0;
		p = *out_x;					 // smoothed power spectrum
		sn2 = p;				 // estimated noise power
		pb = p;					 // smoothed noisy speech power (20)
		pb2 = power( pb, 2 );
		pminu = p;
								 // Running minimum estimate
		actmin = real( ones( 1, nrf ) * INFINITY ); //repmat(Inf,1,nrf);
		actminsub = actmin;		 // sub-window minimum estimate
								 // buffer to store subwindow minima
		actbuf = real( ones( nu, nrf ) * INFINITY ); //repmat(Inf,nu,nrf);
		lminflag = zeros(1,nrf); // flag to remember local minimum
	}
	else
	{
		if( !nrcum )			 // initialize values for first frame
		{
			p = yf.getSubMatrix( 0, nrf - 1, 0, 0 ); // yf(1,:);		 // smoothed power spectrum
			ac = 1;				 // correction factor (9)
			sn2 = p;			 // estimated noise power
			pb = p;				 // smoothed noisy speech power (20)
			pb2 = power( pb, 2);
			pminu = p;
								 // Running minimum estimate
			actmin = real( ones( 1, nrf ) * INFINITY ); //repmat(Inf,1,nrf);
			actminsub = actmin;	 // sub-window minimum estimate
			subwc = nv;			 // force a buffer switch on first loop
								 // buffer to store subwindow minima
			actbuf = real( ones( nu, nrf ) * INFINITY ); //repmat(Inf,nu,nrf);
			ibuf = 0;
								 // flag to remember local minimum
			lminflag = zeros(1,nrf);
		}

		// loop for each frame

		for( size_t t = 0; t < nr; t++ )			 // we use t instead of lambda in the paper
		{
			Matrix yft = yf.getSubMatrix( 0, nrf - 1, t, t ); // yf(t,:);		 // noise speech power spectrum
								 // alpha_c-bar(t)  (9)
			Matrix acb = power( power ( ( dotDivision( sum(p),sum(yft) ) - 1 ), 2) + 1, -1);
acb = real(acb);								 // alpha_c(t)  (10)
			ac = max( acb, acmax ) * ( 1 - aca ) + ac * aca;
								 // alpha_hat: smoothing factor per frequency (11)
			Matrix ah = dotProduct( power(  power( dotDivision(p,sn2)-1,2 ) + 1 , (-1) ), ac ) * amax;    // alpha_hat: smoothing factor per frequency (11)
			//Matrix ah = power( dotProduct( ac, power( dotDivision(p,sn2)-1,2 ) + 1 ), (-1) ) * amax;    // alpha_hat: smoothing factor per frequency (11)
			//ah = power( amax * dotProduct( ac, 1 + power( ( dotDivision( p, sn2 ) - 1 ), 2 ), -1);
ah = real(ah);
			Matrix snr = sum(p) / sum(sn2);
								 // lower limit for alpha_hat (12)
			ah = max( min( mpower( snr, snrexp ), aminh ), ah );

								 // smoothed noisy speech power (3)
			p = dotProduct( ah, p ) + dotProduct( ah*(-1) + 1, yft );
			Matrix b = min( power( ah, 2 ), bmax ); // smoothing constant for estimating periodogram variance (22 + 2 lines)
								 // smoothed periodogram (20)
			pb = dotProduct( b, pb ) + dotProduct( b * (-1) + 1, p );
								 // smoothed periodogram squared (21)
			pb2 = dotProduct( b, pb2 ) + dotProduct( b * (-1) + 1, power( p, 2 ) );

								 // Qeq inverse (23)
			Matrix qeqi = max( min( dotDivision( pb2 - power( pb, 2), power( sn2, 2 ) * 2 ), qeqimax ) , qeqimin / ( t + nrcum + 1 ) );
			Matrix qiav = sum(qeqi)/nrf; // Average over all frequencies (23+12 lines) (ignore non-duplication of DC and nyquist terms)
			Matrix bc = sqrt( qiav ) * av + 1; // bias correction factor (23+11 lines)
								 // we use the simplified form (17) instead of (15)
			Matrix bmind = dotDivision( 1 - md, power( qeqi, -1 ) - md * 2 ) * ( nd - 1 ) * 2 + 1;
								 // same expression but for sub windows
			Matrix bminv = dotDivision( 1 - mv, power( qeqi, -1 ) - mv * 2 ) * ( nv - 1 ) * 2 + 1;
								 // Frequency mask for new minimum
			Matrix kmod = ( bc * dotProduct( p, bmind) ) < actmin ;
			if( any( kmod ) )
			{
				size_t co, ro;
				kmod.size( &co, &ro );
				complex<double> temp;
				Matrix t1 =bc * dotProduct( p, bmind ); 
				Matrix t2 =bc * dotProduct( p, bminv ); 
				for( size_t i = 0; i < co; i++ )
					for( size_t j = 0; j < ro; j++ )
					{
						kmod.getElement( &temp, i, j );
						if( temp.real() == 0 )
							continue;
						t1.getElement( &temp, i, j );
						actmin.setElement( temp , i, j );
						t2.getElement( &temp, i, j );
						actminsub.setElement( temp , i, j );
					}
			}
								 // middle of buffer - allow a local minimum
			if( subwc>1 && subwc<nv )
			{
								 // potential local minimum frequency bins
				lminflag = lminflag | kmod;
				pminu = min(actminsub,pminu);
				sn2 = pminu;
			}
			else if( subwc >= nv )	 // end of buffer - do a buffer switch
			{
								 // increment actbuf storage pointer
				ibuf = 1 + rem( ibuf, nu );
								 // save sub-window minimum
				size_t col, row;
				actmin.size( &col, &row );
				complex<double> temp;
				for( size_t i = 0; i < col ; i++ )
				{
					actmin.getElement( &temp, i, 0 );
					actbuf.setElement( temp, i, ibuf - 1 );
				}
				//actbuf(ibuf,:) = actmin;

				pminu = min(actbuf);
				Matrix i = find(qiav<qith);
				//nsm = nsms(i(1));// noise slope max
				i.getElement( &temp, 0 );
				nsms.getElement( &temp, temp.real() );
				double nsm = temp.real();
				Matrix lmin = lminflag & ( !kmod ) & ( actminsub<pminu*nsm ) & ( actminsub>pminu);
				if( any(lmin) )
				{
					size_t co, ro;
					lmin.size( &co, &ro );
					complex<double> temp;
					for( size_t i = 0; i < co; i++ )
						for( size_t j = 0; j < ro; j++ )
						{
							kmod.getElement( &temp, i, j );
							if( temp.real() == 0 )
								continue;
							actminsub.getElement( &temp , i, j );
							pminu.setElement( temp , i, j );
							//actbuf(:,lmin) = repmat(pminu(lmin),nu,1);
							pminu.getElement( &temp, i, 0 );
							for( size_t k = 0; k < nu; k++ )
								actbuf.setElement( temp, i, k );
						}
				}
				lminflag.size( &col, &row );
				lminflag = zeros( row, col );
				actmin.size( &col, &row );
				actmin = real( ones( row, col) * INFINITY );
				subwc = 0;
			}
			subwc = subwc+1;
			//x(t,:) = sn2;
			size_t col, row, k = 0;
			complex<double> temp;
			sn2.size( &col, &row );
			if( out_x != NULL )
				for( size_t i = 0; i < col; i++ )
				{
					sn2.getElement( &temp, i, 0 );
					out_x->setElement( temp, k++, t );
				}
			Matrix qisq = sqrt(qeqi);
			// empirical formula for standard error based on Fig 15 of [2]
			if( out_xs != NULL )
			{
				Matrix txs = dotProduct( sn2, sqrt( dotDivision( dotProduct(qisq * 100 + nd, qisq ) / (1+0.005*nd+6/nd), power( qeqi, -1 ) * 0.5 + nd - 1 ) * 0.266 ) );
				complex<double> temp;
				for( size_t i = 0; i < nrf; i++ )
				{
					txs.getElement( &temp, i );
					out_xs->setElement( temp, i, t );
				}
			}
		}
	}
	if( out_zo != NULL )				 // we need to store the state for next time
	{
		init_estnoisem_out_stat( *out_zo );
		out_zo->nrcum = nrcum+nr;	 // number of frames so far
		out_zo->p = p;				 // smoothed power spectrum
		out_zo->ac = ac;				 // correction factor (9)
		out_zo->sn2 = sn2;			 // estimated noise power
		out_zo->pb = pb;				 // smoothed noisy speech power (20)
		out_zo->pb2 = pb2;
		out_zo->pminu = pminu;
		out_zo->actmin = actmin;		 // Running minimum estimate
		out_zo->actminsub = actminsub;// sub-window minimum estimate
		out_zo->subwc = subwc;		 // force a buffer switch on first loop
		out_zo->actbuf = actbuf;		 // buffer to store subwindow minima
		out_zo->ibuf = ibuf;
		out_zo->lminflag = lminflag;	 // flag to remember local minimum
		out_zo->tinc = tinc;			 // must be the last one
		out_zo->qq = qq;
	}
}


void estnoisem( Matrix yf, double tz, alg_param *pp, Matrix *out_x, estnoisem_out_stat *out_zo, Matrix *out_xs )
{
	size_t nr, nrf;
	alg_param qq;
	init_alg_param( qq );
	double nrcum, subwc, ibuf, tinc;
	Matrix actmin, actminsub, actbuf, lminflag, sn2, p, pb, pb2, pminu, ac;
	double taca, tamax, taminh, tpfall, tbmax, qeqmin, qeqmax, av, td, nu;
	Matrix qith, nsmdb;


	yf.size( &nrf, &nr);		 // number of frames and freq bins
	if( out_x != NULL )
		*out_x = zeros(nr,nrf);			 // initialize output arrays
	if( out_xs != NULL )
		*out_xs = zeros(nr,nrf);			 // will hold std error in the future

	tinc  =  tz;			 // second argument is frame increment
	nrcum = 0;				 // no frames so far
		// default algorithm constants

	qq.taca = 0.0449;		 // smoothing time constant for alpha_c  =  -tinc/log(0.7) in equ (11)
	qq.tamax = 0.392;		 // max smoothing time constant in (3)  =  -tinc/log(0.96)
	qq.taminh = 0.0133;		 // min smoothing time constant (upper limit) in (3)  =  -tinc/log(0.3)
	qq.tpfall = 0.064;		 // time constant for P to fall (12)
	qq.tbmax = 0.0717;		 // max smoothing time constant in (20)  =  -tinc/log(0.8)
	qq.qeqmin = 2;			 // minimum value of Qeq (23)
	qq.qeqmax = 14;			 // max value of Qeq per frame
	qq.av = 2.12;			 // fudge factor for bc calculation (23 + 13 lines)
	qq.td = 1.536;			 // time to take minimum over
	qq.nu = 8;				 // number of subwindows
								 // noise slope thresholds in dB/s
	double r[] = {0.03,0.05,0.06,INFINITY}; 
	double r2[] = {47,31.4,15.7,4.1}; 
	qq.qith = Matrix(r,4,1) ;
	qq.nsmdb = Matrix(r2,4,1);;

	if( pp != NULL )
	{
		if( !isnan( pp->taca ) )
			qq.taca = pp->taca;
		if( !isnan( pp->tamax ) )
			qq.tamax = pp->tamax;
		if( !isnan( pp->taminh ) )
			qq.taminh = pp->taminh;
		if( !isnan( pp->tpfall ) )
			qq.tpfall = pp->tpfall;
		if( !isnan( pp->tbmax ) )
			qq.tbmax = pp->tbmax;
		if( !isnan( pp->qeqmin ) )
			qq.qeqmin = pp->qeqmin;
		if( !isnan( pp->qeqmax ) )
			qq.qeqmax = pp->qeqmax;
		if( !isnan( pp->av ) )
			qq.av = pp->av;
		if( !isnan( pp->td ) )
			qq.td = pp->td;
		if( !isnan( pp->nu ) )
			qq.nu = pp->nu;
		if( !pp->qith.isempty() )
			qq.qith = pp->qith;
		if( !pp->nsmdb.isempty() )
			qq.nsmdb = pp->nsmdb;
	}

	// unpack parameter structure

	taca = qq.taca;				 // smoothing time constant for alpha_c  =  -tinc/log(0.7) in equ (11)
	tamax = qq.tamax;			 // max smoothing time constant in (3)  =  -tinc/log(0.96)
	taminh = qq.taminh;			 // min smoothing time constant (upper limit) in (3)  =  -tinc/log(0.3)
	tpfall = qq.tpfall;			 // time constant for P to fall (12)
	tbmax = qq.tbmax;			 // max smoothing time constant in (20)  =  -tinc/log(0.8)
	qeqmin = qq.qeqmin;			 // minimum value of Qeq (23)
	qeqmax = qq.qeqmax;			 // max value of Qeq per frame
	av = qq.av;					 // fudge factor for bc calculation (23 + 13 lines)
	td = qq.td;					 // time to take minimum over
	nu = qq.nu;					 // number of subwindows
	qith = qq.qith;				 // noise slope thresholds in dB/s
	nsmdb = qq.nsmdb;			 // maximum permitted +ve noise slope in dB/s

	// derived algorithm constants

	double aca = exp( -tinc / taca );		 // smoothing constant for alpha_c in equ (11)  =  0.7
	double acmax = aca;				 // min value of alpha_c  =  0.7 in equ (11) also  =  0.7
	double amax = exp( -tinc / tamax );	 // max smoothing constant in (3)  =  0.96
	double aminh = exp( -tinc / taminh );	 // min smoothing constant (upper limit) in (3)  =  0.3
	double bmax = exp( -tinc / tbmax );	 // max smoothing constant in (20)  =  0.8
	double snrexp  =  -tinc / tpfall;
	double nv = round( td / ( tinc * nu ) );	 // length of each subwindow in frames
	double nd, md, hd, mv, hv;

	if( nv < 4 )				 // algorithm doesn't work for miniscule frames
	{
		nv = 4;
		nu = max( round( td / ( tinc * nv ) ), 1 );
	}
	nd = nu * nv;					 // length of total window in frames
	mhvals(nd, &md, &hd );		 // calculate the constants M(D) and H(D) from Table III
	mhvals(nv, &mv, &hv );		 // calculate the constants M(D) and H(D) from Table III
								 // [8 4 2 1.2] in paper
	Matrix nsms = power( 10, ( nsmdb * nv * tinc / 10 ) );
	double qeqimax = 1 / qeqmin;			 // maximum value of Qeq inverse (23)
	double qeqimin = 1 / qeqmax;			 // minumum value of Qeq per frame inverse

	if( yf.isempty() )			 // provide dummy initialization
	{
		ac = 1;					 // correction factor (9)
		subwc = nv;				 // force a buffer switch on first loop
		ibuf = 0;
		p = *out_x;					 // smoothed power spectrum
		sn2 = p;				 // estimated noise power
		pb = p;					 // smoothed noisy speech power (20)
		pb2 = power( pb, 2 );
		pminu = p;
								 // Running minimum estimate
		actmin = real( ones( 1, nrf ) * INFINITY ); //repmat(Inf,1,nrf);
		actminsub = actmin;		 // sub-window minimum estimate
								 // buffer to store subwindow minima
		actbuf = real( ones( nu, nrf ) * INFINITY ); //repmat(Inf,nu,nrf);
		lminflag = zeros(1,nrf); // flag to remember local minimum
	}
	else
	{
		if( !nrcum )			 // initialize values for first frame
		{
			p = yf.getSubMatrix( 0, nrf - 1, 0, 0 ); // yf(1,:);		 // smoothed power spectrum
			ac = 1;				 // correction factor (9)
			sn2 = p;			 // estimated noise power
			pb = p;				 // smoothed noisy speech power (20)
			pb2 = power( pb, 2);
			pminu = p;
								 // Running minimum estimate
			actmin = real( ones( 1, nrf ) * INFINITY ); //repmat(Inf,1,nrf);
			actminsub = actmin;	 // sub-window minimum estimate
			subwc = nv;			 // force a buffer switch on first loop
								 // buffer to store subwindow minima
			actbuf = real( ones( nu, nrf ) * INFINITY ); //repmat(Inf,nu,nrf);
			ibuf = 0;
								 // flag to remember local minimum
			lminflag = zeros(1,nrf);
		}

		// loop for each frame

		for( size_t t = 0; t < nr; t++ )			 // we use t instead of lambda in the paper
		{
			Matrix yft = yf.getSubMatrix( 0, nrf - 1, t, t ); // yf(t,:);		 // noise speech power spectrum
								 // alpha_c-bar(t)  (9)
			Matrix acb = power( power ( ( dotDivision( sum(p),sum(yft) ) - 1 ), 2) + 1, -1);
acb = real(acb);								 // alpha_c(t)  (10)
			ac = max( acb, acmax ) * ( 1 - aca ) + ac * aca;
								 // alpha_hat: smoothing factor per frequency (11)

            //ah=amax*ac.*(1+(p./sn2-1).^2).^(-1)    % alpha_hat: smoothing factor per frequency (11)
			Matrix ah = dotProduct( power(  power( dotDivision(p,sn2)-1,2 ) + 1 , (-1) ), ac ) * amax;    // alpha_hat: smoothing factor per frequency (11)
			//ah = power( amax * dotProduct( ac, 1 + power( ( dotDivision( p, sn2 ) - 1 ), 2 ), -1);
ah = real(ah);
			Matrix snr = sum(p) / sum(sn2);
								 // lower limit for alpha_hat (12)
			ah = max( min( mpower( snr, snrexp ), aminh ), ah );

								 // smoothed noisy speech power (3)
			p = dotProduct( ah, p ) + dotProduct( ah*(-1) + 1, yft );
			Matrix b = min( power( ah, 2 ), bmax ); // smoothing constant for estimating periodogram variance (22 + 2 lines)
								 // smoothed periodogram (20)
			pb = dotProduct( b, pb ) + dotProduct( b * (-1) + 1, p );
								 // smoothed periodogram squared (21)
			pb2 = dotProduct( b, pb2 ) + dotProduct( b * (-1) + 1, power( p, 2 ) );

								 // Qeq inverse (23)
			Matrix qeqi = max( min( dotDivision( pb2 - power( pb, 2), power( sn2, 2 ) * 2 ), qeqimax ) , qeqimin / ( t + 1 + nrcum ) );
			Matrix qiav = sum(qeqi)/nrf; // Average over all frequencies (23+12 lines) (ignore non-duplication of DC and nyquist terms)
			Matrix bc = sqrt( qiav ) * av + 1; // bias correction factor (23+11 lines)
								 // we use the simplified form (17) instead of (15)
			Matrix bmind = dotDivision( 1 - md, power( qeqi, -1 ) - md * 2 ) * ( nd - 1 ) * 2 + 1;
								 // same expression but for sub windows
			Matrix bminv = dotDivision( 1 - mv, power( qeqi, -1 ) - mv * 2 ) * ( nv - 1 ) * 2 + 1;
								 // Frequency mask for new minimum
			Matrix kmod = ( bc * dotProduct( p, bmind) ) < actmin ;

			if( any( kmod ) )
			{
				size_t co, ro;
				kmod.size( &co, &ro );
				complex<double> temp;
				Matrix t1 =bc * dotProduct( p, bmind ); 
				Matrix t2 =bc * dotProduct( p, bminv ); 
				for( size_t i = 0; i < co; i++ )
					for( size_t j = 0; j < ro; j++ )
					{
						kmod.getElement( &temp, i, j );
						if( temp.real() == 0 )
							continue;
						t1.getElement( &temp, i, j );
						actmin.setElement( temp , i, j );
						t2.getElement( &temp, i, j );
						actminsub.setElement( temp , i, j );
					}
			}
			if( subwc>1 && subwc<nv )
			{
								 // potential local minimum frequency bins
				lminflag = lminflag | kmod;
				pminu = min(actminsub,pminu);
				sn2 = pminu;
			}
			else if( subwc >= nv )	 // end of buffer - do a buffer switch
			{
								 // increment actbuf storage pointer
				ibuf = 1 + rem( ibuf, nu );
								 // save sub-window minimum
				size_t col, row;
				actmin.size( &col, &row );
				complex<double> temp;
				for( size_t i = 0; i < col ; i++ )
				{
					actmin.getElement( &temp, i, 0 );
					actbuf.setElement( temp, i, ibuf - 1 );
				}
				//actbuf(ibuf,:) = actmin;

				pminu = min(actbuf);
				Matrix i = find(qiav<qith);
				//nsm = nsms(i(1));// noise slope max
				i.getElement( &temp, 0 );
				nsms.getElement( &temp, temp.real() );
				double nsm = temp.real();
				Matrix lmin = lminflag & ( !kmod ) & ( actminsub<pminu*nsm ) & ( actminsub>pminu );
				if( any(lmin) )
				{
					size_t co, ro;
					lmin.size( &co, &ro );
					complex<double> temp;
					for( size_t i = 0; i < co; i++ )
						for( size_t j = 0; j < ro; j++ )
						{
							kmod.getElement( &temp, i, j );
							if( temp.real() == 0 )
								continue;
							actminsub.getElement( &temp , i, j );
							pminu.setElement( temp , i, j );
							//actbuf(:,lmin) = repmat(pminu(lmin),nu,1);
							pminu.getElement( &temp, i, 0 );
							for( size_t k = 0; k < nu; k++ )
								actbuf.setElement( temp, i, k );
						}
				}
				lminflag.size( &col, &row );
				lminflag = zeros( row, col );
				actmin.size( &col, &row );
				actmin = real( ones( row, col ) * INFINITY );
				subwc = 0;
			}
			subwc = subwc+1;
			//x(t,:) = sn2;
			size_t col, row, k = 0;
			complex<double> temp;
			sn2.size( &col, &row );
			if( out_x != NULL )
				for( size_t i = 0; i < col; i++ )
				{
					sn2.getElement( &temp, i, 0 );
					out_x->setElement( temp, k++, t );
				}
			Matrix qisq = sqrt(qeqi);
			// empirical formula for standard error based on Fig 15 of [2]
			if( out_xs != NULL )
			{
				Matrix txs = dotProduct( sn2, sqrt( dotDivision( dotProduct(qisq * 100 + nd, qisq ) / (1+0.005*nd+6/nd), power( qeqi, -1 ) * 0.5 + nd - 1 ) * 0.266 ) );
				complex<double> temp;
				for( size_t i = 0; i < nrf; i++ )
				{
					txs.getElement( &temp, i );
					out_xs->setElement( temp, i, t );
				}
			}
		}
	}
	if( out_zo != NULL )				 // we need to store the state for next time
	{
		init_estnoisem_out_stat( *out_zo );
		out_zo->nrcum = nrcum+nr;	 // number of frames so far
		out_zo->p = p;				 // smoothed power spectrum
		out_zo->ac = ac;				 // correction factor (9)
		out_zo->sn2 = sn2;			 // estimated noise power
		out_zo->pb = pb;				 // smoothed noisy speech power (20)
		out_zo->pb2 = pb2;
		out_zo->pminu = pminu;
		out_zo->actmin = actmin;		 // Running minimum estimate
		out_zo->actminsub = actminsub;// sub-window minimum estimate
		out_zo->subwc = subwc;		 // force a buffer switch on first loop
		out_zo->actbuf = actbuf;		 // buffer to store subwindow minima
		out_zo->ibuf = ibuf;
		out_zo->lminflag = lminflag;	 // flag to remember local minimum
		out_zo->tinc = tinc;			 // must be the last one
		out_zo->qq = qq;
	}
}

void specsub( Matrix si, /* specsub_out_stat * */ double fsz, alg_param *pp, Matrix *out_ss, specsub_out_stat *out_zo )
{
	double fs, ni;
	size_t col, row;
	Matrix ypf, mzf, af, dpf, v, bf;
	alg_param qq, *qp = pp;
	init_alg_param( qq );
	//init_alg_param( qp );
	complex<double> temp, temp2;

// fsz struct
/*
	fs = fsz.fs;
	qq = fsz.qq;
	qp = *fsz.qp;
	ze = fsz.ze;
							 // allocate space for speech
	si.size( &col, &row );
	s = zeros( length( fsz.si ) + col * row, 1 );
	//s(1:length(fsz.si))=fsz.si;
	for( size_t i = 0; i < length( fsz.si ); i++ )
	{
		fsz.si.getElement( &temp, i );
		s.setElement( temp, i );
	}
	//s(length(fsz.si)+1:end)=si(:);
	for( size_t i = length( fsz.si ); i < length( fsz.si ) + col * row; i++ )
	{
		si.getElement( &temp, i - length( fsz.si ) );
		s.setElement( temp, i );
*/
// end of struct

// fsz double

	fs=fsz;					 // sample frequency
	si.size( &col, &row );
	Matrix s = si.reshape( col * row, 1 );
	// default algorithm constants
	qq.of=2;				 // overlap factor = (fft length)/(frame increment)
	qq.ti=16e-3;			 // desired frame increment (16 ms)
	qq.ri=0;				 // round ni to the nearest power of 2
	qq.g=1;					 // subtraction domain: 1=magnitude, 2=power
	qq.e=1;					 // gain exponent
	qq.am=3;				 // max oversubtraction factor
	qq.b=0.01;				 // noise floor
	qq.al=-5;				 // SNR for maximum a (set to Inf for fixed a)
	qq.ah=20;				 // SNR for minimum a
	qq.bt=-1;				 // suppress binary masking
	qq.mx=0;				 // no input mixing
	qq.gh=1;				 // maximum gain
	if( pp != NULL )
	{
		if( !isnan( pp->taca ) )
			qq.taca = pp->taca;
		if( !isnan( pp->tamax ) )
			qq.tamax = pp->tamax;
		if( !isnan( pp->taminh ) )
			qq.taminh = pp->taminh;
		if( !isnan( pp->tpfall ) )
			qq.tpfall = pp->tpfall;
		if( !isnan( pp->tbmax ) )
			qq.tbmax = pp->tbmax;
		if( !isnan( pp->qeqmin ) )
			qq.qeqmin = pp->qeqmin;
		if( !isnan( pp->qeqmax ) )
			qq.qeqmax = pp->qeqmax;
		if( !isnan( pp->av ) )
			qq.av = pp->av;
		if( !isnan( pp->td ) )
			qq.td = pp->td;
		if( !isnan( pp->nu ) )
			qq.nu = pp->nu;
		if( !pp->qith.isempty() )
			qq.qith = pp->qith;
		if( !pp->nsmdb.isempty() )
			qq.nsmdb = pp->nsmdb;
	}

// end of double

	// derived algorithm constants
	if( qq.ri )
	{
		ni = pow( 2, ceil( log2 ( ( qq.ti * fs * sqrt(0.5) ) ) ) );
	}
	else
	{
						 // frame increment in samples
		ni = round( qq.ti * fs );
	}
	double tinc = ni / fs;			 // true frame increment time

	// calculate power spectrum in frames
	double no=round(qq.of);	 // integer overlap factor
	double nf=ni*no;			 // fft length
	Matrix w=sqrt(hamming(nf+1)).transpose();
	w.size( &col, &row );
	w.delColumn( col - 1 );
	//w(end)=[]; // for now always use sqrt hamming window
						 // normalize to give overall gain of 1
	//w=w/sqrt(sum(w( 1:ni:nf ).^2));
	temp = complex<double>( 0, 0 );
	for( size_t i = 0; i < nf; i += ni )
	{
		w.getElement( &temp, i );
		temp2 += pow( temp, 2 );
	}
	w = w / sqrt( temp2 );

	Matrix y=enframe(s,w,ni);
	Matrix yf=rfft(y,nf,2);
	Matrix yp=dotProduct( yf, conj( yf ) );	 // power spectrum of input speech
	size_t nr, nf2;
	yp.size( &nf2, &nr );	 // number of frames

//fsz struct
/*
						 // estimate the noise using minimum statistics
		estnoisem( yp, ze, NULL, &dp, &ze, NULL );
		ssv = fsz.ssv;
*/
//end of struct

//fsz double						 // estimate the noise using minimum statistics
	Matrix dp;
	estnoisem_out_stat ze;
	init_estnoisem_out_stat( ze );
	estnoisem( yp, tinc, qp, &dp, &ze, NULL );
						 // dummy saved overlap
	Matrix ssv=zeros(ni*(no-1),1);
//end of double
	
	if( !nr )			 // no data frames
	{
		Matrix emp;
		*out_ss = emp;
	}
	else
	{
		Matrix mz = ( yp == 0 );		 //  mask for zero power time-frequency bins (unlikely)
		if( qq.al < INFINITY )
		{
			ypf=sum(yp,2);
			dpf=sum(dp,2);
			mzf=( dpf==0 );	 // zero noise frames = very high SNR

			af = ( ( min( max( log10( dotDivision( ypf,dpf+mzf) ) * 10, qq.al ), qq.ah ) - qq.ah ) * (qq.am-1) )/( qq.al - qq.ah ) + 1;
			//af( mzf ) = 1;	 // fix the zero noise frames
			af.size( &col, &row );
			for( size_t i = 0; i < row; i++ )
			{
				mzf.getElement( &temp, 0, i );
				if( temp.real() == 1 )
					af.setElement( complex<double>( 1, 0 ), 0, i );
			}
		}
		else
		{
			Matrix tempmat;
			tempmat = qq.am;
			af=repmat(tempmat,nr,1);
		}
		if( qq.g == 1)		 // magnitude domain subtraction
		{
			v = sqrt( dotDivision( dp, yp + mz ) );
			af = sqrt( af );
			bf = sqrt( qq.b );
		}
		else if( qq.g == 2)		 // power domain subtraction
		{
			v = dotDivision( dp, yp + mz );
			bf = qq.b;
		}
		else	 // arbitrary subtraction domain
		{
			v = power( dotDivision( dp, yp + mz ), qq.g * 0.5);
			af = power( af, qq.g * 0.5 );
			bf = pow( qq.b,  qq.g * 0.5 );
		}
						 // replicate frame oversubtraction factors for each frequency
		af = repmat( af, 1, nf2 );
						 // mask for noise floor limiting
		Matrix mf = v >= power( af + bf, -1);
		v.size( &col, &row);
		Matrix g = zeros( row, col );
		double eg = qq.e / qq.g;	 // gain exponent relative to subtraction domain
		double gh = qq.gh;

		mf.size( &col, &row );
		if( eg == 1 )		 // Normal case
		{
					 // never give a gain > 1
			for( size_t i = 0; i < col; i++)
				for( size_t j = 0; j < row; j++ )
				{
					mf.getElement( &temp, i, j );
					if( temp.real() )
					{
						//g(mf)=min(bf*v(mf),gh);
						v.getElement( &temp2, i, j );
						Matrix tem = min( bf * temp2, gh );
						tem.getElement( &temp, 0 );
						g.setElement( temp, i, j );
					}
					else
					{
						//g(~mf)=1-af(~mf).*v(~mf);
						v.getElement( &temp2, i, j );
						af.getElement( &temp, i, j );
						temp = temp * temp2 * (double)(-1) + (double)1;
						g.setElement( temp, i, j );

					}
				}
		}
		else if( eg == 0.5 )
		{
			for( size_t i = 0; i < col; i++)
				for( size_t j = 0; j < row; j++ )
				{
					mf.getElement( &temp, i, j );
					if( temp.real() )
					{
						//g(mf)=min(sqrt(bf*v(mf)),gh);
						v.getElement( &temp2, i, j );
						Matrix tem = min( sqrt( bf * temp2 ), gh );
						tem.getElement( &temp, 0 );
						g.setElement( temp, i, j );
					}
					else
					{
						//g(~mf)=sqrt(1-af(~mf).*v(~mf));
						v.getElement( &temp2, i, j );
						af.getElement( &temp, i, j );
						temp = sqrt( temp * temp2 * (double)(-1) + (double)1 );
						g.setElement( temp, i, j );

					}
				}
		}
		else
		{
			for( size_t i = 0; i < col; i++)
				for( size_t j = 0; j < row; j++ )
				{
					mf.getElement( &temp, i, j );
					if( temp.real() )
					{
						//g(mf)=min((bf*v(mf)).^eg,gh);
						v.getElement( &temp2, i, j );
						Matrix tem = min( power( bf * temp2, eg ), gh );
						tem.getElement( &temp, 0 );
						g.setElement( temp, i, j );
					}
					else
					{
						//g(~mf)=(1-af(~mf).*v(~mf)).^eg;
						v.getElement( &temp2, i, j );
						af.getElement( &temp, i, j );
						temp = pow( temp * temp2 * (double)(-1) + (double)1, eg );
						g.setElement( temp, i, j );

					}
				}
		}
		if( qq.bt >= 0 )
		{
			g = g > qq.bt;
		}
						 // mix in some of the input
		g = g * ( 1 - qq.mx ) + qq.mx;
						 // inverse dft and apply output window
		Matrix se = dotProduct ( irfft( dotProduct(yf,g).transpose(), nf ).transpose(), repmat(w,nr,1) );
						 // space for overlapped output speech
		*out_ss = zeros( ni * ( nr + no - 1 ), no );
		//out_ss(1:ni*(no-1),end)=ssv;
		for( size_t i = 0; i < ni * ( no - 1); i++ )
		{
			ssv.getElement( &temp, i );
			out_ss->setElement( temp, no - 1, i );
		}
		for( size_t i = 0; i < no; i++ )
		{
						 // number of samples in this set
			double nm = nf * ( 1 + floor( ( (double)nr - (i+1) ) / no ) );
			//ss( 1+(i-1)*ni : nm+(i-1)*ni , i ) = reshape( se( i : no : nr , : )', nm, 1 )
			Matrix tempmat;
			se.size( &col, &row );
			for( size_t j = i; j < nr; j += no )
				for( size_t k = 0; k < col; k++ )
				{
					se.getElement( &temp, k, j );
					tempmat.addRow( &temp, 1 );
				}
			size_t k = 0;
			for( size_t j = i * ni; j < nm + i * ni; j++ )
			{
				tempmat.getElement( &temp, k++ );
				out_ss->setElement( temp, i, j );
			}
		}
		*out_ss = sum( *out_ss, 2 );
	}
	if( out_zo != NULL )
	{
		init_specsub_out_stat( *out_zo );
		if( nr )
		{
						 // save the output tail for next time

			out_ss->size( &col, &row );
			*out_ss = reshape( *out_ss, 1, col * row );
			//out_zo->ssv=out_ss(end-ni*(no-1)+1:end);
			//out_ss( end - ni * (no-1) + 1 : end )=[];
			for( size_t i = col * row - ni * ( no - 1 ); i < col * row; i++ )
			{
				out_ss->getElement( &temp, col * row - ni * ( no - 1 ) );
				out_zo->ssv.addColumn( &temp, 1 );
				out_ss->delColumn( col * row - ni * ( no - 1 ) );
			}
		}
		else
		{
			out_zo->ssv=ssv;	 //
		}
						 // save the tail end of the input speech signal
		//out_zo->si = s( length( *out_ss ) + 1 : end );
		s.size( &col, &row );
		Matrix emptymat;
		out_zo->si = emptymat;
		for( size_t i = length( *out_ss ); i < col * row; i++ )
		{
			s.getElement( &temp, i );
			out_zo->si.addRow( &temp, 1 );
		}
		out_zo->fs = fs;	 // save sample frequency
		out_zo->qq = qq;	 // save loval parameters
		if( qp != NULL )
			out_zo->qp = *qp;	 // save estnoisem parameters
		out_zo->ze = ze;	 // save state of noise estimation
	}
}

Matrix rfft( Matrix x )
{
	size_t col, row, d, n;
	x.size( &col, &row );

	if( length( x ) == 1 )
		return x;
	
	if( row > 1 )
	{
		d = 1;
		n = row;
	}
	else
	{
		d = 2;
		n = col;
	}

	Matrix y = fft( x, n, d );
	if( y.isempty() )
		return y;

	y = y.getSubMatrix( 0, d == 1? fix(n/2):col, 0, d==2? fix(n/2) : row );

	return y;
}

Matrix rfft( Matrix x, size_t n )
{
	size_t col, row;
	x.size( &col, &row );

	if( length( x ) == 1 )
		return x;
	
	size_t d = row > 1 ? 1 : 2;

	Matrix y = fft( x, n, d );
	if( y.isempty() )
		return y;

	y = y.getSubMatrix( 0, d == 1? 1 + fix(n/2):col, 0, d==2? 1+fix(n/2) : row );

	return y;
}

Matrix rfft( Matrix x, size_t n, size_t d)
{
	size_t col, row;
	x.size( &col, &row );

	if( length( x ) == 1 )
		return x;

	Matrix y = fft( x, n, d );
	if( y.isempty() )
		return y;

	y = y.getSubMatrix( 0, d == 2 ? fix(n/2) : col - 1, 0, d == 1 ? fix(n/2) : row - 1 );

	return y;
}

Matrix irfft( Matrix y )
{
	size_t col, row, d;

	y.size( &col ,&row );
	size_t ps = col * row;

	if( length( y ) == 1 )
		return y;

	if( row > 1 )
		d = 1;
	else
		d = 2;

	size_t m = d == 1 ? row : col;
	size_t k = ps / m;

	Matrix v, x;
	if( d == 1 )
		v = y;
	else
		v = y.transpose();

	size_t n = 2 * m - 2;        // default output length

	if( n % 2 )		// odd output length
	{
		Matrix temp = v, temp2 = conj( v );
		complex <double> dat[ col ];
		size_t l, ll;
		v.size( &l, &ll );
		for( size_t i = m; i >= 2; i-- )
		{
			for( size_t j = 0; j < l; j++ )
				temp2.getElement( dat + j, j, i - 1  );
			temp.addRow( dat, l );
		}
		x = real( ifft( temp ) );    // do it the long way
	}
	else			// even output length
	{
		complex <double> dat[ k ], dat2[ k ];
		for( size_t i = 0; i < col; i++ )
		{
			v.getElement( dat, i, m - 1 );
			v.setElement( real( *dat ), i, m - 1 );
		}
		Matrix t = exp( seq( 0, m - 1 ) * complex<double> ( 0, 2 * M_PI / n ) ).transpose() * complex <double>( 0, -0.5 );
		t = repmat( t, 1, k );
		Matrix z = dotProduct( ( t + 0.5 ), ( conj( flipud( v ) ) - v ) ) + v;
		z.delRow( m - 1 );
		z = ifft( z );
		Matrix zr = real( z );
		Matrix zi = imag( z );
		for( size_t i = 0; i < n / 2; i++ )
		{
			for( size_t j = 0; j < k; j++ )
			{
				zr.getElement( dat + j, j, i );
				zi.getElement( dat2 + j, j, i );
			}
			x.addRow( dat, k );
			x.addRow( dat2, k );
		}
	}

	if( d == 2 )
		x = x.transpose();

	return x;
}


Matrix irfft( Matrix y, size_t n )
{

	size_t col, row, d;

	y.size( &col ,&row );
	size_t ps = col * row;

	if( length( y ) == 1 )
		return y;

	if( row > 1 )
		d = 1;
	else
		d = 2;

	size_t m = d == 1 ? row : col;
	size_t k = ps / m;

	Matrix v, x;
	if( d == 1 )
		v = y;
	else
		v = y.transpose();

	size_t mm = 1 + fix( n / 2 );          // expected input length
	if( mm > m )
	{
		double zer[ col ];
		bzero( zer, col * sizeof( double ) );
		for( size_t i = m; i <= mm; i++ )
			v.addRow( zer, col );
	}
	else if( mm < m )
		v = v.getSubMatrix( 0, col - 1, 0, mm - 1 );
	m = mm;

	if( n % 2 )		// odd output length
	{
		Matrix temp = v, temp2 = conj( v );
		complex <double> dat[ col ];
		size_t l, ll;
		v.size( &l, &ll );
		for( size_t i = m; i >= 2; i-- )
		{
			for( size_t j = 0; j < l; j++ )
				temp2.getElement( dat + j, j, i - 1  );
			temp.addRow( dat, l );
		}
		x = real( ifft( temp ) );    // do it the long way
	}
	else			// even output length
	{
		complex <double> dat[ k ], dat2[ k ];
		for( size_t i = 0; i < col; i++ )
		{
			v.getElement( dat, i, m - 1 );
			v.setElement( real( *dat ), i, m - 1 );
		}
		Matrix t = exp( seq( 0, m - 1 ) * complex<double> ( 0, 2 * M_PI / n ) ).transpose() * complex <double>( 0, -0.5 );
		t = repmat( t, 1, k );
		Matrix z = dotProduct( ( t + 0.5 ), ( conj( flipud( v ) ) - v ) ) + v;
		z.delRow( m - 1 );
		z = ifft( z );
		Matrix zr = real( z );
		Matrix zi = imag( z );
		for( size_t i = 0; i < n / 2; i++ )
		{
			for( size_t j = 0; j < k; j++ )
			{
				zr.getElement( dat + j, j, i );
				zi.getElement( dat2 + j, j, i );
			}
			x.addRow( dat, k );
			x.addRow( dat2, k );
		}
	}

	if( d == 2 )
		x = x.transpose();

	return x;
}


Matrix irfft( Matrix y, size_t n, size_t d )
{

	size_t col, row;

	y.size( &col ,&row );
	size_t ps = col * row;

	if( length( y ) == 1 )
		return y;

	size_t m = d == 1 ? row : col;
	size_t k = ps / m;

	Matrix v, x;
	if( d == 1 )
		v = y;
	else
		v = y.transpose();

	size_t mm = 1 + fix( n / 2 );          // expected input length
	if( mm > m )
	{
		double zer[ col ];
		bzero( zer, col * sizeof( double ) );
		for( size_t i = m; i <= mm; i++ )
			v.addRow( zer, col );
	}
	else if( mm < m )
		v = v.getSubMatrix( 0, col - 1, 0, mm - 1 );
	m = mm;

	if( n % 2 )		// odd output length
	{
		Matrix temp = v, temp2 = conj( v );
		complex <double> dat[ col ];
		size_t l, ll;
		v.size( &l, &ll );
		for( size_t i = m; i >= 2; i-- )
		{
			for( size_t j = 0; j < l; j++ )
				temp2.getElement( dat + j, j, i - 1  );
			temp.addRow( dat, l );
		}
		x = real( ifft( temp ) );    // do it the long way
	}
	else			// even output length
	{
		complex <double> dat[ k ], dat2[ k ];
		for( size_t i = 0; i < col; i++ )
		{
			v.getElement( dat, i, m - 1 );
			v.setElement( real( *dat ), i, m - 1 );
		}
		Matrix t = exp( seq( 0, m - 1 ) * complex<double> ( 0, 2 * M_PI / n ) ).transpose() * complex <double>( 0, -0.5 );
		t = repmat( t, 1, k );
		Matrix z = dotProduct( ( t + 0.5 ), ( conj( flipud( v ) ) - v ) ) + v;
		z.delRow( m - 1 );
		z = ifft( z );
		Matrix zr = real( z );
		Matrix zi = imag( z );
		for( size_t i = 0; i < n / 2; i++ )
		{
			for( size_t j = 0; j < k; j++ )
			{
				zr.getElement( dat + j, j, i );
				zi.getElement( dat2 + j, j, i );
			}
			x.addRow( dat, k );
			x.addRow( dat2, k );
		}
	}

	if( d == 2 )
		x = x.transpose();

	return x;
}

Matrix enframe( Matrix x, Matrix win, size_t inc )
{
	size_t col, row, len;

	x.size( &col, &row );
	size_t nx = col * row;

	size_t nwin = length(win);
	if( nwin == 1 )
	{
		complex<double> temp;
		win.getElement( &temp, 0, 0 );
		len = (size_t) temp.real();
	}
	else
		len = nwin;

	size_t nf = fix(((float)nx-len+inc)/inc);
	complex<double> dat[ len ];
	Matrix f;
	x = x.reshape( col * row, 1 );

	for( size_t i = 0; i < nf; i++ )
	{
		for( size_t j = 0; j < len; j++ )
			x.getElement( dat + j, 0, i * inc + j );

		f.addRow( dat, len );
	}

	if( nwin > 1 )
		f = dotProduct( f, repmat( reshape( win, 1, len ), nf, 1 ) );
		//f = dotProduct( f, repmat( reshape( win, len, 1 ), nf, 1 ) );

	return f;
}

Matrix enframe( Matrix x, Matrix win )
{
	size_t col, row, len;

	x.size( &col, &row );
	size_t nx = col * row;

	size_t nwin = length(win);
	if( nwin == 1 )
	{
		complex<double> temp;
		win.getElement( &temp, 0, 0 );
		len = (size_t) temp.real();
	}
	else
		len = nwin;

	size_t inc = len;
	size_t nf = fix(((float)nx-len+inc)/inc);
	complex<double> dat[ len ];
	Matrix f;
	x = x.reshape( col * row, 1 );

	for( size_t i = 0; i < nf; i++ )
	{
		for( size_t j = 0; j < len; j++ )
			x.getElement( dat + j, 0, i * inc + j );

		f.addRow( dat, len );
	}

	if( nwin > 1 )
		f = dotProduct( f, repmat( reshape( win, 1, len ), nf, 1 ) );
		//f = dotProduct( f, repmat( reshape( win, len, 1 ), nf, 1 ) );

	return f;
}

