#include <mwsShObj.h>
#include <string>
#include <dlfcn.h>

using namespace std;

namespace mws
{

	mwsShObj::mwsShObj( string ShObjName )
	{
		dlHandle = dlopen( ShObjName.c_str(), RTLD_LAZY );
	}

	mwsShObj::~mwsShObj()
	{
		dlclose( dlHandle );
	}

	void * mwsShObj::get( string functionName )
	{
		return dlsym( dlHandle, functionName.c_str() );
	}

}
