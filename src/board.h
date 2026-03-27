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
	/*inline const uint64_t& getAllPiecesOccupancy();
	inline const uint64_t& getWhitePiecesOccupancy();
	inline const uint64_t& getBlackPiecesOccupancy();
	inline const std::array<uint64_t, 12>& getBitBoards();
	inline const Color& getSideToMove();
	inline const std::array<PieceType, 64>& getBoard();
	inline const uint8_t& getCastlingRights();
	inline const int& getEnPassantSquare();*/
	
	bool isPositionRepeatedThrice();

	std::array<PieceType, 64> boardArray_;
	std::array<uint64_t, 12> bitBoards_;
	uint64_t whitePieces_;
	uint64_t blackPieces_;
	uint64_t allPieces_;

	uint8_t castlingRights_;
	int enPassantSquare_;
	Color sideToMove_;

	GameState gameState_;

private:
	void initZobrist();
	uint64_t computeZobrist();

	int halfmoveClock_;
	std::vector<uint64_t> positionHistory_;
	std::stack<UnmakeInfo> unmakeStack_;


	//Zobrist
	uint64_t zobristPiece_[12][64];
	uint64_t zobristSide_;
	uint64_t zobristCastling_[16];
	uint64_t zobristEnPassant_[8];

	uint64_t zobristKey_;
};

#endif