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
			void rebuildWS( mwsWorkspace * );
			void rebuildWSO( mwsWSO * );
			mwsWorkspace * buildAll();
	};
}

#endif
