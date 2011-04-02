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

    static vector<int > spliter(string split,string ob){
        string rsplit;
        vector<int > data;
        int last = -1;
        for(int i = 0;i != -1;last = i){
            i = split.find(ob);
            if(i != -1){
                rsplit = split.substr(0,i);
                split = split.substr(i + 1,split.length()-1);
                data.push_back(atoi(rsplit.c_str()));
            }else{
                data.push_back(atoi(split.c_str()));
            }
        
        }
        return data;
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
                vector<int > vcolor;
                vcolor =  spliter(amap["color"],",");
                mwscolors scolor(vcolor[0],vcolor[1],vcolor[2]);
                //               mwsshapes awshape(atoi(amap["shape"]),);
                workspace->addWSO(new mwsWSO(listIterator->first, 
                                             strtopos(amap["pos"]),
                                             strtopos(amap["vel"]),
                                             atoi (amap["radius"].c_str()),scolor,
                                             mwsshapes ((mwsShapeEnum) atoi(amap["shape"].c_str()),
                                                        spliter(amap["dims"], ",")))
                                  );
                cout<<" Split: "<<spliter(amap["dims"], ",")[0]<<endl;
            }
        }
        cout <<"Done with for "<<endl;
	return workspace;

    }

}
