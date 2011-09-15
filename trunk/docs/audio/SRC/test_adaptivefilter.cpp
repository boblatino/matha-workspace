#include "adaptivefilter.h"
#include <iostream>
#include "sndconv.h"
#include "voicebox.h"
#include <cstdlib>

#define BUFFSIZE 12565 * 8

using namespace std;


int main()
{
/*
	Matrix t = seq( 0, 0.05, 1.001 );

	Matrix input = sin( t );
	
	input.printout();

	Matrix out = adaptfilt( input, input, 5, &lms, 0.2 );

	out.printout();
*/

//*
	double *buffer;
	Matrix out;
	sndconv sndconv1( BUFFSIZE );
	sndconv1.readwav( (char *) "noisy.wav" );
	sndconv1.makewav( (char *) "out.wav" );

	while( ( buffer = (double *) sndconv1.popstream() ) )
	{
		Matrix bufmat( buffer, BUFFSIZE * sizeof(char) / sizeof(double) - 20, 1 );
		Matrix refmat( buffer + 19, BUFFSIZE * sizeof(char) / sizeof(double) - 20, 1 );
		//specsub( bufmat, 11025, NULL, &out, NULL );
		out = adaptfilt( bufmat, refmat, 20, &lms, 0.2 );
		size_t siz = size( out, 1 );
		for( size_t i = 0; i < siz; i++ )
			out.getElement( buffer + i, 0, i );
		sndconv1.pushstream( (void *) buffer, siz * sizeof( double ) / sizeof( char ) );
	}

//*/
	return 0;
}

