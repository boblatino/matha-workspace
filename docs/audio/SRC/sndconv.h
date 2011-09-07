#ifndef SNDCONV_H
#define SNDCONV_H

#include <mpg123.h>
#include <sndfile.h>
#include <lame/lame.h>
#include <list>

typedef std::list<SNDFILE* > sndfls ;
typedef std::list<FILE* > mp3hand ;

enum
{
	highqal = 2,
	mediumqal = 5,
	lowqal = 7
};

class sndconv
{
	public :
		sndconv( size_t buffersize);
		int readmp3( char* mp3file );
		int readwav( char* wavfile );
		int readogg( char* oggfile );
		void* popstream();
		void pushstream( void *, size_t );
		void makemp3(char* outfile,int qualityint);
		void makewav(char* outfile);
		void makeogg(char* outfile);
		bool makefiles();
		virtual ~sndconv();
	private:
		char* filename;
		mpg123_handle *mh;
		SNDFILE* sf_file;
		lame_t lame;
		sndfls sndflst;
		mp3hand sndmp3flst;
		unsigned char* bstream;
		unsigned char* mp3_buffer;
		size_t stmsze;
		size_t buffersize;
		int channels;
		int encoding;
		long rate;
		int read;
		int write;
		int type;
};
#endif							 // SNDCONV_H
