#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <string>
#include <stack>
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
	GameState getGamestate();
	uint64_t getZobristKey();
	uint64_t getNewlyGeneratedZobristKey();
	uint64_t getAllPiecesOccupancy();
	uint64_t getWhitePiecesOccupancy();
	uint64_t getBlackPiecesOccupancy();
	
	

private:
	void initZobrist();
	uint64_t computeZobrist();
	bool isPositionRepeatedThrice();

	PieceType board_[64];
	uint64_t bitBoards_[12];
	uint64_t whitePieces_;
	uint64_t blackPieces_;
	uint64_t allPieces_;

	uint8_t castlingRights_;
	int enPassantSquare_;
	Color sideToMove_;
	int halfmoveClock_;

	std::vector<uint64_t> positionHistory_;
	std::stack<UnmakeInfo> unmakeStack_;

	GameState gameState_;


	//Zobrist
	uint64_t zobristPiece_[12][64];
	uint64_t zobristSide_;
	uint64_t zobristCastling_[16];
	uint64_t zobristEnPassant_[8];

	uint64_t zobristKey_;
};

#endif