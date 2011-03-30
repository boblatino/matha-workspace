#include "mwsFileBuilder.h"
#include "mwsConfigReader.h"
#include "mwsWorkspace.h"
#include "mwsWSO.h"
#include <map>
#include <string>
#include <list>	
#include <cstdlib>

using namespace std;

namespace mws
{

    mwsFileBuilder::mwsFileBuilder( string ConfigFileName ) : mwsBuilder()
    {
        configReader = new mwsConfigReader(ConfigFileName );
    }


    mwsFileBuilder::~mwsFileBuilder()
    {
        delete configReader;
    }

    void mwsFileBuilder::rebuildWS( mwsWorkspace * ws )
    {


    }


    void mwsFileBuilder::rebuildWSO( mwsWSO * wso )
    {


    }

    static inline mws_posv strtopos (std::string gg)
    {
        std::string nums = gg.substr (gg.find("[")+1, gg.length()-2);
        mws_posv pos(2);
        pos[0] = atoi (nums.substr (0, gg.find(",")-1).c_str());
        pos[1] = atoi (nums.substr (gg.find(","), gg.length()).c_str());
        return pos;       
    }
    
    mwsWorkspace * mwsFileBuilder::buildAll()
    {
        mwsWorkspace * workspace = NULL;
        mws_scopemap configList = configReader->getAll();
        list< mwsWSO *> objectList;

        cout<<"Config Reader reading "<<endl;
        std::cout<<*configReader;
        cout<<"Reader done "<<endl;
        mws_attrmap wsmap = configList["workspace"];
        /* FIXME: Need some generic parsing with error handling */
        workspace = new mwsWorkspace( atoi(wsmap["length"].c_str()),
                                      atoi(wsmap["width"].c_str()),
                                      atoi(wsmap["resolution"].c_str()));

        
        for( mws_scopeit listIterator = configList.begin();
             listIterator != configList.end();
             listIterator++ )
        {
            mws_attrmap amap = listIterator->second;
//            cout<<"Name is "<<listIterator->first<<endl;
            if (listIterator->first != "workspace")
            {
                //  cout<<"Name inside "<<listIterator->first<<endl;
                
                //parsing configuration
                workspace->addWSO(new mwsWSO(listIterator->first, 
                                             strtopos(amap["pos"]),
                                             strtopos(amap["vel"]),
                                             atoi (amap["radius"].c_str())));
            }
        }
        cout <<"Done with for "<<endl;
	return workspace;

    }

}
