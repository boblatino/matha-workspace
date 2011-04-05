#include <mwsWorkspace.h>
#include <mwsGraphicAdapter.h>
#include <iostream>

mwsWorkspace::mwsWorkspace (int len, int wid, int resolution) :
    length (len), width(wid), res(resolution)
{
    float clr[4]={0,0,0,1};
    std::cout<<"Length is "<<len<<" width: "<<wid<<std::endl;
    mwsGraphicAdapter::getInstance()->mainWin(len, wid, clr, this, false);
}

void mwsWorkspace::addWSO(mwsWSO *obj)
{
    wsoMap[obj->getname()] = obj;
    mwsGraphicAdapter::getInstance()->make(obj);
    std::cout<<"Added object "<<obj->getname();
}

void mwsWorkspace::go()
{
    mwsGraphicAdapter::getInstance()->draw();
    mwsGraphicAdapter::getInstance()->display();
    
#if 0
    while (1){
        /* FIXME: Need motion logic */ 
        sleep(1);
        std::cout<<"doing something"<<std::endl;
    }
#endif
}

void mwsWorkspace::step()
{
    for (mws_omap::iterator it = wsoMap.begin(); it != wsoMap.end(); it++) {
        mws_posv pos = it->second->getpos();
        pos[0] += it->second->getvel()[0];
        pos[1] += it->second->getvel()[1];        
        it->second->setpos(pos);
        //   std::cout<<"doing something"<<it->second->pos[0]<<","
        //       <<it->second->pos[1]<<std::endl;
    }

}
