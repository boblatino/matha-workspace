#ifndef __ISTFT__
#define __ISTFT__

#include "matrices.h"

Matrix istft( Matrix x, double f, Matrix w, double h, double sr );
Matrix istft( Matrix x, double f, Matrix w, double h );
Matrix istft( Matrix x, double f, Matrix w );
Matrix istft( Matrix x, double f );
Matrix istft( Matrix x );

#endif//__ISTFT__
