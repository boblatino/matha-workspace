#include <cstdio>

using namespace std;
#ifdef __cplusplus
extern "C"
{
#endif


void function1()
{
	puts("In function 1\n");
}


void function2( char text[25] )
{
	printf("In function 2 with text : %s\n", text);
}


#ifdef __cplusplus
}
#endif
