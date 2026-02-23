#include "util.h"

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
}


