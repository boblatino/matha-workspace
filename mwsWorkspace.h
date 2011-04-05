#ifndef MWSWORKSPACE_H
#define MWSWORKSPACE_H

#include <mwsTypes.h>
#include <mwsWSO.h>
#include <map>

typedef std::map<mws_string, mwsWSO *> mws_omap;

class mwsWorkspace
{
  private:
    mws_omap wsoMap;
    int length;
    int width;
    int res;
    
  public:
    mwsWorkspace (int len=400, int wid=400, int resolution=1);
    ~mwsWorkspace() {}
        
    void addWSO (mwsWSO *obj);

    void go();

    void stop(){};

    void step();
};

#endif
