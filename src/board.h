#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <array>
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

	const GameState& getGamestate();
	const uint64_t& getZobristKey();
	const uint64_t& getNewlyGeneratedZobristKey();
	const uint64_t& getAllPiecesOccupancy();
	const uint64_t& getWhitePiecesOccupancy();
	const uint64_t& getBlackPiecesOccupancy();
	const std::array<uint64_t, 12>& getBitBoards();
	const Color& getSideToMove();
	const std::array<PieceType, 64>& getBoard();
	const uint8_t& getCastlingRights();
	const int& getEnPassantSquare();
	
	

private:
	void initZobrist();
	uint64_t computeZobrist();
	bool isPositionRepeatedThrice();

	std::array<PieceType, 64> board_;
	std::array<uint64_t, 12> bitBoards_;
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