#include "mwsDraw.h"
#include <stdlib.h>
#include <stdexcept>

#ifdef WIN32
   #include <windows.h>
#endif

#include <gL/glut.h>
#include <functional>


///////////////////////////////////////////

void mwsDrawStub();
void reshape(int w,int h);
void keya(unsigned char key,int x,int y);

///////////////////////////////////////////

mwscolors::mwscolors(float r,float g,float b,float alphaa){
	wscolor[0] = r;
	wscolor[1] = g;
	wscolor[2] = b;
	wscolor[3] = alphaa;
}

float* mwscolors::getcolors(){
	return wscolor;
}

///////////////////////////////////////////

mwsshapes::mwsshapes(mwsShapeEnum ashape = mwsCircle,ldims dimsint = ldims(0,0)){
    shape = ashape;
    dims = dimsint;
}

mwsShapeEnum mwsshapes::getshape(){
         return shape;
}

ldims mwsshapes::getdims(){
         return dims;
}

///////////////////////////////////////////

mwsDrawable::mwsDrawable(mwscolors &colorf,mwsshapes &shapeint,int radiusint,posv posint):shape(shapeint){
         color = colorf;
         radius = radiusint;

}

mwscolors mwsDrawable::getcolor(){
         return color;
}

mwsshapes mwsDrawable::getshape(){
         return shape;
}

int mwsDrawable::getradius(){
         return radius;
}

posv mwsDrawable::getpos(){
         return pos;
}

void mwsDrawable::setpos(posv posint){
         pos = posint;
}

///////////////////////////////////////////

mwsGraphicsAdapter *mwsGraphicsAdapter::instance=NULL;

mwsGraphicsAdapter::mwsGraphicsAdapter() {
     int ac = 1;
     char *av = "Graphic Adapter";
	 glutInit(&ac,&av);
     glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
}

void mwsGraphicsAdapter::mainWin(int width, int len,float wincolor[4],bool full=true) {
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

mwsGraphicsAdapter *mwsGraphicsAdapter::getInstance(){
	if(!(instance)){
       instance = new mwsGraphicsAdapter;
    }
       return instance;
}

void mwsGraphicsAdapter::make(mwsDrawable &drawthis){
        shapeso.push_back(&drawthis);
}

void mwsGraphicsAdapter::draw(){
        glClearColor(windowcolor[0],windowcolor[1],windowcolor[2],windowcolor[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

        for(dlist::iterator i = shapeso.begin(); i != shapeso.end();++i){
	       if((*i)->getshape().getshape() == mwsCircle){
	           DrawCircle((*i)->getpos(), (*i)->getradius(), (*i)->getcolor().getcolors());
	       }else{
               DrawPolygon((*i)->getpos(), (*i)->getradius(), (*i)->getcolor().getcolors());
	       }
        }

        glutSwapBuffers();
}

void mwsGraphicsAdapter::display(){
         glutDisplayFunc(mwsDrawStub);
         if(fullwin == true){
            glutFullScreen();
         }
         glutReshapeFunc(reshape);
         glutKeyboardFunc(keya);

         glutMainLoop();
}

void mwsGraphicsAdapter::DrawPolygon(posv shapeos,int gradius,float* colorsa){
        glPushMatrix();
        glTranslatef((float)(shapeos[0] - gradius),(float)(shapeos[1] - gradius),0);
        glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,colorsa);
        glRectf(0,0,(float) gradius*2,(float) gradius*2);
        glPopMatrix();
}

void mwsGraphicsAdapter::DrawCircle(posv shapeos,int gradius,float* colorsa){
        glPushMatrix();
        GLUquadricObj *glopj;
        glopj = gluNewQuadric();
        glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,colorsa);
        glTranslatef((float)shapeos[0],(float)shapeos[1], 0);
        gluDisk(glopj, 0, gradius, 32, 4);
        glPopMatrix();
}

void mwsGraphicsAdapter::refresh(){
    draw();
}

///////////////////////////////////////////

void mwsDrawStub() {
    mwsGraphicsAdapter::getInstance()->draw();
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
