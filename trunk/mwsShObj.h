#ifndef MWSSHOBJ_H
#define MWSSHOBJ_H

#include <string>
#include <dlfcn.h>

using namespace std;

namespace mws
{

	class mwsShObj
	{
		public:
			inline mwsShObj( string ShObjName );
			inline ~mwsShObj();
			inline void * get( string functinName);

		private:
			void * dlHandle;
	};


	inline mwsShObj::mwsShObj( string ShObjName )
	{
		dlHandle = dlopen( ShObjName.c_str(), RTLD_LAZY ) ;
	}

	inline mwsShObj::~mwsShObj()
	{
		dlclose( dlHandle );
	}

	inline void * mwsShObj::get( string functionName )
	{
		return dlsym( dlHandle, functionName.c_str() );
	}


}

#endif
