#include <mwsFileBuilder.h>
#include <iostream>

using namespace mws;

#define DEFAULT_WS_NAME "samplews.conf"

int main(int argc, char **argv)
{
    char *filename;
    if (argc < 2) {
        filename = DEFAULT_WS_NAME;
    }
    mwsBuilder *bldr = new mwsFileBuilder (filename);

    mwsWorkspace *ws = bldr->buildAll();

    ws->go();
    
    return 0;
}
