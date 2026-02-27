#include "util.h"
#include <bit>

namespace Util
{
	uint64_t knightAttacks_[64];

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

	void initKnightMoves()
	{

		for (int sq = 0; sq < 64; sq++)
		{
			uint64_t moves = 0ULL;
			int rank = sq / 8;
			int file = sq % 8;
			if (rank + 2 < 8)
			{
				if (file + 1 < 8) moves |= squareMask(sq + 17);
				if (file - 1 >= 0) moves |= squareMask(sq + 15);
			}
			if (rank - 2 >= 0)
			{
				if (file + 1 < 8) moves |= squareMask(sq - 15);
				if (file - 1 >= 0) moves |= squareMask(sq - 17);
			}
			if (file + 2 < 8)
			{
				if (rank + 1 < 8) moves |= squareMask(sq + 10);
				if (rank - 1 >= 0) moves |= squareMask(sq - 6);
			}
			if (file - 2 >= 0)
			{
				if (rank + 1 < 8) moves |= squareMask(sq + 6);
				if (rank - 1 >= 0) moves |= squareMask(sq - 10);
			}
			knightAttacks_[sq] = moves;
		}
	}

	void initKingMoves()
	{
		for (int sq = 0; sq < 64; sq++)
		{
			uint64_t moves = 0ULL;
			int rank = sq / 8;
			int file = sq % 8;
			if (rank + 1 < 8)
			{
				moves |= squareMask(sq + 8);
				if (file + 1 < 8) moves |= squareMask(sq + 9);
				if (file - 1 >= 0) moves |= squareMask(sq + 7);
			}
			if (rank - 1 >= 0)
			{
				moves |= squareMask(sq - 8);
				if (file + 1 < 8) moves |= squareMask(sq - 7);
				if (file - 1 >= 0) moves |= squareMask(sq - 9);
			}
			if (file + 1 < 8)
			{
				moves |= squareMask(sq + 1);
			}
			if (file - 1 >= 0)
			{
				moves |= squareMask(sq - 1);
			}
			kingMoves_[sq] = moves;
		}
	}

	uint64_t squareMask(int square)
	{
		return 1ULL << (63 - square);
	}
}


