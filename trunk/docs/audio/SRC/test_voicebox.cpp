#include "voicebox.h"
#include "matrices.h"
#include <iostream>

using namespace std;

int main()
{
	double x[ 25 ] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24};

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

	Matrix xo, xs;
	estnoisem_out_stat zo;
	init_estnoisem_out_stat( zo );
	cout << "estnoisem( m1, 3, NULL, &xo, &zo, &xs );" << endl;
	estnoisem( m1, 3, NULL, &xo, &zo, &xs );
	xo.printout();
	cout << "xs" << endl;
	xs.printout();

	cout << "zo:" << endl;
	cout << endl << "double nrcum;";
	cout << zo.nrcum;
	cout << endl <<	"Matrix p;";
	zo.p.printout();
	cout << endl <<	"Matrix ac;";
	zo.ac.printout();
	cout << endl <<	"Matrix sn2;";
	zo.sn2.printout();
	cout << endl <<	"Matrix pb;";
	zo.pb.printout();
	cout << endl <<	"Matrix pb2;";
	zo.pb2.printout();
	cout << endl <<	"Matrix pminu;";
	zo.pminu.printout();
	cout << endl <<	"Matrix actmin;";
	zo.actmin.printout();
	cout << endl <<	"Matrix actminsub;";
	zo.actminsub.printout();
	cout << endl <<	"double subwc;";
	cout << zo.subwc;
	cout << endl <<	"Matrix actbuf;";
	zo.actbuf.printout();
	cout << endl <<	"double ibuf;";
	cout << zo.ibuf;
	cout << endl <<	"Matrix lminflag;";
	zo.lminflag.printout();
	cout << endl <<	"double tinc;";
	cout << zo.tinc;
	cout << endl <<	"estnoisem_alg_param qq;";
//	estnoisem_alg_param qq;

	cout << "estnoisem( m1, zo, NULL, &xo, &zo, &xs );" << endl;
	estnoisem( m1, &zo, NULL, &xo, &zo, &xs );
	xo.printout();
	cout << "xs" << endl;
	xs.printout();

	cout << "zo:" << endl;
	cout << endl << "double nrcum;";
	cout << zo.nrcum;
	cout << endl <<	"Matrix p;";
	zo.p.printout();
	cout << endl <<	"Matrix ac;";
	zo.ac.printout();
	cout << endl <<	"Matrix sn2;";
	zo.sn2.printout();
	cout << endl <<	"Matrix pb;";
	zo.pb.printout();
	cout << endl <<	"Matrix pb2;";
	zo.pb2.printout();
	cout << endl <<	"Matrix pminu;";
	zo.pminu.printout();
	cout << endl <<	"Matrix actmin;";
	zo.actmin.printout();
	cout << endl <<	"Matrix actminsub;";
	zo.actminsub.printout();
	cout << endl <<	"double subwc;";
	cout << zo.subwc;
	cout << endl <<	"Matrix actbuf;";
	zo.actbuf.printout();
	cout << endl <<	"double ibuf;";
	cout << zo.ibuf;
	cout << endl <<	"Matrix lminflag;";
	zo.lminflag.printout();
	cout << endl <<	"double tinc;";
	cout << zo.tinc;
	cout << endl <<	"estnoisem_alg_param qq;";
//	estnoisem_alg_param qq;
	cout << endl;

	specsub_out_stat z;
	init_specsub_out_stat( z );
	cout << "specsub( m1, 100, NULL, &xo, &z )" << endl;
	specsub( m1, 100, NULL, &xo, &z );
	xo.printout();

	return 0;
}
