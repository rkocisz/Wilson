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
	WhiteKingSide = 1,
	WhiteQueenSide = 2,
	BlackKingSide = 4,
	BlackQueenSide = 8
};


#endif
