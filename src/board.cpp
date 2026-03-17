#include <iostream>
#include <bitset>
#include <bit>
#include "common.h"
#include "util.h"
#include "board.h"

Board::Board()
: castlingRights_(15)
, enPassantSquare_(-1)
, sideToMove_(Color::white)
, halfmoveClock_(0)
, gameState_(GameState::playing)
{
	for (int i = 0; i < 64; ++i)
		boardArray_[i] = PieceType::empty;

	for (int i = 0; i < 12; ++i)
		bitBoards_[i] = 0ULL;


	loadStartPos();

	initZobrist();
	zobristKey_ = computeZobrist();
	positionHistory_.push_back(zobristKey_);
}

void Board::makeMove(Move move)
{
	unmakeStack_.push(UnmakeInfo(castlingRights_, enPassantSquare_, halfmoveClock_, zobristKey_));

	zobristKey_ ^= zobristSide_;
	sideToMove_ = Util::opposite(sideToMove_);

	//normal moving
	boardArray_[move.startPos] = PieceType::empty;
	boardArray_[move.endPos] = move.moved;

	uint64_t startMask = Util::squareMask(move.startPos);
	uint64_t endMask = Util::squareMask(move.endPos);

	bitBoards_[move.moved] &= ~startMask;
	bitBoards_[move.moved] |= endMask;
	
	if (move.moved < 6)
	{
		whitePieces_ &= ~startMask;
		whitePieces_ |= endMask;
	}
	else
	{
		blackPieces_ &= ~startMask;
		blackPieces_ |= endMask;;
	}

	zobristKey_ ^= zobristPiece_[move.moved][move.startPos];
	zobristKey_ ^= zobristPiece_[move.moved][move.endPos];

	//capturing
	if (move.captured != PieceType::empty)
	{
		uint64_t captureMask = Util::squareMask(move.endPos);
		bitBoards_[move.captured] &= ~captureMask;

		if (move.captured < 6)
			whitePieces_ &= ~captureMask;
		else
			blackPieces_ &= ~captureMask;

		zobristKey_ ^= zobristPiece_[move.captured][move.endPos];
	}

	//promotion
	if (move.promotion != PieceType::empty)
	{
		boardArray_[move.endPos] = move.promotion;
		uint64_t promotionMask = Util::squareMask(move.endPos);
		bitBoards_[move.moved] &= ~promotionMask;
		bitBoards_[move.promotion] |= promotionMask;

		if (move.promotion < 6)
			whitePieces_ |= promotionMask;
		else
			blackPieces_ |= promotionMask;

		zobristKey_ ^= zobristPiece_[move.moved][move.endPos];
		zobristKey_ ^= zobristPiece_[move.promotion][move.endPos];
	}

	//castling rights
	//king moves
	if (move.moved == PieceType::whiteKing)
	{
		castlingRights_ &= ~CastlingRights::whiteKingSide;
		castlingRights_ &= ~CastlingRights::whiteQueenSide;
	}
	else if (move.moved == PieceType::blackKing)
	{
		castlingRights_ &= ~CastlingRights::blackKingSide;
		castlingRights_ &= ~CastlingRights::blackQueenSide;
	}

	//rook moves
	if (move.moved == PieceType::whiteRook)
	{
		if (move.startPos == 63)
			castlingRights_ &= ~whiteKingSide;

		if (move.startPos == 56)
			castlingRights_ &= ~whiteQueenSide;
	}
	else if (move.moved == PieceType::blackRook)
	{
		if (move.startPos == 7)
			castlingRights_ &= ~blackKingSide;

		if (move.startPos == 0)
			castlingRights_ &= ~blackQueenSide;
	}

	//rook captured
	if (move.captured == PieceType::whiteRook)
	{
		if (move.endPos == 63)
			castlingRights_ &= ~whiteKingSide;

		if (move.endPos == 56)
			castlingRights_ &= ~whiteQueenSide;
	}

	if (move.captured == PieceType::blackRook)
	{
		if (move.endPos == 7)
			castlingRights_ &= ~blackKingSide;

		if (move.endPos == 0)
			castlingRights_ &= ~blackQueenSide;
	}

	zobristKey_ ^= zobristCastling_[unmakeStack_.top().prevCastlingRights];
	zobristKey_ ^= zobristCastling_[castlingRights_];

	//enPassantRights
	if (move.moved == PieceType::whitePawn &&
		move.endPos == move.startPos - 16)
	{
		enPassantSquare_ = move.startPos - 8;
	}
	else if (move.moved == PieceType::blackPawn &&
		move.endPos == move.startPos + 16)
	{
		enPassantSquare_ = move.startPos + 8;
	}
	else
	{
		enPassantSquare_ = -1;
	}

	if(unmakeStack_.top().prevEnPassantSquare != -1)
	{
		zobristKey_ ^= zobristEnPassant_[unmakeStack_.top().prevEnPassantSquare % 8];
	}
	if (enPassantSquare_ != -1)
	{
		zobristKey_ ^= zobristEnPassant_[enPassantSquare_ % 8];
	}
	

	//50 move rule
	if (move.moved == PieceType::whitePawn || move.moved == PieceType::blackPawn || move.captured != empty)
	{
		halfmoveClock_ = 0;
	}
	else
	{
		halfmoveClock_++;
	}

	//additional moving to custom moves
	switch(move.moveType)
	{
		case MoveType::shortCastle:
			if (move.moved == PieceType::whiteKing)
			{
				boardArray_[63] = PieceType::empty;
				boardArray_[61] = PieceType::whiteRook;

				startMask = Util::squareMask(63);
				endMask = Util::squareMask(61);
				
				bitBoards_[PieceType::whiteRook] &= ~startMask;
				bitBoards_[PieceType::whiteRook] |= endMask;

				whitePieces_ &= ~startMask;
				whitePieces_ |= endMask;

				zobristKey_ ^= zobristPiece_[PieceType::whiteRook][63];
				zobristKey_ ^= zobristPiece_[PieceType::whiteRook][61];
			}
			else
			{
				boardArray_[7] = PieceType::empty;
				boardArray_[5] = PieceType::blackRook;

				startMask = Util::squareMask(7);
				endMask = Util::squareMask(5);

				bitBoards_[PieceType::blackRook] &= ~startMask;
				bitBoards_[PieceType::blackRook] |= endMask;

				blackPieces_ &= ~startMask;
				blackPieces_ |= endMask;

				zobristKey_ ^= zobristPiece_[PieceType::blackRook][7];
				zobristKey_ ^= zobristPiece_[PieceType::blackRook][5];
			}
			break;

		case MoveType::longCastle:
			if (move.moved == PieceType::whiteKing)
			{
				boardArray_[56] = PieceType::empty;
				boardArray_[59] = PieceType::whiteRook;

				startMask = Util::squareMask(56);
				endMask = Util::squareMask(59);

				bitBoards_[PieceType::whiteRook] &= ~startMask;
				bitBoards_[PieceType::whiteRook] |= endMask;

				whitePieces_ &= ~startMask;
				whitePieces_ |= endMask;

				zobristKey_ ^= zobristPiece_[PieceType::whiteRook][56];
				zobristKey_ ^= zobristPiece_[PieceType::whiteRook][59];
			}
			else
			{
				boardArray_[0] = PieceType::empty;
				boardArray_[3] = PieceType::blackRook;

				startMask = Util::squareMask(0);
				endMask = Util::squareMask(3);

				bitBoards_[PieceType::blackRook] &= ~startMask;
				bitBoards_[PieceType::blackRook] |= endMask;

				blackPieces_ &= ~startMask;
				blackPieces_ |= endMask;

				zobristKey_ ^= zobristPiece_[PieceType::blackRook][0];
				zobristKey_ ^= zobristPiece_[PieceType::blackRook][3];
			}
			break;

		case MoveType::enPassant:
			if (move.moved == PieceType::whitePawn)
			{
				boardArray_[move.endPos + 8] = PieceType::empty;
				endMask = Util::squareMask(move.endPos + 8);
				bitBoards_[PieceType::blackPawn] &= ~endMask;

				blackPieces_ &= ~endMask;

				zobristKey_ ^= zobristPiece_[PieceType::blackPawn][move.endPos + 8];
			}
			else
			{
				boardArray_[move.endPos - 8] = PieceType::empty;
				endMask = Util::squareMask(move.endPos - 8);
				bitBoards_[PieceType::whitePawn] &= ~endMask;

				whitePieces_ &= ~endMask;

				zobristKey_ ^= zobristPiece_[PieceType::whitePawn][move.endPos - 8];
			}
			break;

		default:
			break;
	}

	allPieces_ = whitePieces_ | blackPieces_;

	positionHistory_.push_back(zobristKey_);

	if (isPositionRepeatedThrice())
	{
		gameState_ = GameState::draw;
	}
}

