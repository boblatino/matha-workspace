#include <iostream>
#include <string>
#include <strings.h>
#include "sndconv.h"

#define TYPE_MP3	0
#define TYPE_WAV	1
#define TYPE_OGG	2

using namespace std;

sndconv::sndconv( size_t bsize = 0)
{
	buffersize = bsize;
	filename = NULL;
	stmsze = 0;
	channels = 0;
	encoding = 0;
	rate = 0;
	read = 0;
	bstream = (unsigned char*) malloc(buffersize);
	mp3_buffer = (unsigned char*) malloc( (buffersize>7200?buffersize:7200)  );
}

int sndconv::readmp3( char* mp3file = NULL )
{
	if( mp3file == NULL )
		return -1;
	if( filename != NULL )
		return -2;
	type = TYPE_MP3;
	int ret = MPG123_OK;
	mpg123_init();
	mh = mpg123_new(NULL,&ret);
	mpg123_open(mh, mp3file);
	ret = mpg123_getformat(mh,&rate,&channels,&encoding);
	mpg123_format_none(mh);
	mpg123_format(mh, rate, channels, encoding);
	filename = mp3file;
	return 0;
}

int sndconv::readwav( char* wavfile = NULL )
{
	if( wavfile == NULL )
		return -1;
	if( filename != NULL )
		return -2;
	type = TYPE_WAV;
	SF_INFO info;
	if( ( sf_file = sf_open( wavfile, SFM_READ, &info )  ) == NULL )
		return -1;
	rate = info.samplerate;
	channels = info.channels;
	encoding = info.format;
	filename = wavfile;
	return 0;
}

int sndconv::readogg( char* oggfile = NULL )
{
	if( oggfile == NULL )
		return -1;
	if( filename != NULL )
		return -2;
	type = TYPE_OGG;
	SF_INFO info;
	if( ( sf_file = sf_open( oggfile, SFM_READ, &info )  ) == NULL )
		return -1;
	rate = info.samplerate;
	channels = info.channels;
	encoding = info.format;
	filename = oggfile;
	return 0;
}

void sndconv::pushstream( void *buffer, size_t buffsiz )
{
	if( (int)( sndflst.size() ) != 0 )
	{
		for(sndfls::const_iterator lii = sndflst.begin();lii != sndflst.end();lii++)
		{
			sf_write_double( *lii, (double *) buffer, buffsiz / sizeof( double ) );
		}
	}

	if( (int)( sndmp3flst.size() ) != 0 )
	{
		long * s_mv = (long *) buffer;
		for(mp3hand::const_iterator mp3ah = sndmp3flst.begin();mp3ah != sndmp3flst.end();mp3ah++)
		{
			write = lame_encode_buffer_long2( lame, s_mv, s_mv, buffsiz/sizeof(long), mp3_buffer, buffersize );
			if( write < 0 )
				cerr << "lame_encode_buffer(): " << write << endl;
			else
				fwrite(mp3_buffer, 1, write, *mp3ah);
		}
	}
}

void* sndconv::popstream()
{
	if( filename == NULL )
		return NULL;
	switch ( type )
	{
		case TYPE_MP3:
		{
			int ret = mpg123_read(mh,bstream,buffersize,&stmsze);
			if(ret == MPG123_OK)
			{
				read++;
				return (void*)bstream;
			}
			else
			{
				return NULL;
			}
			break;
		}
		case TYPE_WAV:
		case TYPE_OGG:
			if( sf_read_double( sf_file, (double*)bstream, buffersize / sizeof( double ) ) <= 0 )
				return NULL;
			read++;
			return (void *) bstream;
			break;
	}
	return NULL;
}


void sndconv::makemp3(char* outfile,int qualityint = highqal)
{
	FILE* mp3hh = fopen(outfile,"wb");
	lame = lame_init();
	lame_set_in_samplerate(lame, rate * channels );
	lame_set_num_channels(lame,channels);
	lame_set_quality(lame,qualityint);
	lame_set_VBR(lame, vbr_default);
	lame_init_params(lame);
	sndmp3flst.push_back(mp3hh);
	write = lame_encode_flush(lame, mp3_buffer, buffersize);
}


void sndconv::makewav(char* outfile)
{
	SF_INFO wavfinfo;
	bzero(&wavfinfo, sizeof(wavfinfo) );
	wavfinfo.samplerate = rate;
	wavfinfo.channels = channels;
	wavfinfo.format = SF_FORMAT_WAV|SF_FORMAT_PCM_16;
	SNDFILE* sndwavf = sf_open(outfile,SFM_WRITE,&wavfinfo);
	sndflst.push_back(sndwavf);
}


void sndconv::makeogg(char* outfile)
{
	SF_INFO oggfinfo;
	bzero(&oggfinfo, sizeof(oggfinfo) );
	oggfinfo.samplerate = rate;
	oggfinfo.channels = channels;
	oggfinfo.format = SF_FORMAT_OGG|SF_FORMAT_VORBIS;
	SNDFILE* sndoggf = sf_open(outfile,SFM_WRITE,&oggfinfo);
	sndflst.push_back(sndoggf);
}


bool sndconv::makefiles()
{
	if((((int )sndflst.size()) != 0) || ((int )sndmp3flst.size()) != 0)
	{
		for(sndfls::const_iterator lii = sndflst.begin();lii != sndflst.end();lii++)
		{
			sf_write_double(*lii,(double*)bstream,stmsze/sizeof(double));
		}

		short int * s_mv = (short int *) bstream;
		for(mp3hand::const_iterator mp3ah = sndmp3flst.begin();mp3ah != sndmp3flst.end();mp3ah++)
		{
			write = lame_encode_buffer(lame,s_mv, s_mv, stmsze/sizeof(short int), mp3_buffer, buffersize);
			if( write < 0 )
				cerr << "lame_encode_buffer(): " << write << endl;
			else
				fwrite(mp3_buffer, 1, write, *mp3ah);
		}
		return true;
	}
	else
	{
		return false;
	}
}


sndconv::~sndconv()
{
	//dtor
	for(sndfls::const_iterator lii = sndflst.begin();lii != sndflst.end();lii++)
	{
		sf_close(*lii);
	}
	for(mp3hand::const_iterator mp3ah = sndmp3flst.begin();mp3ah != sndmp3flst.end();mp3ah++)
	{
		fclose(*mp3ah);
	}
	free(bstream);
	free(mp3_buffer);
	if( type == TYPE_MP3 )
	{
		mpg123_close(mh);
		mpg123_delete(mh);
		mpg123_exit();
	}
	else if( type == TYPE_WAV || type == TYPE_OGG )
		sf_close( sf_file );
}
