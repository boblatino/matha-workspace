#ifndef __mwsConfigReader__
#define __mwsConfigReader__

#include <map>
#include <string>
#include <iostream>
#include <mwsTypes.h>

typedef std::map<std::string , std::string > mws_attrmap;
typedef mws_attrmap::iterator mws_attrit;
typedef std::map<std::string , mws_attrmap> mws_scopemap;
typedef mws_scopemap::iterator mws_scopeit;

class mwsConfigReader {
   private:
    mws_scopemap data;
    mws_string filename;
    bool readconf(mws_string ,mws_scopemap &);
  public :
    mwsConfigReader(mws_string filen);  
    mws_string getAttrib(mws_string scope,mws_string attrib);
    mws_attrmap getScope(mws_string scope);
    mws_scopemap getAll();

    friend std::ostream& operator<<(std::ostream &out, mwsConfigReader &reader) {
        mws_scopemap smap = reader.getAll();
        for (mws_scopeit sit = smap.begin(); sit != smap.end(); sit++) {
            out<<"["<<sit->first<<"]"<<std::endl;
            mws_attrmap amap = sit->second;
            for(mws_attrit ait = amap.begin(); ait != amap.end(); ait++) {
                out<<"\t"<<ait->first<<" = "<<ait->second<<std::endl;
            }
        }
        return out;
    }
};

#endif
