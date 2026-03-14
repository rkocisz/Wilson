#include "moveGenUtil.h"
#include "structs.h"
#include "util.h"
#include "common.h"

namespace MoveGenUtil
{
	uint64_t knightMoves_[64];
	uint64_t kingMoves_[64];

	namespace
	{
		MagicInfo bishopMagic_[64];
		MagicInfo rookMagic_[64];
		uint64_t precomputedBishopMoves_[64][512];
		uint64_t precomputedRookMoves_[64][4096];
		uint64_t bishopMoves_[64][512];
		uint64_t rookMoves_[64][4096];


		void initKnightMoves()
		{
			for (int sq = 0; sq < 64; sq++)
			{
				uint64_t moves = 0ULL;
				int rank = sq / 8;
				int file = sq % 8;
				if (rank + 2 < 8)
				{
					if (file + 1 < 8) moves |= Util::squareMask(sq + 17);
					if (file - 1 >= 0) moves |= Util::squareMask(sq + 15);
				}
				if (rank - 2 >= 0)
				{
					if (file + 1 < 8) moves |= Util::squareMask(sq - 15);
					if (file - 1 >= 0) moves |= Util::squareMask(sq - 17);
				}
				if (file + 2 < 8)
				{
					if (rank + 1 < 8) moves |= Util::squareMask(sq + 10);
					if (rank - 1 >= 0) moves |= Util::squareMask(sq - 6);
				}
				if (file - 2 >= 0)
				{
					if (rank + 1 < 8) moves |= Util::squareMask(sq + 6);
					if (rank - 1 >= 0) moves |= Util::squareMask(sq - 10);
				}
				knightMoves_[sq] = moves;
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
					moves |= Util::squareMask(sq + 8);
					if (file + 1 < 8) moves |= Util::squareMask(sq + 9);
					if (file - 1 >= 0) moves |= Util::squareMask(sq + 7);
				}
				if (rank - 1 >= 0)
				{
					moves |= Util::squareMask(sq - 8);
					if (file + 1 < 8) moves |= Util::squareMask(sq - 7);
					if (file - 1 >= 0) moves |= Util::squareMask(sq - 9);
				}
				if (file + 1 < 8)
				{
					moves |= Util::squareMask(sq + 1);
				}
				if (file - 1 >= 0)
				{
					moves |= Util::squareMask(sq - 1);
				}
				kingMoves_[sq] = moves;
			}
		}


		uint64_t computeRookRelevantOccupancy(int square)
		{
			uint64_t mask = 0ULL;

			int rank = square / 8;
			int file = square % 8;

			for (int r = rank + 1; r <= 6; r++)
				mask |= (Util::squareMask(r * 8 + file));

			for (int r = rank - 1; r >= 1; r--)
				mask |= (Util::squareMask(r * 8 + file));

			for (int f = file + 1; f <= 6; f++)
				mask |= (Util::squareMask(rank * 8 + f));

			for (int f = file - 1; f >= 1; f--)
				mask |= (Util::squareMask(rank * 8 + f));

			return mask;
		}


		uint64_t generateBishopOccupancyFromIndex(int square, int index)
		{
			uint64_t startingMask = bishopMagic_[square].relevantMask;
			uint64_t occupancyMask = 0ULL;

			int bitPos = 0;
			int currentSquare = 0;

			while (startingMask)
			{
				currentSquare = 63 - Util::popLSB(startingMask);

				if (index & (1 << bitPos))
				{
					occupancyMask |= Util::squareMask(currentSquare);
				}

				bitPos++;
			}
			return occupancyMask;
		}


		uint64_t generateRookOccupancyFromIndex(int square, int index)
		{
			uint64_t startingMask = rookMagic_[square].relevantMask;
			uint64_t occupancyMask = 0ULL;

			int bitPos = 0;
			int currentSquare = 0;

			while (startingMask)
			{
				currentSquare = 63 - Util::popLSB(startingMask);

				if (index & (1 << bitPos))
				{
					occupancyMask |= Util::squareMask(currentSquare);
				}

				bitPos++;
			}
			return occupancyMask;
		}


		bool testMagicNumber(int square, uint64_t magicNumber, bool isBishop)
		{
			int relevantBits = 0;
			int shift = 0;

			if (isBishop)
			{
				relevantBits = bishopMagic_[square].relevantBits;
				shift = bishopMagic_[square].shift;
			}
			else
			{
				relevantBits = rookMagic_[square].relevantBits;
				shift = rookMagic_[square].shift;
			}

			int tableSize = 1 << relevantBits;

			std::vector<uint64_t> used(tableSize, -1ULL);

			for (int i = 0; i < tableSize; i++)
			{
				uint64_t occupancy = isBishop
					? bishopMagic_[square].occupancyVariations[i]
					: rookMagic_[square].occupancyVariations[i];

				uint64_t moves = isBishop
					? precomputedBishopMoves_[square][i]
					: precomputedRookMoves_[square][i];

				int index = (occupancy * magicNumber) >> shift;

				if (used[index] == -1ULL)
				{
					used[index] = moves;
				}
				else if (used[index] != moves)
				{
					return false;
				}
			}

			return true;
		}


