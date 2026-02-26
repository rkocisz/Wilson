#ifndef UTIL_H
#define UTIL_H

#include <random>
#include <cinttypes>

namespace Util
{
	uint64_t randomU64();
    int popLSB(uint64_t& bb);
}


#endif
