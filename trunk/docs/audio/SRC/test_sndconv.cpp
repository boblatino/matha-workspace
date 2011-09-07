#include <iostream>
#include "sndconv.h"

using namespace std;

int main()
{
	void *buffer;
	sndconv sndconv1(  1024 );
	sndconv1.readwav( (char *) "noisy.wav" );
//	sndconv1.readmp3( (char *) "noisy.mp3" ); 
//	sndconv1.readmp3( (char *) "file.mp3" ); 
	sndconv1.makewav( (char *) "out.wav" );
	sndconv1.makeogg( (char *) "out.ogg" );
	sndconv1.makemp3( (char *) "out.mp3", highqal );

/*	while( buffer = sndconv1.popstream() )
	{
		sndconv1.pushstream( buffer );
		if(sndconv1.makefiles() == false)     // 		<-- makefiles is deprecated.
		{
			cout << "makefiles() failed." << endl;
			break;
		}
	}
*/

	while( ( buffer = (void *) sndconv1.popstream() ) )
		sndconv1.pushstream( buffer, 1024 );

	return 0;
}
