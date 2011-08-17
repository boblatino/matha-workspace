#include "voicebox.h"

void estnoisem()
{

}


void specsub()
{

}


void rfft()
{

}


void irfft()
{

}


Matrix enframe( Matrix x, Matrix win )
{
	size_t col_x, row_x, n_win, len;

	x.size( &col, &row );
	nx = col * row;

	nwin = length(win);
	if( nwin == 1 )
	{
		complex<double> temp;
		win.getElement( &temp, 0, 0 );
		len = (size_t) temp.real();
	}
	else
		len = nwin;

	inc = len;	//Remove in the other.

	nf = fix( ( nx - len + inc ) / inc );
	f = zeros( nf, len );
	indf = inc * ( 0 : (nf - 1) ) .';
	inds = ( 1 : len );
	f( : ) = x( indf( :, ones( 1, len ) ) + inds( ones( nf, 1 ) , : ) );
	if( nwin > 1 )
	{
		w = win( : )';
		f = f .* w( ones( nf, 1 ) , : );
	}
	if( nargout > 1 )
		t=(1+len)/2+indf;


//return f;
}


