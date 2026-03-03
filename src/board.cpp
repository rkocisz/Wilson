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
		board_[i] = PieceType::empty;

	for (int i = 0; i < 12; ++i)
		bitBoards_[i] = 0ULL;

	loadStartPos();

	initZobrist();
	zobristKey_ = computeZobrist();
	positionHistory_.push_back(zobristKey_);

	Util::initKnightMoves();
	Util::initKingMoves();
	Util::initMagicBitboards();
}

void Board::makeMove(Move move)
{
	unmakeStack_.push(UnmakeInfo(castlingRights_, enPassantSquare_, halfmoveClock_, zobristKey_));

	zobristKey_ ^= zobristSide_;
	sideToMove_ = Util::opposite(sideToMove_);

	//normal moving
	board_[move.startPos] = PieceType::empty;
	board_[move.endPos] = move.moved;

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
		board_[move.endPos] = move.promotion;
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
				board_[63] = PieceType::empty;
				board_[61] = PieceType::whiteRook;

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
				board_[7] = PieceType::empty;
				board_[5] = PieceType::blackRook;

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
				board_[56] = PieceType::empty;
				board_[59] = PieceType::whiteRook;

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
				board_[0] = PieceType::empty;
				board_[3] = PieceType::blackRook;

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
				board_[move.endPos + 8] = PieceType::empty;
				endMask = Util::squareMask(move.endPos + 8);
				bitBoards_[PieceType::blackPawn] &= ~endMask;

				blackPieces_ &= ~endMask;

				zobristKey_ ^= zobristPiece_[PieceType::blackPawn][move.endPos + 8];
			}
			else
			{
				board_[move.endPos - 8] = PieceType::empty;
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
			board_[61] = PieceType::empty;
			board_[63] = PieceType::whiteRook;

			bitBoards_[PieceType::whiteRook] &= ~Util::squareMask(61);
			bitBoards_[PieceType::whiteRook] |= Util::squareMask(63);

			whitePieces_ &= ~Util::squareMask(61);
			whitePieces_ |= Util::squareMask(63);
		}
		else
		{
			board_[5] = PieceType::empty;
			board_[7] = PieceType::blackRook;

			bitBoards_[PieceType::blackRook] &= ~Util::squareMask(5);
			bitBoards_[PieceType::blackRook] |= Util::squareMask(7);

			blackPieces_ &= ~Util::squareMask(5);
			blackPieces_ |= Util::squareMask(7);
		}
		break;

	case MoveType::longCastle:
		if (move.moved == PieceType::whiteKing)
		{
			board_[59] = PieceType::empty;
			board_[56] = PieceType::whiteRook;

			bitBoards_[PieceType::whiteRook] &= ~Util::squareMask(59);
			bitBoards_[PieceType::whiteRook] |= Util::squareMask(56);

			whitePieces_ &= ~Util::squareMask(59);
			whitePieces_ |= Util::squareMask(56);
		}
		else
		{
			board_[3] = PieceType::empty;
			board_[0] = PieceType::blackRook;

			bitBoards_[PieceType::blackRook] &= ~Util::squareMask(3);
			bitBoards_[PieceType::blackRook] |= Util::squareMask(0);

			blackPieces_ &= ~Util::squareMask(3);
			blackPieces_ |= Util::squareMask(0);
		}
		break;

	case MoveType::enPassant:
		if (move.moved == PieceType::whitePawn)
		{
			board_[move.endPos + 8] = PieceType::blackPawn;
			bitBoards_[PieceType::blackPawn] |= Util::squareMask(move.endPos + 8);
			blackPieces_ |= Util::squareMask(move.endPos + 8);
		}
		else
		{
			board_[move.endPos - 8] = PieceType::whitePawn;
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

	board_[move.startPos] = move.moved;
	board_[move.endPos] = move.captured;

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
		board_[i] = PieceType::empty;

	for (int i = 0; i < 12; ++i)
		bitBoards_[i] = 0ULL;

	whitePieces_ = 0ULL;
	blackPieces_ = 0ULL;
	allPieces_ = 0ULL;

	for (int i = 8; i < 16; ++i)
	{
		board_[i] = PieceType::blackPawn;
		bitBoards_[PieceType::blackPawn] |= Util::squareMask(i);
	}

	board_[0] = board_[7] = PieceType::blackRook;
	board_[1] = board_[6] = PieceType::blackKnight;
	board_[2] = board_[5] = PieceType::blackBishop;
	board_[3] = PieceType::blackQueen;
	board_[4] = PieceType::blackKing;

	bitBoards_[PieceType::blackRook] |= Util::squareMask(0) | Util::squareMask(7);
	bitBoards_[PieceType::blackKnight] |= Util::squareMask(1) | Util::squareMask(6);
	bitBoards_[PieceType::blackBishop] |= Util::squareMask(2) | Util::squareMask(5);
	bitBoards_[PieceType::blackQueen] |= Util::squareMask(3);
	bitBoards_[PieceType::blackKing] |= Util::squareMask(4);


	for (int i = 48; i < 56; ++i)
	{
		board_[i] = PieceType::whitePawn;
		bitBoards_[PieceType::whitePawn] |= Util::squareMask(i);
	}

	board_[56] = board_[63] = PieceType::whiteRook;
	board_[57] = board_[62] = PieceType::whiteKnight;
	board_[58] = board_[61] = PieceType::whiteBishop;
	board_[59] = PieceType::whiteQueen;
	board_[60] = PieceType::whiteKing;

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

		if (board_[i] == PieceType::empty)
		{
			std::cout << ". ";
		}
		else
		{
			std::cout << Util::piecesEmotes_[board_[i]] << " ";
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


	std::cout << "\n\n\n";

	for (int i = 0; i < 12; i++)
	{
		std::cout << std::bitset<64>(bitBoards_[i]) << "\n";
	}

	std::cout << "\n" << std::bitset<64>(whitePieces_) << "\n" << std::bitset<64>(blackPieces_) << "\n" << std::bitset<64>(allPieces_) << "\n";
}

GameState Board::getGamestate()
{
	return gameState_;
}

uint64_t Board::getZobristKey()
{
	return zobristKey_;
}

uint64_t Board::getNewlyGeneratedZobristKey()
{
	return computeZobrist();
}

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
		PieceType piece = board_[sq];

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

void Board::generatePseudoLegalMoves()
{
	pseudoLegalMoves_.clear();

	if (sideToMove_ == Color::white)
	{
		generateWhitePawnMoves();
		generateWhiteKnightMoves();
		generateWhiteBishopMoves();
		generateWhiteRookMoves();
		generateWhiteQueenMoves();
		generateWhiteKingMoves();
	}
	else
	{
		generateBlackPawnMoves();
		generateBlackKnightMoves();
		generateBlackBishopMoves();
		generateBlackRookMoves();
		generateBlackQueenMoves();
		generateBlackKingMoves();
	}
	return ;
}

std::vector<Move> Board::generateLegalMoves()
{
	legalMoves_.clear();
	generatePseudoLegalMoves();

	Color moving = sideToMove_;
	uint64_t kingMask = 0ULL;

	for (const Move& move : pseudoLegalMoves_)
	{
		makeMove(move);

		if (moving == Color::white)
		{
			kingMask = bitBoards_[PieceType::whiteKing];
		}
		else
		{
			kingMask = bitBoards_[PieceType::blackKing];
		}
		
		int kingPos = 63 - Util::popLSB(kingMask);

		if (!isSquareAttacked(kingPos, Util::opposite(moving)))
		{
			legalMoves_.push_back(move);
		}

		unmakeMove(move);

	}
	return legalMoves_;
}

void Board::generateWhitePawnMoves()
{
	uint64_t captureLeft = (bitBoards_[PieceType::whitePawn] << 9) & blackPieces_ & ~FILE_H;
	uint64_t captureRight= (bitBoards_[PieceType::whitePawn] << 7) & blackPieces_ & ~FILE_A;

	uint64_t promoLeft = captureLeft & RANK_8;
	uint64_t promoRight = captureRight & RANK_8;

	uint64_t normalLeft = captureLeft & ~RANK_8;
	uint64_t normalRight = captureRight & ~RANK_8;

	uint64_t singlePush = (bitBoards_[PieceType::whitePawn] << 8) & ~allPieces_;

	uint64_t singlePushPromo = singlePush & RANK_8;
	uint64_t singlePushNonPromo = singlePush & ~RANK_8;

	uint64_t doublePush = ((singlePush & RANK_3) << 8) & ~allPieces_;

	uint64_t enPassantLeft = 0ULL;
	uint64_t enPassantRight = 0ULL;

	if(enPassantSquare_ != -1)
	{
		enPassantLeft = (bitBoards_[PieceType::whitePawn] << 9) & Util::squareMask(enPassantSquare_) & ~FILE_H;
		enPassantRight = (bitBoards_[PieceType::whitePawn] << 7) & Util::squareMask(enPassantSquare_) & ~FILE_A;
	}

	while(enPassantLeft)
	{
		int endPos = 63 - Util::popLSB(enPassantLeft);
		int startPos = endPos + 9;
		pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::whitePawn, PieceType::empty, PieceType::empty, MoveType::enPassant);
	}

	while (enPassantRight)
	{
		int endPos = 63 - Util::popLSB(enPassantRight);
		int startPos = endPos + 7;
		pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::whitePawn, PieceType::empty, PieceType::empty, MoveType::enPassant);
	}

	while (normalLeft)
	{
		int endPos = 63 - Util::popLSB(normalLeft);
		int startPos = endPos + 9;

		pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::whitePawn, board_[endPos], PieceType::empty, MoveType::normal);
	}
	
	while (normalRight)
	{
		int endPos = 63 - Util::popLSB(normalRight);
		int startPos = endPos + 7;

		pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::whitePawn, board_[endPos], PieceType::empty, MoveType::normal);
	}

	while (promoLeft)
	{
		int endPos = 63 - Util::popLSB(promoLeft);
		int startPos = endPos + 9;

		for (int i = 1; i < 5; i++)
		{
			pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::whitePawn, board_[endPos], static_cast<PieceType>(i), MoveType::normal);
		}
	}

	while (promoRight)
	{
		int endPos = 63 - Util::popLSB(promoRight);
		int startPos = endPos + 7;
		for (int i = 1; i < 5; i++)
		{
			pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::whitePawn, board_[endPos], static_cast<PieceType>(i), MoveType::normal);
		}
	}

	while (singlePushPromo)
	{
		int endPos = 63 - Util::popLSB(singlePushPromo);
		int startPos = endPos + 8;
		for (int i = 1; i < 5; i++)
		{
			pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::whitePawn, PieceType::empty, static_cast<PieceType>(i), MoveType::normal);
		}
	}

	while (singlePushNonPromo)
	{
		int endPos = 63 - Util::popLSB(singlePushNonPromo);
		int startPos = endPos + 8;
		pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::whitePawn, PieceType::empty, PieceType::empty, MoveType::normal);
	}

	while(doublePush)
	{
		int endPos = 63 - Util::popLSB(doublePush);
		int startPos = endPos + 16;
		pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::whitePawn, PieceType::empty, PieceType::empty, MoveType::normal);
	}
}