void Board::unmakeMove(Move move)
{
	positionHistory_.pop_back();

	zobristKey_ = unmakeStack_.top().prevZobristKey;

	sideToMove_ = Util::opposite(sideToMove_);
	castlingRights_ = unmakeStack_.top().prevCastlingRights;
	enPassantSquare_ = unmakeStack_.top().prevEnPassantSquare;
	halfmoveClock_ = unmakeStack_.top().prevHalfmoveClock;

	unmakeStack_.pop();

	uint64_t startMask = Util::squareMask(move.startPos);
	uint64_t endMask = Util::squareMask(move.endPos);

	switch (move.moveType)
	{
	case MoveType::shortCastle:
		if (move.moved == PieceType::whiteKing)
		{
			boardArray_[61] = PieceType::empty;
			boardArray_[63] = PieceType::whiteRook;

			bitBoards_[PieceType::whiteRook] &= ~Util::squareMask(61);
			bitBoards_[PieceType::whiteRook] |= Util::squareMask(63);

			whitePieces_ &= ~Util::squareMask(61);
			whitePieces_ |= Util::squareMask(63);
		}
		else
		{
			boardArray_[5] = PieceType::empty;
			boardArray_[7] = PieceType::blackRook;

			bitBoards_[PieceType::blackRook] &= ~Util::squareMask(5);
			bitBoards_[PieceType::blackRook] |= Util::squareMask(7);

			blackPieces_ &= ~Util::squareMask(5);
			blackPieces_ |= Util::squareMask(7);
		}
		break;

	case MoveType::longCastle:
		if (move.moved == PieceType::whiteKing)
		{
			boardArray_[59] = PieceType::empty;
			boardArray_[56] = PieceType::whiteRook;

			bitBoards_[PieceType::whiteRook] &= ~Util::squareMask(59);
			bitBoards_[PieceType::whiteRook] |= Util::squareMask(56);

			whitePieces_ &= ~Util::squareMask(59);
			whitePieces_ |= Util::squareMask(56);
		}
		else
		{
			boardArray_[3] = PieceType::empty;
			boardArray_[0] = PieceType::blackRook;

			bitBoards_[PieceType::blackRook] &= ~Util::squareMask(3);
			bitBoards_[PieceType::blackRook] |= Util::squareMask(0);

			blackPieces_ &= ~Util::squareMask(3);
			blackPieces_ |= Util::squareMask(0);
		}
		break;

	case MoveType::enPassant:
		if (move.moved == PieceType::whitePawn)
		{
			boardArray_[move.endPos + 8] = PieceType::blackPawn;
			bitBoards_[PieceType::blackPawn] |= Util::squareMask(move.endPos + 8);
			blackPieces_ |= Util::squareMask(move.endPos + 8);
		}
		else
		{
			boardArray_[move.endPos - 8] = PieceType::whitePawn;
			bitBoards_[PieceType::whitePawn] |= Util::squareMask(move.endPos - 8);
			whitePieces_ |= Util::squareMask(move.endPos - 8);
		}
		break;

	default:
		break;
	}

	if (move.promotion != PieceType::empty)
	{
		bitBoards_[move.promotion] &= ~endMask;
		
		bitBoards_[move.moved] |= endMask;
	}

	boardArray_[move.startPos] = move.moved;
	boardArray_[move.endPos] = move.captured;

	bitBoards_[move.moved] &= ~endMask;
	bitBoards_[move.moved] |= startMask;

	if (move.moved < 6)
	{
		whitePieces_ &= ~endMask;
		whitePieces_ |= startMask;
	}
	else
	{
		blackPieces_ &= ~endMask;
		blackPieces_ |= startMask;
	}

	if (move.captured != PieceType::empty)
	{
		bitBoards_[move.captured] |= endMask;

		if (move.captured < 6)
			whitePieces_ |= endMask;
		else
			blackPieces_ |= endMask;
	}

	allPieces_ = whitePieces_ | blackPieces_;
}

