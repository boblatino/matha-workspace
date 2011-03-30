#ifndef __mwsDraw__
#define __mwsDraw__

#include <list>
#include <mwsTypes.h>
#include <mwsDrawable.h>

typedef std::list<mwsDrawable *> dlist;

//////////////////////////////////////////////////////////

class mwsGraphicAdapter{
    private:
       bool fullwin;
       float windowcolor[4];
       static mwsGraphicAdapter *instance;
       dlist shapeso;
       int mainWinWidth;
       int mainWinLength;
       mwsGraphicAdapter();
       void DrawPolygon(mws_posv shapeos,int gradius,float* colorsa);
       void DrawCircle(mws_posv shapeos,int gradius,float* colorsa);
    public:
       static mwsGraphicAdapter *getInstance();
       void make(mwsDrawable *drawthis);
       void refresh();
       void mainWin(int width, int len,float wincolor[4],bool full);
       void display();
       void draw();
};

///////////////////////////////////////////

#endif


