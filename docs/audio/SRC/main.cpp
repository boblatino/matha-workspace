#include <iostream>
#include <string>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <map>
#include "mwsConfigReader.h"

using namespace std;

#define TRUE 1
#define FALSE 0

#define MALE 1
#define FEMALE 2
#define MIXED 3
#define SAME 4
#define AUTODETECT 5
#define SLOWDOWN 6
#define SPEEDUP 7
#define SOFT 8
#define EXCITABLE 9

#define DEFAULT_FORMAT	0
#define FORMAT_MP3	0
#define FORMAT_OGG	1

struct arginf
{
	char* ifilename;
	std::map<char*,int> ofilename;
	char* cfilename;
//	int multio; //it's bool			WHAT IS THIS??
	int changevoice; //it's bool
	int inputvoice;
	int outputvoice;
	int tempo;
	int style;
}arginfo;

void configreader( char *filename );
int optionassign( char *option );
void formatDistribute( char *filenames, char *formats );
int formatof( char * format );

int main( int argc, char **argv )
{
	int opt = -1;
	int argop = 0;
	int argindex = 0;

	arginfo.ifilename = NULL;
//	arginfo.ofilename = NULL;
	arginfo.cfilename = NULL;
//	arginfo.multio = FALSE;
	arginfo.changevoice = FALSE;
	arginfo.inputvoice = AUTODETECT;
	arginfo.outputvoice = SAME;
	arginfo.tempo = SAME;
	arginfo.style = SAME;

	struct option myarg[] = {
		{"filename",required_argument,0,'i'},
		{"outputfile",required_argument,0,'o'},
		{"configfile",required_argument,0,'c'},
		{"changevoice",required_argument,0,'v'},
		{"inputvoice",required_argument,0,'f'},
		{"outputvoice",required_argument,0,'m'},
		{"tempo",required_argument,0,'t'},
		{"style",required_argument,0,'s'},
		{0,0,0,0}
	};

	while((argop = getopt_long(argc,argv,"i:o:c:v:f:m:t:s:",myarg,&argindex)) != -1)
	{
		switch(argop)
		{
			case 'i':
                		if(optarg != NULL)
                			arginfo.ifilename = optarg;
				else
					printf("bad option for argument --filename -i\n");
				break;

			case 'o':
				if(optarg != NULL)
					arginfo.ofilename.insert(pair<char*,int>(optarg,DEFAULT_FORMAT));
				else
					printf("bad option for argument --outputfile -o\n");
				break;

			case 'c':
				if(optarg != NULL)
				{
					arginfo.cfilename = optarg;
					configreader(arginfo.cfilename);
				}
				else
					printf("bad option for argument --configfile -c\n");
				break;

			case 'v':
				if(optarg != NULL)
				{
					if((strcmp(optarg,"yes") == 0) || (strcmp(optarg,"y") == 0))
					{
						arginfo.changevoice = TRUE;
					}
					else
					{
						if((strcmp(optarg,"no") != 0) && (strcmp(optarg,"n") != 0))
							printf("bad option for argument --changevoice -v\n");
						arginfo.changevoice = FALSE;
					}
				}
				break;

			case 'f':
				if(optarg != NULL)
				{
					opt = optionassign(optarg);
					if((opt == MALE) || (opt == FEMALE) || (opt == MIXED) || (opt == AUTODETECT))
						arginfo.inputvoice = opt;
					else
						printf("bad option for argument --inputvoice -f\n");
				}
				break;

			case 'm':
				if(optarg != NULL)
				{
					opt = optionassign(optarg);
					if((opt == MALE) || (opt == FEMALE) || (opt == MIXED) || (opt == AUTODETECT))
						arginfo.outputvoice = opt;
					else
						printf("bad option for argument --outputvoice -m\n");
                		}
				break;

			case 't':
				if(optarg != NULL)
				{
					opt = optionassign(optarg);
					if((opt == SAME) || (opt == SLOWDOWN) || (opt == SPEEDUP) || (opt == AUTODETECT))
						arginfo.tempo = optionassign(optarg);
					else
						printf("bad option for argument --tempo -t\n");
				}
				break;

			case 's':
				if(optarg != NULL)
				{
					opt = optionassign(optarg);
					if((opt == SAME) || (opt == SOFT) || (opt == EXCITABLE))
						arginfo.style = optionassign(optarg);
					else
						printf("bad option for argument --style -s\n");
				}
				break;

			case '?' || 'h':
				printf("that's the help %s \n",optarg);
				break;

			default:
				printf("none of the arguments\n");
		}
	}
	cout << "Input filename: " << arginfo.ifilename << endl;
	cout << "Configuration file: " << arginfo.cfilename << endl;
	for (map<char*,int>::iterator it =arginfo.ofilename.begin(); it!= arginfo.ofilename.end(); it++)
		cout << "Outfile name: " << it->first << "\t->\t" << it->second << endl;

}

