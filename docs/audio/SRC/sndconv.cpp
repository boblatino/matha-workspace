#include <iostream>
#include <string>
#include <strings.h>
#include "sndconv.h"

using namespace std;

sndconv::sndconv(char* filepath = NULL,size_t bsize = 0)
{
	buffersize = bsize;
	if(filepath != NULL)
	{
		filename = filepath;
		stmsze = 0;
		channels = 0;
		encoding = 0;
		rate = 0;
		read = 0;
		int ret = MPG123_OK;
		mpg123_init();
		mh = mpg123_new(NULL,&ret);
		mpg123_open(mh,filepath);
		ret = mpg123_getformat(mh,&rate,&channels,&encoding);
		bstream = (unsigned char*) malloc(buffersize);
		mp3_buffer = (unsigned char*) malloc(buffersize);
		mpg123_format_none(mh);
		mpg123_format(mh, rate, channels, encoding);
	}
	else
	{
		filename = NULL;
	}
}

void sndconv::pushstream( char *buffer, size_t buffsiz )
{
	if( (int)( sndflst.size() ) != 0 )
	{
		for(sndfls::const_iterator lii = sndflst.begin();lii != sndflst.end();lii++)
		{
			sf_write_short( *lii, (short *)buffer, buffsiz/sizeof(short) );
		}
	}

	if( (int)( sndmp3flst.size() ) != 0 )
	{
		short int * s_mv = (short int *) buffer;
		for(mp3hand::const_iterator mp3ah = sndmp3flst.begin();mp3ah != sndmp3flst.end();mp3ah++)
		{
			write = lame_encode_buffer( lame, s_mv, s_mv, buffsiz/sizeof(short int), mp3_buffer, buffersize );
			if( write < 0 )
				cerr << "lame_encode_buffer(): " << write << endl;
			else
				fwrite(mp3_buffer, 1, write, *mp3ah);
		}
	}
}

char* sndconv::popstream()
{
	int ret = mpg123_read(mh,bstream,buffersize,&stmsze);
	if(ret == MPG123_OK)
	{
		read++;
		return (char*)bstream;
	}
	else
	{
		return NULL;
	}
}


void sndconv::makemp3(char* outfile,int qualityint = highqal)
{
	FILE* mp3hh = fopen(outfile,"wb");
	lame = lame_init();
	lame_set_in_samplerate(lame, rate*2);
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
			sf_write_short(*lii,(short*)bstream,stmsze/sizeof(short));
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
	mpg123_close(mh);
	mpg123_delete(mh);
	mpg123_exit();
}
