#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <string>
#include <cinttypes>
#include <random>
#include "enums.h"
#include "structs.h"


class Board
{
public:
	bool operator==(const Board&) const = default;

	Board();
	void draw();
	void makeMove(Move move);
	void unmakeMove(Move move);
	void loadStartPos();

private:

	inline uint64_t squareMask(int square);	
	inline Color opposite(Color c);
	void initZobrist();
	uint64_t computeZobrist();

	PieceType board_[64];
	uint64_t bitBoards_[12];

	uint8_t castlingRights_;
	int enPassantSquare_;
	Color sideToMove_;
	int halfmoveClock_;

	//Zobrist
	uint64_t zobristPiece_[12][64];
	uint64_t zobristSide_;
	uint64_t zobristCastling_[16];
	uint64_t zobristEnPassant_[8];

	uint64_t zobristKey_;
};

#endif