int optionassign(char* option)
{
	if(strcmp(optarg,"male") == 0){
		return MALE;
	}
	if(strcmp(optarg,"female") == 0){
		return FEMALE;
	}
	if(strcmp(optarg,"mixed") == 0){
		return MIXED;
	}
	if(strcmp(optarg,"same") == 0){
		return SAME;
	}
	if(strcmp(optarg,"autodetect") == 0){
		return AUTODETECT;
	}
	if(strcmp(optarg,"slowdown") == 0){
		return SLOWDOWN;
	}
	if(strcmp(optarg,"speedup") == 0){
		return SPEEDUP;
	}
	if(strcmp(optarg,"soft") == 0){
		return SOFT;
	}
	if(strcmp(optarg,"excitable") == 0){
		return EXCITABLE;
	}
	return -1;
}

void configreader(char* filename)
{
	string filenames;
	filenames = filename;
	mwsConfigReader ahmed(filenames);

	if( arginfo.ifilename == NULL )
		arginfo.ifilename = (char* ) ahmed.getAttrib("file","filename").c_str();
	if( arginfo.ofilename.empty() )
		formatDistribute( (char *)ahmed.getAttrib("file","outputfile").c_str(),(char *)ahmed.getAttrib( "file", "format").c_str());
	if(ahmed.getAttrib("file","changevoice").empty())
	{
		printf("the bad option for argument --changevoice -v\n");
	}
	else
	{
		if((strcmp(ahmed.getAttrib("file","changevoice").c_str(),"yes") == 0) || (strcmp(ahmed.getAttrib("file","changevoice").c_str(),"y") == 0))
		{
			arginfo.changevoice = TRUE;
		}
		else
		{
			if((strcmp(ahmed.getAttrib("file","changevoice").c_str(),"no") != 0) && (strcmp(ahmed.getAttrib("file","changevoice").c_str(),"n") != 0))
			{
				printf("the bad option for argument --changevoice -v\n");
			}
			arginfo.changevoice = FALSE;
		}
	}
	int opt = -1;
	if((char* ) ahmed.getAttrib("file","inputvoice").empty())
	{
		opt = optionassign((char* ) ahmed.getAttrib("file","inputvoice").c_str());
		if((opt == MALE) || (opt == FEMALE) || (opt == MIXED) || (opt == AUTODETECT))
		{
			arginfo.inputvoice = opt;
		}
		else
		{
			printf("the bad option for argument --inputvoice -f\n");
		}
	}
	if((char* ) ahmed.getAttrib("file","outputvoice").empty())
	{
		opt = optionassign((char* ) ahmed.getAttrib("file","outputvoice").c_str());
		if((opt == MALE) || (opt == FEMALE) || (opt == MIXED) || (opt == AUTODETECT))
		{
			arginfo.outputvoice = opt;
		}
		else
		{
			printf("the bad option for argument --outputvoice -m\n");
		}
	}
	if((char* ) ahmed.getAttrib("file","tempo").empty())
	{
		opt = optionassign((char* ) ahmed.getAttrib("file","tempo").c_str());
		if((opt == SAME) || (opt == SLOWDOWN) || (opt == SPEEDUP) || (opt == AUTODETECT))
		{
			arginfo.tempo = opt;
		}
		else
		{
			printf("the bad option for argument --tempo -t\n");
		}
	}
	if((char* ) ahmed.getAttrib("file","style").empty())
	{
		opt = optionassign((char* ) ahmed.getAttrib("file","style").c_str());
		if((opt == SAME) || (opt == SOFT) || (opt == EXCITABLE))
		{
			arginfo.style = opt;
		}
		else
		{
			printf("the bad option for argument --style -s\n");
		}
	}
}

void formatDistribute( char *filenames, char *formats )
{
	char *temp; 

	if( !filenames )
		return;
	temp = strtok( filenames, "," );
	
	arginfo.ofilename.insert( pair<char *, int>( temp, DEFAULT_FORMAT ) );
	while( temp = strtok( NULL, "," ) )
	{
		arginfo.ofilename.insert( pair<char *, int>( temp, DEFAULT_FORMAT ) );
	}

	if( !formats )
	{
		cout << "No formats specified, using default format for all output files." << endl;
		return;
	}

	map< char*, int>::iterator it = arginfo.ofilename.begin();
	if( !( temp = strtok( formats, "," ) ) );
	{
		cout << "No formats specified, using default format for all output files." << endl;
		return;
	}
	it->second = formatof( temp );
	it++;

	while( temp = strtok( NULL, "," ) )
	{
		it->second = formatof( temp );
		it++;
	}

	if( it != arginfo.ofilename.end() )
		cout << "Number of formats is less than number of output files. Using default formats for the rest." << endl;

	return;
}

int formatof( char * format )
{
	if( !strncmp( format, "mp3", 3 ) )
		return FORMAT_MP3;
	if( !strncmp( format, "ogg", 3 ) )
		return FORMAT_OGG;
	cout << "Unkown format: '" << format << "'. Using default format instead." << endl;
	return DEFAULT_FORMAT;
}
