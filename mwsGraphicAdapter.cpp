#include "mwsGraphicAdapter.h"
#include <stdlib.h>
#include <stdexcept>
#include <iostream>
#include <math.h>
#include <vector>

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/glut.h>
#include <functional>


///////////////////////////////////////////

void mwsDrawStub();
void reshape(int w,int h);
void keya(unsigned char key,int x,int y);

///////////////////////////////////////////

mwsGraphicAdapter *mwsGraphicAdapter::instance=NULL;

mwsGraphicAdapter::mwsGraphicAdapter() {
    int ac = 1;
    char *av = "Graphic Adapter";
    glutInit(&ac,&av);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
}

void mwsGraphicAdapter::mainWin(int width, int len,float wincolor[4],bool full=true) {
    mainWinWidth = width;
    mainWinLength = len;
    /* Draw window with this width and length */
    windowcolor[0] = wincolor[0];
    windowcolor[1] = wincolor[1];
    windowcolor[2] = wincolor[2];
    windowcolor[3] = wincolor[3];
    fullwin = full;
    glutInitWindowPosition(100,100);
    glutInitWindowSize(width,len);
    glutCreateWindow("Matha");
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}

mwsGraphicAdapter *mwsGraphicAdapter::getInstance(){
    if(!(instance)){
        instance = new mwsGraphicAdapter;
    }
    return instance;
}

void mwsGraphicAdapter::make(mwsDrawable *drawthis){
    shapeso.push_back(&(*drawthis));
}

void mwsGraphicAdapter::draw(){

    /* FIXME: GL crashes if this is not right, so we need error handling */
    
    glClearColor(windowcolor[0],windowcolor[1],windowcolor[2],windowcolor[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    for(dlist::iterator i = shapeso.begin(); i != shapeso.end();++i){
        if((*i)->getshape().getshape() == mwsCircle){
            DrawCircle((*i)->getpos(), (*i)->getradius(), (*i)->getcolor().getcolors());
        }else{
            DrawPolygon((*i)->getpos(), (*i)->getshape().getdims(), (*i)->getcolor().getcolors());
        }
    }

    glutSwapBuffers();
}

void mwsGraphicAdapter::display(){
    glutDisplayFunc(mwsDrawStub);
    if(fullwin == true){
        glutFullScreen();
    }
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keya);

    glutMainLoop();
}

void mwsGraphicAdapter::DrawPolygon(mws_posv shapeos,mws_ldims dims,float* colorsa){
    glPushMatrix();
    std::cout<<" Dims "<<dims[0]<<std::endl;
    glTranslatef((float)(shapeos[0] - dims[0])*2/mainWinWidth,(float)(shapeos[1] - dims[1])*2/mainWinLength,0);
    glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,colorsa);
    glRectf(0,0,(float) dims[0]*2/mainWinWidth,(float) dims[1]*2/mainWinLength);
    glPopMatrix();
}

void mwsGraphicAdapter::DrawCircle(mws_posv shapeos,int gradius,float* colorsa){
    glPushMatrix();
    GLUquadricObj *glopj;
    glopj = gluNewQuadric();
    glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,colorsa);
    glTranslatef((float)shapeos[0]*2/mainWinWidth,(float)shapeos[1]*2/mainWinLength, 0);
    gluDisk(glopj, 0, (float) gradius*2/mainWinWidth, 32, 4);
    glPopMatrix();
}

void mwsGraphicAdapter::refresh(){
    //draw();
}

///////////////////////////////////////////

void mwsDrawStub() {
    mwsGraphicAdapter::getInstance()->draw();
}

///////////////////////////////////////////

void reshape(int w,int h){
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0,(float)w/(float)h,1.0,100.0);
    gluLookAt(0,0,10,0,0,0,0,1,0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

///////////////////////////////////////////

void keya(unsigned char key,int x,int y){
    if(key == 27){
        exit(0);
    }
}

///////////////////////////////////////////
