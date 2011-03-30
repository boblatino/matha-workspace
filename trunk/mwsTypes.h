#ifndef __MWS_TYPES__
#define __MWS_TYPES__

#include <string>
#include <vector>

typedef std::string mws_string;

typedef enum
{
	mwsCircle = 0,
	mwsSquare = 1
} mwsShapeEnum;

typedef std::vector<int > mws_ldims;
typedef std::vector<int > mws_posv;


#endif
