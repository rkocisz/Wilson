#include "util.h"
#include "common.h"
#include "structs.h"
#include "board.h"
#include "moveGen.h"

#include <iostream>
#include <bit>
#include <set>
#include <bitset>

namespace Util
{
	const char* piecesEmotes_[12] = 
	{
			"\xE2\x99\x9A", "\xE2\x99\x9B", "\xE2\x99\x9C", "\xE2\x99\x9D", "\xE2\x99\x9E", "\xE2\x99\x9F",
			"\xE2\x99\x94", "\xE2\x99\x95", "\xE2\x99\x96", "\xE2\x99\x97", "\xE2\x99\x98", "\xE2\x99\x99"
	};

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

	std::string squareToNotation(int sq) 
	{
		int fileNum = sq % 8;
		int rankNum = sq / 8;

		char file = 'a' + fileNum;
		char rank = '8' - rankNum;

		std::string s = "";
		s += file;
		s += rank;
		return s;
	}

	std::string moveToLAN(Move& move)
	{
		std::string s = squareToNotation(move.startPos) + squareToNotation(move.endPos);

		if (move.promotion != PieceType::empty) 
		{
			if (move.promotion == PieceType::whiteQueen || move.promotion == PieceType::blackQueen) s += "q";
			else if (move.promotion == PieceType::whiteRook || move.promotion == PieceType::blackRook) s += "r";
			else if (move.promotion == PieceType::whiteBishop || move.promotion == PieceType::blackBishop) s += "b";
			else if (move.promotion == PieceType::whiteKnight || move.promotion == PieceType::blackKnight) s += "n";
		}

		return s;
	}

	Move LANToMove(std::string moveStr, Board& board)
	{
		std::vector<Move> moves = MoveGen::generateLegalMoves(&board);
		
		for (int i = 0; i < moves.size(); i++)
		{
			if (moveToLAN(moves[i]) == moveStr)
			{
				return moves[i];
			}
		}
	
		Move move;
		return move;
	}
}


