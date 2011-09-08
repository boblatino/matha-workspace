#include "istft.h"
#include <iostream>

using namespace std;

Matrix istft( Matrix d, double ftsize, Matrix w, double h )
{
	size_t cols, rows;
	Matrix win;
	complex <double> temp;
	double w1;

	d.size( &cols, &rows );
	if( rows != (ftsize/2)+1 )
	{
		//error('number of rows should be fftsize/2+1')
		cerr << "number of rows should be " << ftsize/2+1 << endl;
		Matrix ret;
		return ret;
	}

	if( length( w ) == 1 )
	{
		w.getElement( &temp, 0, 0 );
		w1 = temp.real();
		if( w1 == 0)
		{
			// special case: rectangular window
			win = ones( 1, ftsize );
		}
		else
		{
			if( rem( w1, 2 ) == 0 ) // force window to be odd-len
			{
				w1++;
			}
			double halflen = ( w1 - 1 ) / 2;
			double halff = ftsize / 2;
			Matrix halfwin = ( cos( seq( 0, halflen ) * M_PI / halflen ) + 1 ) * 0.5;
			win = zeros(1, ftsize);
			double acthalflen = min( halff, halflen );
			for( size_t i = 0; i < acthalflen; i++ )
			{
				halfwin.getElement( &temp, i );
				//win( ( halff + 1 ):( halff + acthalflen ) ) = halfwin( 1 : acthalflen );
				win.setElement( temp, halff + i );
				//win( ( halff + 1 ):-1:( halff - acthalflen + 2 ) ) = halfwin( 1 : acthalflen );
				win.setElement( temp, halff - i );
			}
			// 2009-01-06: Make stft-istft loop be identity for 25// hop
			win = win * 2 / 3;
		}
	}
	else
	{
		win = w;
	}

	w1 = length(win);
	// now can set default hop
	if( h == 0 )
	{
		h = floor( w1 / 2 );
	}

	size_t xlen = ftsize + ( cols - 1 ) * h;
	Matrix x = zeros( 1, xlen );

	for( size_t b = 0; b <= h * ( cols - 1 ); b += h )
	{
		//ft = d( :, 1+b/h ).transpose();
		Matrix ft = d.getSubMatrix( b/h, b/h, 0, rows - 1 ).transpose();
		//Matrix t = ft( [ ( ftsize / 2 ) : -1 : 2 ] );
		Matrix t;
		for( size_t i = ftsize / 2; i >= 2; i-- )
		{
			ft.getElement( &temp, i - 1 );
			t.addColumn( &temp, 1 );
		}
		ft = cat( 2, ft, conj( t ) );
		Matrix px = real( ifft( ft ) );
		//x( (b+1) : (b+ftsize) ) = x( (b+1) : (b+ftsize) ) + px .* win;
		complex <double> tpx, twin;
		for( size_t i = 0; i < ftsize; i++ )
		{
			x.getElement( &temp, b + i );
			px.getElement( &tpx, i );
			win.getElement( &twin, i );
			x.setElement( temp + tpx * twin, b + i );
		}	
	}

	return x;
}


Matrix istft( Matrix d, double ftsize, Matrix w )
{
	//if nargin < 4; h = 0; end  // will become winlen/2 later
	return istft( d, ftsize, w, 0 );
}

Matrix istft( Matrix d, double ftsize )
{
	//if nargin < 3; w = 0; end
	complex <double> temp( 0, 0 );
	Matrix w( &temp, 1, 1 );
	return istft( d, ftsize, w );
}

Matrix istft( Matrix d )
{
	//if nargin < 2; ftsize = 2*(size(d,1)-1); end
	return istft( d, ( size( d, 1 ) - 1 ) * 2 );
}
