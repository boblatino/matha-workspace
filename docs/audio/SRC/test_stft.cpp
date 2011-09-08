#include <iostream>
#include "matrices.h"
#include "stft.h"
#include "istft.h"

using namespace std;

int main()
{
	Matrix x = seq( 0, 1027 );
	Matrix y = stft( x );
	cout << "STFT:" << endl;
	y.printout();
	cout << "ISTFT:" << endl;
	istft( y ).printout();
	return 0;
}
