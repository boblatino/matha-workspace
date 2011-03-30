#include <mwsDrawable.h>

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

mwsshapes::mwsshapes(mwsShapeEnum ashape,mws_ldims dimsint){
    shape = ashape;
    dims = dimsint;
}

mwsShapeEnum mwsshapes::getshape(){
         return shape;
}

mws_ldims mwsshapes::getdims(){
         return dims;
}

///////////////////////////////////////////

mwsDrawable::mwsDrawable(mwscolors colorf,mwsshapes shapeint,int radiusint,mws_posv posint):shape(shapeint){
         color = colorf;
         radius = radiusint;
         pos = posint;
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

mws_posv mwsDrawable::getpos(){
         return pos;
}

void mwsDrawable::setpos(mws_posv posint){
         pos = posint;
}

///////////////////////////////////////////

