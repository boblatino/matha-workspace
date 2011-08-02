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

#define DEFAULT_FORMAT  0
#define FORMAT_MP3  0
#define FORMAT_OGG  1

struct arginf
{
	char *ifilename;
	std::map<char*,int> ofilename;
	char *cfilename;
	char *format;
	int changevoice;			 //it's bool
	int inputvoice;
	int outputvoice;
	int tempo;
	int style;
}arginfo;

struct arginf_mod
{
	bool ifilename;
	bool ofilename;
	bool cfilename;
	bool format;
	bool changevoice;
	bool inputvoice;
	bool outputvoice;
	bool tempo;
	bool style;
} argmod;

void configreader( char *filename );
int optionassign( char *option );
void formatDistribute( char *filenames, char *formats );
int formatof( char * format );

int main( int argc, char **argv )
{
	int opt = -1;
	int argop = 0;
	int argindex = 0;
	int ofncount = 0;
	int fmtcount = 0;
	map<char *, int>::iterator it;

	arginfo.ifilename = NULL;
	arginfo.format = NULL;
	arginfo.cfilename = NULL;
	arginfo.changevoice = FALSE;
	arginfo.inputvoice = AUTODETECT;
	arginfo.outputvoice = SAME;
	arginfo.tempo = SAME;
	arginfo.style = SAME;

	arginfo.ifilename = false;
	arginfo.format = false;
	arginfo.cfilename = false;
	arginfo.changevoice = false;
	arginfo.inputvoice = false;
	arginfo.outputvoice = false;
	arginfo.tempo = false;
	arginfo.style = false;

	struct option myarg[] =
	{
		{ "filename", required_argument, 0, 'i' },
		{ "outputfile", required_argument, 0, 'o' },
		{ "format", required_argument, 0, 'f' },
		{ "configfile", required_argument, 0, 'c' },
		{ "changevoice", required_argument, 0, 'v' },
		{ "inputvoice", required_argument, 0, 'z' },
		{ "outputvoice", required_argument, 0, 'm' },
		{ "tempo", required_argument, 0, 't' },
		{ "style", required_argument, 0, 's' },
		{ 0, 0, 0, 0 }
	};

	while((argop = getopt_long(argc,argv,"i:o:f:c:v:z:m:t:s:",myarg,&argindex)) != -1)
	{
		switch(argop)
		{
			case 'i':
				if(optarg != NULL)
				{
					arginfo.ifilename = optarg;
					argmod.ifilename = true;
				}
				else
					printf("bad option for argument --filename -i\n");
				break;

			case 'o':
				if(optarg != NULL)
				{
					ofncount++;
					arginfo.ofilename.insert(pair<char*,int>(optarg,DEFAULT_FORMAT));
					argmod.ofilename = true;
				}
				else
					printf("bad option for argument --outputfile -o\n");
				break;

			case 'f':
				if( fmtcount >= ofncount )
					cout << "Out of order arguments (Format without an output file). Please specify the output file first." << endl;
				else
				{
					it = arginfo.ofilename.begin();
					for( int i = 0; i < fmtcount; it++, i++);
					it->second = formatof( optarg );
					fmtcount++;
				}
				break;

			case 'c':
				if(optarg != NULL)
				{
					arginfo.cfilename = optarg;
					argmod.cfilename = true;
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
						argmod.changevoice = true;
						arginfo.changevoice = TRUE;
					}
					else if((strcmp(optarg,"no") == 0) || (strcmp(optarg,"n") == 0))
					{
						argmod.changevoice = true;
						arginfo.changevoice = FALSE;
					}
					else
						cout << "Unknown option '" << optarg << "' for changevoice." << endl;
				}
				break;

			case 'z':
				if(optarg != NULL)
				{
					opt = optionassign(optarg);
					if((opt == MALE) || (opt == FEMALE) || (opt == MIXED) || (opt == AUTODETECT))
					{
						argmod.inputvoice = true;
						arginfo.inputvoice = opt;
					}
					else
						cout << "Unknown option '" << optarg << "' for inputvoice." << endl;

				}
				break;

			case 'm':
				if(optarg != NULL)
				{
					opt = optionassign(optarg);
					if((opt == MALE) || (opt == FEMALE) || (opt == MIXED) || (opt == AUTODETECT))
					{
						argmod.outputvoice = true;
						arginfo.outputvoice = opt;
					}
					else
						cout << "Unknown option '" << optarg << "' for outputvoice." << endl;
				}
				break;

			case 't':
				if(optarg != NULL)
				{
					opt = optionassign(optarg);
					if((opt == SAME) || (opt == SLOWDOWN) || (opt == SPEEDUP) || (opt == AUTODETECT))
					{
						argmod.tempo = true;
						arginfo.tempo = opt;
					}
					else
						cout << "Unknown option '" << optarg << "' for tempo." << endl;
				}
				break;

			case 's':
				if(optarg != NULL)
				{
					opt = optionassign(optarg);
					if((opt == SAME) || (opt == SOFT) || (opt == EXCITABLE))
					{
						argmod.style = true;
						arginfo.style = opt;
					}
					else
						cout << "Unknown option '" << optarg << "' for style." << endl;
				}
				break;

			case '?' || 'h':
				printf("that's the help %s \n",optarg);
				break;

			default:
				cout << "Unknown argument: " << argop << ' ' << optarg << endl;
		}
	}

	cout << "Input filename: " << arginfo.ifilename << endl;
	cout << "Configuration file: " << arginfo.cfilename << endl;
	for( it =arginfo.ofilename.begin(); it!= arginfo.ofilename.end(); it++)
		cout << "Outfile name: " << it->first << "\tFormat: " << it->second << endl;
	cout << "Changevoice=" << arginfo.changevoice << endl;
	cout << "Inputvoice=" << arginfo.inputvoice << endl;
	cout << "Outputvoice=" << arginfo.outputvoice << endl;
	cout << "Tempo=" << arginfo.tempo << endl;
	cout << "Style=" << arginfo.style << endl;
}


