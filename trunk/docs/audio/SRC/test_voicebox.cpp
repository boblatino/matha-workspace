#include "voicebox.h"
#include "matrices.h"
#include <iostream>

using namespace std;

int main()
{
	double x[ 25 ] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24};
	double w[ 6 ] = { 1, 2, 3, 3, 2, 1 };

	Matrix m1( x, 5, 5 );
	Matrix win;
	win = ones(1) * 4;

	cout << "m1.printout();" << endl;
	m1.printout();
	cout << "win.printout();" << endl;
	win.printout();
	cout << "enframe( m1, win ).printout();" << endl;
	enframe( m1, win ).printout();
	cout << "enframe( m1, win, 2 ).printout();" << endl;
	enframe( m1, win, 2 ).printout();
	cout << "mr0 = rfft( m1, 5, 1 ).printout();" << endl;
	Matrix mr0 = rfft( m1, 5, 1 );
	mr0.printout();
	cout << "mr1 = rfft( m1, 5, 2 ).printout();" << endl;
	Matrix mr1 = rfft( m1, 5, 2 );
	mr1.printout();
	cout << "irfft( mr0, 5, 1 ).printout();" << endl;
	irfft( mr0, 5, 1 ).printout();
	cout << "irfft( mr1, 5, 2 ).printout();" << endl;
	irfft( mr1, 5, 2 ).printout();

	cout << "mr2 = rfft( m1, 4, 1 ).printout();" << endl;
	Matrix mr2 = rfft( m1, 4, 1 );
	mr2.printout();
	cout << "mr3 = rfft( m1, 4, 2 ).printout();" << endl;
	Matrix mr3 = rfft( m1, 4, 2 );
	mr3.printout();
	cout << "irfft( mr2, 4, 1 ).printout();" << endl;
	irfft( mr2, 4, 1 ).printout();
	cout << "irfft( mr3, 4, 2 ).printout();" << endl;
	irfft( mr3, 4, 2 ).printout();
	cout << "irfft( mr2, 3, 1 ).printout();" << endl;
	irfft( mr2, 3, 1 ).printout();
	cout << "irfft( mr3, 5, 2 ).printout();" << endl;
	irfft( mr3, 5, 2 ).printout();

	return 0;
}