void Board::generateWhiteKnightMoves()
{
	uint64_t knights = bitBoards_[PieceType::whiteKnight];

	while (knights)
	{
		int startPos = 63 - Util::popLSB(knights);
		uint64_t attacks = Util::knightMoves_[startPos] & ~whitePieces_;

		while(attacks)
		{
			int endPos = 63 - Util::popLSB(attacks);
			pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::whiteKnight, board_[endPos], PieceType::empty, MoveType::normal);
		}
	}
}

void Board::generateWhiteBishopMoves()
{
	uint64_t bishops = bitBoards_[PieceType::whiteBishop];

	while (bishops)
	{
		int startPos = 63 - Util::popLSB(bishops);
		uint64_t moves = Util::computeBishopMoves(startPos, allPieces_) & ~whitePieces_;

		while (moves)
		{
			int endPos = 63 - Util::popLSB(moves);
			pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::whiteBishop, board_[endPos], PieceType::empty, MoveType::normal);
		}
	}
}

void Board::generateWhiteRookMoves()
{
	uint64_t rooks = bitBoards_[PieceType::whiteRook];

	while (rooks)
	{
		int startPos = 63 - Util::popLSB(rooks);
		uint64_t moves = Util::computeRookMoves(startPos, allPieces_) & ~whitePieces_;

		while (moves)
		{
			int endPos = 63 - Util::popLSB(moves);
			pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::whiteRook, board_[endPos], PieceType::empty, MoveType::normal);
		}
	}
}