int optionassign(char* option)
{
	if(strcmp(option,"male") == 0)
	{
		return MALE;
	}
	if(strcmp(option,"female") == 0)
	{
		return FEMALE;
	}
	if(strcmp(option,"mixed") == 0)
	{
		return MIXED;
	}
	if(strcmp(option,"same") == 0)
	{
		return SAME;
	}
	if(strcmp(option,"autodetect") == 0)
	{
		return AUTODETECT;
	}
	if(strcmp(option,"slowdown") == 0)
	{
		return SLOWDOWN;
	}
	if(strcmp(option,"speedup") == 0)
	{
		return SPEEDUP;
	}
	if(strcmp(option,"soft") == 0)
	{
		return SOFT;
	}
	if(strcmp(option,"excitable") == 0)
	{
		return EXCITABLE;
	}
	return -1;
}


void configreader(char* filename)
{
	string filenames;
	filenames = filename;
	mwsConfigReader config(filenames);

	if( !argmod.ifilename )
		arginfo.ifilename = (char* ) config.getAttrib("file","filename").c_str();

	if( !argmod.ofilename )
		formatDistribute( (char *)config.getAttrib("file","outputfile").c_str(),(char *)config.getAttrib( "file", "format").c_str());

	if( !argmod.changevoice && !config.getAttrib("file","changevoice").empty() )
	{
		if((strcmp(config.getAttrib("file","changevoice").c_str(),"yes") == 0) || (strcmp(config.getAttrib("file","changevoice").c_str(),"y") == 0))
			arginfo.changevoice = TRUE;
		else if((strcmp(config.getAttrib("file","changevoice").c_str(),"no") == 0) || (strcmp(config.getAttrib("file","changevoice").c_str(),"n") == 0))
			arginfo.changevoice = FALSE;
		else
			cout << "Error parsing configuration file at 'changevoice:" << config.getAttrib("file","changevoice").c_str() << "'" << endl;
	}

	int opt = -1;
	if( !argmod.inputvoice && !config.getAttrib("file","inputvoice").empty() )
	{
		opt = optionassign((char* ) config.getAttrib("file","inputvoice").c_str());
		if((opt == MALE) || (opt == FEMALE) || (opt == MIXED) || (opt == AUTODETECT))
			arginfo.inputvoice = opt;
		else
			cout << "Error parsing configuration file at 'inputvoice:" << config.getAttrib("file","inputvoice").c_str() << "'" << endl;
	}

	if( !argmod.outputvoice && !config.getAttrib("file","outputvoice").empty() )
	{
		opt = optionassign((char* ) config.getAttrib("file","outputvoice").c_str());
		if((opt == MALE) || (opt == FEMALE) || (opt == MIXED) || (opt == AUTODETECT))
			arginfo.outputvoice = opt;
		else
			cout << "Error parsing configuration file at 'outputvoice:" << config.getAttrib("file","outputvoice").c_str() << "'" << endl;
	}

	if( !argmod.tempo && !config.getAttrib("file","tempo").empty() )
	{
		opt = optionassign((char* ) config.getAttrib("file","tempo").c_str());
		if((opt == SAME) || (opt == SLOWDOWN) || (opt == SPEEDUP) || (opt == AUTODETECT))
			arginfo.tempo = opt;
		else
			cout << "Error parsing configuration file at 'tempo:" << config.getAttrib("file","tempo").c_str() << "'" << endl;
	}

	if( !argmod.style && !config.getAttrib("file","style").empty() )
	{
		opt = optionassign((char* ) config.getAttrib("file","style").c_str());
		if((opt == SAME) || (opt == SOFT) || (opt == EXCITABLE))
			arginfo.style = opt;
		else
			cout << "Error parsing configuration file at 'style:" << config.getAttrib("file","style").c_str() << "'" << endl;
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
