#include "util.h"
#include "common.h"
#include "structs.h"

#include <iostream>
#include <bit>
#include <set>
#include <bitset>

namespace Util
{
	const char* piecesEmotes_[12] = { "♚", "♛", "♜", "♝", "♞", "♟","♔", "♕", "♖", "♗", "♘", "♙" };

	namespace
	{
		std::mt19937_64 rng(123456);
	}
	
	uint64_t randomU64() 
	{ 
		return rng();
	}

	int popLSB(uint64_t& x)
	{
		int bitPos = std::countr_zero(x);
		x &= x - 1;
		return bitPos;
	}

	int bitCount(uint64_t x)
	{
		int bitCount = 0;
		while (x)
		{
			bitCount++;
			popLSB(x);
		}

		return bitCount;
	}


	uint64_t squareMask(int square)
	{
		return 1ULL << (63 - square);
	}

	Color opposite(Color c)
	{
		return (c == white) ? black : white;
	}
}