void Board::loadStartPos()
{
	for (int i = 0; i < 64; ++i)
		boardArray_[i] = PieceType::empty;

	for (int i = 0; i < 12; ++i)
		bitBoards_[i] = 0ULL;

	whitePieces_ = 0ULL;
	blackPieces_ = 0ULL;
	allPieces_ = 0ULL;

	for (int i = 8; i < 16; ++i)
	{
		boardArray_[i] = PieceType::blackPawn;
		bitBoards_[PieceType::blackPawn] |= Util::squareMask(i);
	}

	boardArray_[0] = boardArray_[7] = PieceType::blackRook;
	boardArray_[1] = boardArray_[6] = PieceType::blackKnight;
	boardArray_[2] = boardArray_[5] = PieceType::blackBishop;
	boardArray_[3] = PieceType::blackQueen;
	boardArray_[4] = PieceType::blackKing;

	bitBoards_[PieceType::blackRook] |= Util::squareMask(0) | Util::squareMask(7);
	bitBoards_[PieceType::blackKnight] |= Util::squareMask(1) | Util::squareMask(6);
	bitBoards_[PieceType::blackBishop] |= Util::squareMask(2) | Util::squareMask(5);
	bitBoards_[PieceType::blackQueen] |= Util::squareMask(3);
	bitBoards_[PieceType::blackKing] |= Util::squareMask(4);


	for (int i = 48; i < 56; ++i)
	{
		boardArray_[i] = PieceType::whitePawn;
		bitBoards_[PieceType::whitePawn] |= Util::squareMask(i);
	}

	boardArray_[56] = boardArray_[63] = PieceType::whiteRook;
	boardArray_[57] = boardArray_[62] = PieceType::whiteKnight;
	boardArray_[58] = boardArray_[61] = PieceType::whiteBishop;
	boardArray_[59] = PieceType::whiteQueen;
	boardArray_[60] = PieceType::whiteKing;

	bitBoards_[PieceType::whiteRook] |= Util::squareMask(56) | Util::squareMask(63);
	bitBoards_[PieceType::whiteKnight] |= Util::squareMask(57) | Util::squareMask(62);
	bitBoards_[PieceType::whiteBishop] |= Util::squareMask(58) | Util::squareMask(61);
	bitBoards_[PieceType::whiteQueen] |= Util::squareMask(59);
	bitBoards_[PieceType::whiteKing] |= Util::squareMask(60);

	for (int i = 0; i < 6; i++)
	{
		whitePieces_|= bitBoards_[i];
		blackPieces_ |= bitBoards_[i + 6];
	}

	allPieces_ = whitePieces_ | blackPieces_;
	
}

