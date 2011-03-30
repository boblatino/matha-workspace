#ifndef MWSBUILDER_H
#define MWSBUILDER_H

#include "mwsWorkspace.h"
#include "mwsWSO.h"

using namespace std;

namespace mws
{
	class mwsBuilder
	{
		public:
			virtual void rebuildWS( mwsWorkspace * )=0;
			virtual void rebuildWSO( mwsWSO * )=0;
			virtual mwsWorkspace * buildAll()=0;
	};
}

#endif
