#ifndef __MWS_DRAWABLE__
#define __MWS_DRAWABLE__

#include <mwsTypes.h>

/////////////////////////////////////////////////////////

class mwsshapes{
    private:
       mwsShapeEnum shape;
       mws_ldims dims;
    public:
       mwsshapes(mwsShapeEnum ashape=mwsCircle,
                 mws_ldims dimsint=mws_ldims(1,1));
       mwsShapeEnum getshape();
       mws_ldims getdims();
};

/////////////////////////////////////////////////////////

class mwscolors{
   private:
      float wscolor[4];
   public:
      mwscolors(float r=1,float g=0,float b=0,float alphaa=1);
      float* getcolors();
};

//////////////////////////////////////////////////////////

class mwsDrawable{
    private:
       mwscolors color;
       mwsshapes shape;
       int radius;
       mws_posv pos;
    public:
       mwsDrawable(mwscolors colorf,mwsshapes shapeint,int radius,mws_posv posint);
       mwscolors getcolor();
       mwsshapes getshape();
       int getradius();
       mws_posv getpos();
       void setpos(mws_posv posint);
};

//////////////////////////////////////////////////////////


#endif
