#include <iostream>
#include "sndconv.h"

using namespace std;

int main()
{
	char *buffer;
	sndconv sndconv1( (char *) "file.mp3", 1024 );
	sndconv1.makewav( (char *) "out.wav" );
	sndconv1.makeogg( (char *) "out.ogg" );
	sndconv1.makemp3( (char *) "out.mp3", highqal );

	while( buffer = sndconv1.popstream() )
	{
		sndconv1.pushstream( buffer );
		if(sndconv1.makefiles() == false)
		{
			cout << "makefiles() failed." << endl;
			break;
		}
	}
	return 0;
}
