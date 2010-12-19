#include "mwsFileBuilder.h"
#include "mwsConfigReader.h"
#include "mwsWorkspace.h"
#include "mwsIWSO.h"
#include <map>
#include <string>
#include <list>	
#include <cstdlib>

using namespace std;

namespace mws
{

	mwsFileBuilder::mwsFileBuilder( string ConfigFileName )
	{
		mwsFileBuilder::configReader = new mwsConfigReader(
				ConfigFileName );
	}


	mwsFileBuilder::~mwsFileBuilder()
	{
		delete mwsFileBuilder::configReader;
	}

	void mwsFileBuilder::rebuildWS( mwsWorkspace * ws )
	{


	}


	void mwsFileBuilder::rebuildWSO( mwsWSO * wso )
	{


	}


	mwsWorkspace * mwsFileBuilder::buildAll()
	{
		mwsWorkspace * workspace = NULL;
		map< string, map< string, string > > configList =
			configReader->getAll();
		map< string, map< string, string > >::iterator listIterator;
		list< mwsWso > objectList;

		for( listIterator = configList.begin();
				listIterator!=configReader.end();
				listIterator++ )
		{
			switch( listIterator->"name" )
			{
				case "workspace":
					int length = ( listIterator->find(
							"length" ) !=
						configList.end() ) ?
						atoi( (listIterator->"length").c_str ) :
						WS_DEFAULT_LENGTH;
					int width = ( listIterator->find(
							"width" ) !=
						configList.end() ) ?
						atoi( (listIterator->"width").c_str ) :
						WS_DEFAULT_WIDTH;
					int resolution = ( listIterator->find(
							"resolution" ) !=
						configList.end() ) ?
						atoi( (	listIterator-> "resolution").c_str ) :
						WS_DEFAULT_RESOLUTION;

					workspace = new mwsWorkspace( length,
							width, resolution );

					while( !objectList.empty() )
					{
						workspace->addwso(
								objectList.begin()
								);
						objectList.pop_front();
					}
					break;

				case "robot":



					break;

				case "obstacle":
					//parsing configuration

					mwsWso * newObject = new mwsWso;


					if( workspace )
						workspace->addwso( *newObject );
					else
						objectList.push_back( *newObject );
					break;

				default:
					delete configReader;
					return NULL;
			}


		}

	return workspace;

	}

}