void Board::draw()
{
	for (int i = 0; i < 64; i++)
	{
		if (i % 8 == 0)
		{
			std::cout << 8 - i / 8 << " ";
		}

		if (boardArray_[i] == PieceType::empty)
		{
			std::cout << ". ";
		}
		else
		{
			std::cout << Util::piecesEmotes_[boardArray_[i]] << " ";
		}

		if ((i + 1) % 8 == 0)
		{
			std::cout << "\n";
		}
	}

	std::cout << "  ";

	for (int i = 0; i < 8; i++)
	{
		std::cout << char('a' + i) << " ";
	}


	/*std::cout << "\n\n\n";

	for (int i = 0; i < 12; i++)
	{
		std::cout << std::bitset<64>(bitBoards_[i]) << "\n";
	}

	std::cout << "\n" << std::bitset<64>(whitePieces_) << "\n" << std::bitset<64>(blackPieces_) << "\n" << std::bitset<64>(allPieces_) << "\n";*/
}

const GameState& Board::getGamestate()
{
	return gameState_;
}

const uint64_t& Board::getZobristKey()
{
	return zobristKey_;
}

const uint64_t& Board::getNewlyGeneratedZobristKey()
{
	return computeZobrist();
}
//
//inline const uint64_t& Board::getAllPiecesOccupancy()
//{
//	return allPieces_;
//}
//
//inline const uint64_t& Board::getWhitePiecesOccupancy()
//{
//	return whitePieces_;
//}
//
//inline const uint64_t& Board::getBlackPiecesOccupancy()
//{
//	return blackPieces_;
//}
//
//inline const std::array<uint64_t, 12>& Board::getBitBoards()
//{
//	return bitBoards_;
//}
//
//inline const Color& Board::getSideToMove()
//{
//	return sideToMove_;
//}
//
//inline const std::array<PieceType, 64>& Board::getBoard()
//{
//	return boardArray_;
//}
//
//inline const uint8_t& Board::getCastlingRights()
//{
//	return castlingRights_;
//}
//
//inline const int& Board::getEnPassantSquare()
//{
//	return enPassantSquare_;
//}

void Board::initZobrist()
{
	for (int piece = 0; piece < 12; piece++)
	{
		for (int sq = 0; sq < 64; sq++)
		{
			zobristPiece_[piece][sq] = Util::randomU64();
		}
	}
	zobristSide_ = Util::randomU64();

	for (int i = 0; i < 16; i++)
	{
		zobristCastling_[i] = Util::randomU64();
	}

	for (int i = 0; i < 8; i++)
	{
		zobristEnPassant_[i] = Util::randomU64();
	}
}

uint64_t Board::computeZobrist()
{
	uint64_t key = 0;

	for (int sq = 0; sq < 64; sq++)
	{
		PieceType piece = boardArray_[sq];

		if (piece != PieceType::empty)
		{
			key ^= zobristPiece_[piece][sq];
		}
	}

	if (sideToMove_ == black)
		key ^= zobristSide_;

	key ^= zobristCastling_[castlingRights_];

	if (enPassantSquare_ != -1)
	{
		int file = enPassantSquare_ % 8;
		key ^= zobristEnPassant_[file];
	}

	return key;
}

bool Board::isPositionRepeatedThrice()
{
	int count = 0;

	for (int i = positionHistory_.size() - 1; i >= 0; i--)
	{
		if (positionHistory_[i] == zobristKey_)
			count++;

		if (count >= 3)
			return true;

		if (halfmoveClock_ < positionHistory_.size() - i)
			break;
	}

	return false;
}