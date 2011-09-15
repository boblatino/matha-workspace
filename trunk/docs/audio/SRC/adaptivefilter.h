#ifndef __LMS_H__
#define __LMS_H__

#include "matrices.h"

Matrix adaptfilt( Matrix input, Matrix reference, double order, Matrix (*method)( Matrix, Matrix, double, double ), double mu );
Matrix lms( Matrix error, Matrix input, double order, double mu );


#endif//__LMS_H__
