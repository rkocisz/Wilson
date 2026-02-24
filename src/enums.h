#ifndef ENUMS_H 
#define ENUMS_H

enum PieceType
{
	whiteKing,
	whiteQueen,
	whiteRook,
	whiteBishop,
	whiteKnight,
	whitePawn,
	blackKing,
	blackQueen,
	blackRook,
	blackBishop,
	blackKnight,
	blackPawn,
	empty
};

enum MoveType
{
	normal,
	shortCastle,
	longCastle,
	enPassant,
};

enum CastlingRights 
{
	whiteKingSide = 1,
	whiteQueenSide = 2,
	blackKingSide = 4,
	blackQueenSide = 8
};

enum Color
{
	white,
	black
};

enum GameState
{
	playing,
	draw,
	winWhite,
	winBlack
};


#endif
