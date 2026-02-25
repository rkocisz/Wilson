#include "util.h"
#include <bit>

namespace Util
{
	namespace
	{
		std::mt19937_64 rng(123456);
	}
	
	uint64_t randomU64() 
	{ 
		return rng();
	}

	inline int popLSB(uint64_t& x)
	{
		int bitPos = std::countr_zero(x);
		x &= x - 1;
		return bitPos;
	}
}