void Board::generateWhiteQueenMoves()
{
	uint64_t queens = bitBoards_[PieceType::whiteQueen];
	while (queens)
	{
		int startPos = 63 - Util::popLSB(queens);
		uint64_t moves = (Util::computeBishopMoves(startPos, allPieces_) | Util::computeRookMoves(startPos, allPieces_)) & ~whitePieces_;
		while (moves)
		{
			int endPos = 63 - Util::popLSB(moves);
			pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::whiteQueen, board_[endPos], PieceType::empty, MoveType::normal);
		}
	}
}

void Board::generateWhiteKingMoves()
{
	int startPos = 63 - std::countr_zero(bitBoards_[PieceType::whiteKing]);
	uint64_t kingMoves = Util::kingMoves_[startPos] & ~whitePieces_;

	while (kingMoves)
	{
		int endPos = 63 - Util::popLSB(kingMoves);
		pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::whiteKing, board_[endPos], PieceType::empty, MoveType::normal);
	}

	if (castlingRights_ & CastlingRights::whiteKingSide && !(allPieces_ & (Util::squareMask(61) | Util::squareMask(62))))
	{
		pseudoLegalMoves_.emplace_back(60, 62, PieceType::whiteKing, PieceType::empty, PieceType::empty, MoveType::shortCastle);
	}
	if (castlingRights_ & CastlingRights::whiteQueenSide && !(allPieces_ & (Util::squareMask(59) | Util::squareMask(58) | Util::squareMask(57))))
	{
		pseudoLegalMoves_.emplace_back(60, 58, PieceType::whiteKing, PieceType::empty, PieceType::empty, MoveType::longCastle);
	}
}

