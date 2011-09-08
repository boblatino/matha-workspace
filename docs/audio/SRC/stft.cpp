#include "stft.h"

using namespace std;

Matrix stft( Matrix x, double f, Matrix w, double h, double sr)
{
	Matrix win;
	double we;
	complex <double> temp;
	//function D = stft(x, f, w, h, sr)
	// D = stft(X, F, W, H, SR)                       Short-time Fourier transform.
	//	Returns some frames of short-term Fourier transform of x.  Each
	//	column of the result is one F-point fft (default 256); each
	//	successive frame is offset by H points (W/2) until X is exhausted.
	//       Data is hann-windowed at W pts (F), or rectangular if W=0, or
	//       with W if it is a vector.
	//       Without output arguments, will plot like sgram (SR will get
	//       axes right, defaults to 8000).
	//	See also 'istft.m'.
	// dpwe 1994may05.  Uses built-in 'fft'
	// $Header: /home/empire6/dpwe/public_html/resources/matlab/pvoc/RCS/stft.m,v 1.4 2010/08/13 16:03:14 dpwe Exp $


	// expect x as a row
	if( size(x,1) > 1 )
	{
		x = x.transpose();
	}

	size_t s = length(x);

	if( length(w) == 1 )
	{
		w.getElement( &temp, 0, 0 );
		we = temp.real();
		if( we == 0 )
		{
			// special case: rectangular window
			win = ones(1,f);
		}
		else
		{
			if( rem( we , 2) == 0 ) // force window to be odd-len
			{
				we++;
			}
			double halflen = ( we - 1 ) / 2;
			double halff = f/2;		 // midpoint of win
			Matrix halfwin = ( cos( seq( 0, halflen ) * M_PI / halflen ) + 1 ) * 0.5;
			win = zeros(1, f);
			double acthalflen = min( halff, halflen );
			for( size_t i = 0; i < acthalflen; i++ )
			{
				halfwin.getElement( &temp, i );
				//win( ( halff + 1 ):( halff + acthalflen ) ) = halfwin( 1 : acthalflen );
				win.setElement( temp, halff + i );
				//win( ( halff + 1 ):-1:( halff - acthalflen + 2 ) ) = halfwin( 1 : acthalflen );
				win.setElement( temp, halff - i );
			}
		}
	}
	else
	{
		win = w;
	}

	we = length(win);
	// now can set default hop
	if( h == 0 )
	{
		h = floor( we / 2 );
	}

	size_t c = 0;

	// pre-allocate output array
	Matrix d = zeros( ( 1 + f / 2 ), 1 + fix( ( s - f ) / h ) );

	for( size_t b = 0; b <= (s-f); b += h )
	{
		Matrix u = dotProduct( win, x.getSubMatrix( b, b + f - 1, 0, 0 ) );
		Matrix t = fft( u );
		for( size_t i = 0; i <= f / 2; i++ )
		{
			//d(:,c) = t( 1 : (1+f/2) )';
			t.getElement( &temp, i );
			d.setElement( temp, c, i );
		}
		c++;
	}

	return d;
}


Matrix stft( Matrix x, double f, Matrix w, double h)
{
	return stft( x, f, w, h, 8000 );
}

Matrix stft( Matrix x, double f, Matrix w )
{
	return stft( x, f, w, 0 );
}

Matrix stft( Matrix x, double f )
{
	Matrix w( &f, 1, 1 );
	return stft( x, f, w );
}

Matrix stft( Matrix x )
{
	return stft( x, 256 );
}

