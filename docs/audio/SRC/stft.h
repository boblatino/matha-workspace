#ifndef __STFT__
#define __STFT__

#include "matrices.h"

Matrix stft( Matrix x, double f, Matrix w, double h, double sr );
Matrix stft( Matrix x, double f, Matrix w, double h );
Matrix stft( Matrix x, double f, Matrix w );
Matrix stft( Matrix x, double f );
Matrix stft( Matrix x );

#endif//__STFT__
