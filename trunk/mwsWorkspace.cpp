#include <mwsWorkspace.h>
#include <mwsGraphicAdapter.h>
#include <iostream>

mwsWorkspace::mwsWorkspace (int len, int wid, int resolution) :
    length (len), width(wid), res(resolution)
{
    float clr[4]={0,0,0,1};
    std::cout<<"Length is "<<len<<" width: "<<wid<<std::endl;
    mwsGraphicAdapter::getInstance()->mainWin(len, wid, clr, false);
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
    while (1){
        /* FIXME: Need motion logic */ 
        sleep(1);
        std::cout<<"doing something"<<std::endl;
    }
}
