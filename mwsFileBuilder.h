#ifndef MWSFILEBUILDER_H
#define MWSFILEBUILDER_H

#include "mwsBuilder.h"
#include "mwsConfigReader.h"
#include <mwsWorkspace.h>

#define	WS_DEFAULT_LENGTH	20
#define	WS_DEFAULT_WIDTH	20
#define	WS_DEFAULT_RESOLUTION	1

using namespace std;

namespace mws
{
    class mwsFileBuilder : public mwsBuilder
    {
      public:
        mwsFileBuilder( string configFileName );
        ~mwsFileBuilder();
        mwsWorkspace *buildAll();
        void rebuildWS( mwsWorkspace * ws );
        void rebuildWSO( mwsWSO * wso );
      private:
        mwsConfigReader * configReader;
    };
}
#endif