		void initMagicBitboards()
		{
			for (int i = 0; i < 64; i++)
			{
				bishopMagic_[i].relevantMask = computeBishopMoves(i, 0ULL) & ~FILE_H & ~FILE_A & ~RANK_1 & ~RANK_8;
				rookMagic_[i].relevantMask = computeRookRelevantOccupancy(i);

				bishopMagic_[i].relevantBits = Util::bitCount(bishopMagic_[i].relevantMask);
				rookMagic_[i].relevantBits = Util::bitCount(rookMagic_[i].relevantMask);

				bishopMagic_[i].shift = 64 - bishopMagic_[i].relevantBits;
				rookMagic_[i].shift = 64 - rookMagic_[i].relevantBits;

				for (int j = 0; j < (1ULL << bishopMagic_[i].relevantBits); j++)
				{
					bishopMagic_[i].occupancyVariations.push_back(generateBishopOccupancyFromIndex(i, j));
					precomputedBishopMoves_[i][j] = computeBishopMoves(i, bishopMagic_[i].occupancyVariations[j]);
				}

				for (int j = 0; j < (1ULL << rookMagic_[i].relevantBits); j++)
				{
					rookMagic_[i].occupancyVariations.push_back(generateRookOccupancyFromIndex(i, j));
					precomputedRookMoves_[i][j] = computeRookMoves(i, rookMagic_[i].occupancyVariations[j]);
				}

				while (true)
				{
					uint64_t randomMagicNum = Util::randomU64() & Util::randomU64() & Util::randomU64();

					if (testMagicNumber(i, randomMagicNum, true))
					{
						bishopMagic_[i].magic = randomMagicNum;
						break;
					}
				}

				while (true)
				{
					uint64_t randomMagicNum = Util::randomU64() & Util::randomU64() & Util::randomU64();

					if (testMagicNumber(i, randomMagicNum, false))
					{
						rookMagic_[i].magic = randomMagicNum;
						break;
					}
				}

				uint64_t index = 0;

				for (int j = 0; j < (1ULL << bishopMagic_[i].relevantBits); j++)
				{
					index = bishopMagic_[i].occupancyVariations[j];
					index *= bishopMagic_[i].magic;
					index >>= bishopMagic_[i].shift;
					bishopMoves_[i][index] = precomputedBishopMoves_[i][j];
				}

				for (int j = 0; j < (1ULL << rookMagic_[i].relevantBits); j++)
				{
					index = rookMagic_[i].occupancyVariations[j];
					index *= rookMagic_[i].magic;
					index >>= rookMagic_[i].shift;
					rookMoves_[i][index] = precomputedRookMoves_[i][j];
				}
			}
		}
	}


	void init()
	{
		initKingMoves();
		initKnightMoves();
		initMagicBitboards();
	}


	uint64_t computeBishopMoves(int square, uint64_t occupancy)
	{
		uint64_t moves = 0ULL;
		int rank = square / 8;
		int file = square % 8;
		for (int r = rank + 1, f = file + 1; r < 8 && f < 8; r++, f++)
		{
			moves |= Util::squareMask(r * 8 + f);
			if (occupancy & Util::squareMask(r * 8 + f))
				break;
		}
		for (int r = rank + 1, f = file - 1; r < 8 && f >= 0; r++, f--)
		{
			moves |= Util::squareMask(r * 8 + f);
			if (occupancy & Util::squareMask(r * 8 + f))
				break;
		}
		for (int r = rank - 1, f = file + 1; r >= 0 && f < 8; r--, f++)
		{
			moves |= Util::squareMask(r * 8 + f);
			if (occupancy & Util::squareMask(r * 8 + f))
				break;
		}
		for (int r = rank - 1, f = file - 1; r >= 0 && f >= 0; r--, f--)
		{
			moves |= Util::squareMask(r * 8 + f);
			if (occupancy & Util::squareMask(r * 8 + f))
				break;
		}
		return moves;
	}


	uint64_t computeRookMoves(int square, uint64_t occupancy)
	{
		uint64_t moves = 0ULL;
		int rank = square / 8;
		int file = square % 8;

		for (int r = rank + 1; r < 8; r++)
		{
			moves |= Util::squareMask(r * 8 + file);
			if (occupancy & Util::squareMask(r * 8 + file))
				break;
		}
		for (int r = rank - 1; r >= 0; r--)
		{
			moves |= Util::squareMask(r * 8 + file);
			if (occupancy & Util::squareMask(r * 8 + file))
				break;
		}
		for (int f = file + 1; f < 8; f++)
		{
			moves |= Util::squareMask(rank * 8 + f);
			if (occupancy & Util::squareMask(rank * 8 + f))
				break;
		}
		for (int f = file - 1; f >= 0; f--)
		{
			moves |= Util::squareMask(rank * 8 + f);
			if (occupancy & Util::squareMask(rank * 8 + f))
				break;
		}

		return moves;
	}


	uint64_t getBishopMoves(int square, uint64_t occupancy)
	{
		occupancy &= bishopMagic_[square].relevantMask;
		occupancy *= bishopMagic_[square].magic;
		occupancy >>= bishopMagic_[square].shift;
		return bishopMoves_[square][occupancy];
	}


	uint64_t getRookMoves(int square, uint64_t occupancy)
	{
		occupancy &= rookMagic_[square].relevantMask;
		occupancy *= rookMagic_[square].magic;
		occupancy >>= rookMagic_[square].shift;
		return rookMoves_[square][occupancy];
	}

}