void Board::generateBlackPawnMoves()
{
	uint64_t captureLeft = (bitBoards_[PieceType::blackPawn] >> 7) & whitePieces_ & ~FILE_H;
	uint64_t captureRight = (bitBoards_[PieceType::blackPawn] >> 9) & whitePieces_ & ~FILE_A;

	uint64_t promoLeft = captureLeft & RANK_1;
	uint64_t promoRight = captureRight & RANK_1;

	uint64_t normalLeft = captureLeft & ~RANK_1;
	uint64_t normalRight = captureRight & ~RANK_1;

	uint64_t singlePush = (bitBoards_[PieceType::blackPawn] >> 8) & ~allPieces_;

	uint64_t singlePushPromo = singlePush & RANK_1;
	uint64_t singlePushNonPromo = singlePush & ~RANK_1;

	uint64_t doublePush = ((singlePush & RANK_6) >> 8) & ~allPieces_;

	uint64_t enPassantLeft = 0ULL;
	uint64_t enPassantRight = 0ULL;

	if (enPassantSquare_ != -1)
	{
		enPassantLeft = (bitBoards_[PieceType::blackPawn] >> 7) & Util::squareMask(enPassantSquare_) & ~FILE_H;
		enPassantRight = (bitBoards_[PieceType::blackPawn] >> 9) & Util::squareMask(enPassantSquare_) & ~FILE_A;
	}

	while (enPassantLeft)
	{
		int endPos = 63 - Util::popLSB(enPassantLeft);
		int startPos = endPos - 7;
		pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::blackPawn, PieceType::empty, PieceType::empty, MoveType::enPassant);
	}

	while (enPassantRight)
	{
		int endPos = 63 - Util::popLSB(enPassantRight);
		int startPos = endPos - 9;
		pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::blackPawn, PieceType::empty, PieceType::empty, MoveType::enPassant);
	}

	while (normalLeft)
	{
		int endPos = 63 - Util::popLSB(normalLeft);
		int startPos = endPos - 7;

		pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::blackPawn, board_[endPos], PieceType::empty, MoveType::normal);
	}

	while (normalRight)
	{
		int endPos = 63 - Util::popLSB(normalRight);
		int startPos = endPos - 9;

		pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::blackPawn, board_[endPos], PieceType::empty, MoveType::normal);
	}

	while (promoLeft)
	{
		int endPos = 63 - Util::popLSB(promoLeft);
		int startPos = endPos - 7;

		for (int i = 7; i < 11; i++)
		{
			pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::blackPawn, board_[endPos], static_cast<PieceType>(i), MoveType::normal);
		}
	}

	while (promoRight)
	{
		int endPos = 63 - Util::popLSB(promoRight);
		int startPos = endPos - 9;
		for (int i = 7; i < 11; i++)
		{
			pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::blackPawn, board_[endPos], static_cast<PieceType>(i), MoveType::normal);
		}
	}

	while (singlePushPromo)
	{
		int endPos = 63 - Util::popLSB(singlePushPromo);
		int startPos = endPos - 8;
		for (int i = 7; i < 11; i++)
		{
			pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::blackPawn, PieceType::empty, static_cast<PieceType>(i), MoveType::normal);
		}
	}

	while (singlePushNonPromo)
	{
		int endPos = 63 - Util::popLSB(singlePushNonPromo);
		int startPos = endPos - 8;
		pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::blackPawn, PieceType::empty, PieceType::empty, MoveType::normal);
	}

	while (doublePush)
	{
		int endPos = 63 - Util::popLSB(doublePush);
		int startPos = endPos - 16;
		pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::blackPawn, PieceType::empty, PieceType::empty, MoveType::normal);
	}
}

