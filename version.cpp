#include "version.h"

#define LUX_CORE_MAJOR_VERSION_NUM    0
#define LUX_CORE_MINOR_VERSION_NUM    1

unsigned int get_major_version()
{
	return LUX_CORE_MAJOR_VERSION_NUM;
}

unsigned int get_minor_version()
{
	return LUX_CORE_MINOR_VERSION_NUM;
}