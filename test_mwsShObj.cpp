#include "mwsShObj.h"

//for  debugging
#include <iostream>

using namespace std;
using namespace mws;

typedef void (* typefun1)( void );
typedef void (* typefun2)( string );

//typefun1 fun1;
void (* fun1)();
typefun2 fun2;

int main()
{
	mws::mwsShObj * myShObj = new mws::mwsShObj( "./libtest_ShObj.so" );
	fun1 = (void (*)()) myShObj->get( "function1" );
	fun2 = (typefun2) myShObj->get( "function2" );
	fun1();
	char text[10] = "My text";
	fun2( text );
	delete myShObj;
	return 0;
}
