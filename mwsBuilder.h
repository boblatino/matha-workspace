#ifndef MWSBUILDER_H
#define MWSBUILDER_H

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
