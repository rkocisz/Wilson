#ifndef STRUCTS_H
#define STRUCTS_H

#include "enums.h"
#include <cinttypes>
#include <vector>

struct Move
{
	uint64_t startPos;
	uint64_t endPos;
	PieceType moved;
	PieceType captured;
	PieceType promotion;
	MoveType moveType;

	bool operator==(const Move&) const = default; 

	Move(uint64_t startPos, uint64_t endPos, PieceType moved, PieceType captured, PieceType promotion, MoveType moveType)
	: startPos(startPos)
	, endPos(endPos)
	, moved(moved)
	, captured(captured)
	, promotion(promotion)
	, moveType(moveType)
	{ 
	}

	Move()
	: startPos(-1)
	, endPos(-1)
	, moved(PieceType::empty)
	, captured(PieceType::empty)
	, promotion(PieceType::empty)
	, moveType(MoveType::normal)
	{
	}
};

struct UnmakeInfo 
{
	uint8_t prevCastlingRights;
	int prevEnPassantSquare;
	int prevHalfmoveClock;
	uint64_t prevZobristKey;

	bool operator==(const UnmakeInfo&) const = default;
};

struct MagicInfo
{
	uint64_t relevantMask;
	uint64_t magic;
	int relevantBits;
	int shift;
	std::vector<uint64_t> occupancyVariations;
	bool operator==(const MagicInfo&) const = default;
};

struct TTEntry
{
	Move bestMove;
	int depth;
	int eval;

	TTEntry(Move bestMove, int depth, int eval)
	:bestMove(bestMove)
	,depth(depth)
	,eval(eval)
	{
	}

	TTEntry()
	{
		bestMove = Move();
		depth = -1;
		eval = -1;
	}
};

#endif 
