#ifndef MWSSHOBJ_H
#define MWSSHOBJ_H

#include <string>

using namespace std;

namespace mws
{

	class mwsShObj
	{
		public:
			mwsShObj( string ShObjName );
			~mwsShObj();
			void * get( string functinName);

		private:
			void * dlHandle;
	};

}

#endif
