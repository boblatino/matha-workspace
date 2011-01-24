#ifndef __mwsDraw__
#define __mwsDraw__

#include <vector>
#include <list>

typedef enum
{
	mwsCircle = 0,
	mwsSquare = 1
} mwsShapeEnum;

typedef std::vector<int > ldims;
typedef std::vector<int > posv;


/////////////////////////////////////////////////////////

class mwsshapes{
    private:
       mwsShapeEnum shape;
       ldims dims;
    public:
       mwsshapes(mwsShapeEnum ashape,ldims dimsint);
       mwsShapeEnum getshape();
       ldims getdims();
};

/////////////////////////////////////////////////////////

class mwscolors{
   private:
      float wscolor[4];
   public:
      mwscolors(float r=0,float g=0,float b=0,float alphaa=1);
      float* getcolors();
};

//////////////////////////////////////////////////////////

class mwsDrawable{
    private:
       mwscolors color;
       mwsshapes shape;
       int radius;
       posv pos;
    public:
       mwsDrawable(mwscolors &colorf,mwsshapes &shapeint,int radius,posv posint);
       mwscolors getcolor();
       mwsshapes getshape();
       int getradius();
       posv getpos();
       void setpos(posv posint);
};

//////////////////////////////////////////////////////////

typedef std::list<mwsDrawable *> dlist;

//////////////////////////////////////////////////////////

class mwsGraphicsAdapter{
    private:
       bool fullwin;
       float windowcolor[4];
       static mwsGraphicsAdapter *instance;
       dlist shapeso;
       mwsGraphicsAdapter();
       void DrawPolygon(posv shapeos,int gradius,float* colorsa);
       void DrawCircle(posv shapeos,int gradius,float* colorsa);
    public:
       static mwsGraphicsAdapter *getInstance();
       void make(mwsDrawable &drawthis);
       void refresh();
       void mainWin(int width, int len,float wincolor[4],bool full);
       void display();
       void draw();
};

///////////////////////////////////////////

#endif