void Board::generateBlackKnightMoves()
{
	uint64_t knights = bitBoards_[PieceType::blackKnight];

	while (knights)
	{
		int startPos = 63 - Util::popLSB(knights);
		uint64_t attacks = Util::knightMoves_[startPos] & ~blackPieces_;

		while (attacks)
		{
			int endPos = 63 - Util::popLSB(attacks);
			pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::blackKnight, board_[endPos], PieceType::empty, MoveType::normal);
		}
	}
}

void Board::generateBlackBishopMoves()
{
	uint64_t bishops = bitBoards_[PieceType::blackBishop];

	while (bishops)
	{
		int startPos = 63 - Util::popLSB(bishops);
		uint64_t moves = Util::computeBishopMoves(startPos, allPieces_) & ~blackPieces_;

		while (moves)
		{
			int endPos = 63 - Util::popLSB(moves);
			pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::blackBishop, board_[endPos], PieceType::empty, MoveType::normal);
		}
	}
}

void Board::generateBlackRookMoves()
{
	uint64_t rooks = bitBoards_[PieceType::blackRook];

	while (rooks)
	{
		int startPos = 63 - Util::popLSB(rooks);
		uint64_t moves = Util::computeRookMoves(startPos, allPieces_) & ~blackPieces_;

		while (moves)
		{
			int endPos = 63 - Util::popLSB(moves);
			pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::blackRook, board_[endPos], PieceType::empty, MoveType::normal);
		}
	}
}

void Board::generateBlackQueenMoves()
{
	uint64_t queens = bitBoards_[PieceType::blackQueen];
	while (queens)
	{
		int startPos = 63 - Util::popLSB(queens);
		uint64_t moves = (Util::computeBishopMoves(startPos, allPieces_) | Util::computeRookMoves(startPos, allPieces_)) & ~blackPieces_;
		while (moves)
		{
			int endPos = 63 - Util::popLSB(moves);
			pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::blackQueen, board_[endPos], PieceType::empty, MoveType::normal);
		}
	}
}

void Board::generateBlackKingMoves()
{
	int startPos = 63 - std::countr_zero(bitBoards_[PieceType::blackKing]);
	uint64_t kingMoves = Util::kingMoves_[startPos] & ~blackPieces_;

	while (kingMoves)
	{
		int endPos = 63 - Util::popLSB(kingMoves);
		pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::blackKing, board_[endPos], PieceType::empty, MoveType::normal);
	}

	if (castlingRights_ & CastlingRights::blackKingSide && !(allPieces_ & (Util::squareMask(5) | Util::squareMask(6))))
	{
		pseudoLegalMoves_.emplace_back(4, 6, PieceType::blackKing, PieceType::empty, PieceType::empty, MoveType::shortCastle);
	}
	if (castlingRights_ & CastlingRights::blackQueenSide && !(allPieces_ & (Util::squareMask(1) | Util::squareMask(2) | Util::squareMask(3))))
	{
		pseudoLegalMoves_.emplace_back(4, 2, PieceType::blackKing, PieceType::empty, PieceType::empty, MoveType::longCastle);
	}
}

bool Board::isSquareAttacked(int square, Color attacker)
{
	uint64_t squareMask = Util::squareMask(square);

	if (attacker == Color::white)
	{
		uint64_t attackers = ((squareMask >> 7) & ~FILE_H) | ((squareMask >> 9) & ~FILE_A);
		if(attackers & bitBoards_[PieceType::whitePawn])
			return true;

		if (Util::knightMoves_[square] & bitBoards_[PieceType::whiteKnight])
			return true;


		if (Util::kingMoves_[square] & bitBoards_[PieceType::whiteKing])
			return true;

		if (Util::computeBishopMoves(square, allPieces_) & (bitBoards_[PieceType::whiteBishop] | bitBoards_[PieceType::whiteQueen]))
			return true;

		if(Util::computeRookMoves(square, allPieces_) & (bitBoards_[PieceType::whiteRook] | bitBoards_[PieceType::whiteQueen]))
			return true;

	}
	else
	{
		uint64_t attackers = ((squareMask << 7) & ~FILE_A) | ((squareMask << 9) & ~FILE_H);
		if (attackers & bitBoards_[PieceType::blackPawn])
			return true;

		if (Util::knightMoves_[square] & bitBoards_[PieceType::blackKnight])
			return true;

		if (Util::kingMoves_[square] & bitBoards_[PieceType::blackKing])
			return true;

		if (Util::computeBishopMoves(square, allPieces_) & (bitBoards_[PieceType::blackBishop] | bitBoards_[PieceType::blackQueen]))
			return true;

		if (Util::computeRookMoves(square, allPieces_) & (bitBoards_[PieceType::blackRook] | bitBoards_[PieceType::blackQueen]))
			return true;
	}

	return false;
}