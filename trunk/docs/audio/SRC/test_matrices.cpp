#include "matrices.h"
#include <iostream>

using namespace std;

int main()
{
	double i[ 20 ] = {1,3,5,7,9,11,13,15,17,19,0,2,4,6,8,10,12,14,16,18};
	double j[ 10 ] = { 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1 };
	double k[ 9 ] = { 1, 5, 2, 0, 3, 4, 2, 2, 1 };
	double l[ 16 ] = { 2, 0, 0, 0, 3, -1, 0, 3, 4, 2, 2, -6, 5, 1, 4, 0 };
	double m[ 20 ] = { 1, 4, 9, 16, 25, 36, 49, 64, 81, 100, 121, 144, 169, 196, 225, 256, 289, 324, 361, 400 };
	
	Matrix mat1;
	Matrix mat2( i, 4, 5 );
	Matrix mat3;

	cout << "Matrix 1: ";
	if( mat1.isempty() )
		cout <<	"Empty." << endl;
	else
	{
		size_t x, y;
		mat1.size( &y, &x );
		cout <<	x << "x" << y << "." << endl;
	}
	cout << "Matrix 2: ";
	if( mat2.isempty() )
		cout <<	"Empty." << endl;
	else
	{
		size_t x, y;
		mat2.size( &y, &x );
		cout <<	x << "x" << y << "." << endl;
	}

	mat2.printout();

	cout << "Adding a row." <<endl;
	mat2.addRow( j, 4 );
	mat2.printout();

	cout << "Adding a column." <<endl;
	mat2.addColumn( j, 6 );
	mat2.printout();

	cout << "Getting element 0x1." << endl;
	complex<double> ret;
        mat2.getElement( &ret, 0, 1 );
	cout << "0x1 -> " << ret << endl;

	cout << "Setting element 2x2." << endl;
	mat2.setElement( 50.4, 2, 2 );
	mat2.printout();

	cout << "mat1 = mat2" << endl;
	mat1 = mat2;
	mat1.printout();

	cout << "mat3 = mat1 + mat2" << endl;
	mat3 = mat1 + mat2;
	mat3.printout();

	cout << "mat1 = mat2 - mat3" << endl;
	mat1 = mat2 - mat3;
	mat1.printout();

	cout << "Clearing mat1." << endl;
	mat1.clear();
	mat1.printout();

	cout << "mat1 = mat2 .* mat3" << endl;
	mat1 = dotProduct( mat2, mat3 );
	mat1.printout();

	cout << "mat2 = mat1 ./ mat3" << endl;
	mat2 = dotDivision( mat1, mat3 );
	mat2.printout();

	cout << "Deleting row 2 of mat2" << endl;
	mat2.delRow( 2 );
	mat2.printout();

	cout << "Deleting column 3 of mat2" << endl;
	mat2.delColumn( 3 );
	mat2.printout();

	cout << " mat2 = mat2.transpose()" << endl;
	mat2 = mat2.transpose();
	mat2.printout();

	cout << "Creating mat4 3x3." << endl;
	Matrix mat4( k, 3, 3 );
	mat4.printout();
	cout << "Determinant = " << mat4.determinant() << endl;

	cout << "Creating mat5 4x4." << endl; 
	Matrix mat5( l, 4, 4 );
	mat5.printout();

	cout << "Inverse of mat5." << endl;
	mat5.inverse().printout();
	cout << mat5.determinant() << endl;

	cout << "mat5(1-3,0-2)" << endl;
	mat5.getSubMatrix(1,3,0,2).printout();

	cout << "exp(mat4)" << endl;
	exp(mat4).printout();

	cout << "round(exp(mat4))" << endl;
	round(exp(mat4)).printout();

	cout << "Creating mat6 of squares." << endl;
	Matrix mat6( m, 4, 5 );
	mat6.printout();

	cout << "Its sqrt()." <<endl;
	sqrt(mat6).printout();

	return 0;
}
