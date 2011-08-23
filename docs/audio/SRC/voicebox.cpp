#include "voicebox.h"
#include <cmath>
#include <cstring>

#include <iostream>

using namespace std;

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

void estnoisem()
{

}


void specsub()
{

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

//	if nargin<2 || isempty(n)
//		n=2*m-2;        // default output length
//	else
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
//	end

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

	size_t nf = fix((nx-len+inc)/inc);
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
		f = dotProduct( f, repmat( reshape( win, len, 1 ), nf, 1 ) );

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
	size_t nf = fix((nx-len+inc)/inc);
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
		f = dotProduct( f, repmat( reshape( win, len, 1 ), nf, 1 ) );

	return f;
}

