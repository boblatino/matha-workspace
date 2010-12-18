#include "mwsShObj.h"

using namespace std;
using namespace mws;

typedef void (* typefun1)( void );
typedef void (* typefun2)( string );

typefun1 fun1;
typefun2 fun2;

int main()
{
	mws::mwsShObj * myShObj = new mws::mwsShObj( "test_ShObj.so" );
	fun1 = (typefun1) myShObj->get( "function1" );
	fun2 = (typefun2) myShObj->get( "function2" );
	fun1();
	fun2( "My text" );
	delete myShObj;
	return 0;
}
