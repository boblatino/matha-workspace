#include "adaptivefilter.h"
#include <iostream>

using namespace std;

Matrix adaptfilt( Matrix input, Matrix reference, double order, Matrix (*method)( Matrix, Matrix, double, double ), double mu )
{
	if( !input.isvector() || !reference.isvector() )
	{
		cerr << "Error: Input and reference signals must be a vector" << endl;
		Matrix ret;
		return ret;
	}

	if( length( input ) != length( reference ) )
	{
		cerr << "Error: Input and reference signals should be of same length" << endl;
		Matrix ret;
		return ret;
	}

	size_t siz = size( input, 1 );
	if( siz == 1 )
	{
		input = input.transpose();
		siz = size( input, 1 );
	}
	if( size( reference, 1 ) == 1 )
		reference = reference.transpose();

	Matrix error = zeros( 1, 1 );
	Matrix d, y, u, w;
	Matrix output = input;
	complex <double> temp;

	u = flipud( input.getSubMatrix( 0, 0, 0, order - 1 ) );

	for( size_t i = 0; i < siz - order; i++ )
	{
		/* Getting current d. */
		d = reference.getSubMatrix( 0, 0, i + order - 1, i + order - 1 );
		/* Getting filter weights. */
		w = method( error, u, order, mu );
		/* Getting input. */
		u = flipud( input.getSubMatrix( 0, 0, i, i + order - 1 ) );
		/* Calculating output of filter and error. */
		y = w.transpose() * u;
		error = d - y;
		/* Setting ouput. */
		y.getElement( &temp, 0 );
		output.setElement( temp, i + order - 1 );
	}

	return output;
}

Matrix lms( Matrix error, Matrix input, double order, double mu )
{
	static Matrix w = zeros( order, 1 );
	w += input * error * mu;
	return w;
}
