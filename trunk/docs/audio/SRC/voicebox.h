#ifndef __VOICEBOX_H__
#define __VOICEBOX_H__

#include "matrices.h"

/* estnoisem(). */
void estnoisem( );

/* specsub(). */
void specsub();

/* rfft(). */
Matrix rfft( Matrix x );
Matrix rfft( Matrix x, size_t n );
Matrix rfft( Matrix x, size_t n, size_t d );

/* irfft(). */
Matrix irfft( Matrix y );
Matrix irfft( Matrix y, size_t n );
Matrix irfft( Matrix y, size_t n, size_t d );

/* enframe(). */
Matrix enframe( Matrix x, Matrix win );
Matrix enframe( Matrix x, Matrix win, size_t len );


#endif//__VOICEBOX_H__
