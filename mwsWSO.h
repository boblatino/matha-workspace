#ifndef __MWS_WSO__
#define __MWS_WSO__

#include <mwsTypes.h>
#include <mwsDrawable.h>

class mwsWSO : public mwsDrawable
{
  private:
    mws_posv pos;
    mws_posv vel;
    mws_string name;

  public:
    mwsWSO(mws_string nm="default", mws_posv ps=mws_posv(2),
           mws_posv vl=mws_posv(2), int radius=1,
           mwscolors colorf=mwscolors(),
           mwsshapes shapeint=mwsshapes()) :
        mwsDrawable (colorf, shapeint, radius, ps),
        pos(ps), vel(vl), name (nm)  {};
    virtual ~mwsWSO(){};
    inline mws_string& getname() { return name;}
    inline mws_posv& getpos() {return pos;}
    inline mws_posv& getvel() {return vel;}
    
};


#endif
