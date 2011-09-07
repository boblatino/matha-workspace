#include <iostream>
#include "sndconv.h"
#include "voicebox.h"
#include <cstdlib>

#define BUFFSIZE 12565 * 8

using namespace std;

int main()
{
	double *buffer;
	Matrix out;
	sndconv sndconv1( BUFFSIZE );
	sndconv1.readwav( (char *) "noisy.wav" );
	sndconv1.makewav( (char *) "out.wav" );
	sndconv1.makeogg( (char *) "out.ogg" );

	while( ( buffer = (double *) sndconv1.popstream() ) )
	{
		Matrix bufmat( buffer, BUFFSIZE * sizeof(char) / sizeof(double), 1 );
		specsub( bufmat, 11025, NULL, &out, NULL );
		size_t siz = size( out, 1 );
		for( size_t i = 0; i < siz; i++ )
			out.getElement( buffer + i, 0, i );
		sndconv1.pushstream( (void *) buffer, siz * sizeof( double ) / sizeof( char ) );
	}

	return 0;
}
