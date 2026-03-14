#include "moveGen.h"
#include "enums.h"
#include "structs.h"
#include "util.h"
#include "moveGenUtil.h"
#include "common.h"

#include <vector>
#include <bit>
#include <cassert>


namespace MoveGen
{
	namespace
	{
		std::vector<Move> pseudoLegalMoves_;
		std::vector<Move> legalMoves_;

		Board* board_;
		
		/*std::array<PieceType, 64> board_->boardArray_;
		std::array<uint64_t, 12> board_->bitBoards_;
		uint64_t board_->whitePieces_;
		uint64_t board_->blackPieces_;
		uint64_t board_->allPieces_;

		uint8_t board_->castlingRights_;
		int board_->enPassantSquare_;
		Color sideToMove_;*/


		void generateWhitePawnMoves()
		{
			uint64_t captureLeft = (board_->bitBoards_[PieceType::whitePawn] << 9) & board_->blackPieces_ & ~FILE_H;
			uint64_t captureRight = (board_->bitBoards_[PieceType::whitePawn] << 7) & board_->blackPieces_ & ~FILE_A;

			uint64_t promoLeft = captureLeft & RANK_8;
			uint64_t promoRight = captureRight & RANK_8;

			uint64_t normalLeft = captureLeft & ~RANK_8;
			uint64_t normalRight = captureRight & ~RANK_8;

			uint64_t singlePush = (board_->bitBoards_[PieceType::whitePawn] << 8) & ~board_->allPieces_;

			uint64_t singlePushPromo = singlePush & RANK_8;
			uint64_t singlePushNonPromo = singlePush & ~RANK_8;

			uint64_t doublePush = ((singlePush & RANK_3) << 8) & ~board_->allPieces_;

			uint64_t enPassantLeft = 0ULL;
			uint64_t enPassantRight = 0ULL;

			if (board_->enPassantSquare_ != -1)
			{
				enPassantLeft = (board_->bitBoards_[PieceType::whitePawn] << 9) & Util::squareMask(board_->enPassantSquare_) & ~FILE_H;
				enPassantRight = (board_->bitBoards_[PieceType::whitePawn] << 7) & Util::squareMask(board_->enPassantSquare_) & ~FILE_A;
			}

			while (enPassantLeft)
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

				pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::whitePawn, board_->boardArray_[endPos], PieceType::empty, MoveType::normal);
			}

			while (normalRight)
			{
				int endPos = 63 - Util::popLSB(normalRight);
				int startPos = endPos + 7;

				pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::whitePawn, board_->boardArray_[endPos], PieceType::empty, MoveType::normal);
			}

			while (promoLeft)
			{
				int endPos = 63 - Util::popLSB(promoLeft);
				int startPos = endPos + 9;

				for (int i = 1; i < 5; i++)
				{
					pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::whitePawn, board_->boardArray_[endPos], static_cast<PieceType>(i), MoveType::normal);
				}
			}

			while (promoRight)
			{
				int endPos = 63 - Util::popLSB(promoRight);
				int startPos = endPos + 7;
				for (int i = 1; i < 5; i++)
				{
					pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::whitePawn, board_->boardArray_[endPos], static_cast<PieceType>(i), MoveType::normal);
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

			while (doublePush)
			{
				int endPos = 63 - Util::popLSB(doublePush);
				int startPos = endPos + 16;
				pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::whitePawn, PieceType::empty, PieceType::empty, MoveType::normal);
			}
		}


		void generateWhiteKnightMoves()
		{
			uint64_t knights = board_->bitBoards_[PieceType::whiteKnight];

			while (knights)
			{
				int startPos = 63 - Util::popLSB(knights);
				uint64_t attacks = MoveGenUtil::knightMoves_[startPos] & ~board_->whitePieces_;

				while (attacks)
				{
					int endPos = 63 - Util::popLSB(attacks);
					pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::whiteKnight, board_->boardArray_[endPos], PieceType::empty, MoveType::normal);
				}
			}
		}


		void generateWhiteBishopMoves()
		{
			uint64_t bishops = board_->bitBoards_[PieceType::whiteBishop];

			while (bishops)
			{
				int startPos = 63 - Util::popLSB(bishops);
				uint64_t moves = MoveGenUtil::getBishopMoves(startPos, board_->allPieces_) & ~board_->whitePieces_;

				while (moves)
				{
					int endPos = 63 - Util::popLSB(moves);
					pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::whiteBishop, board_->boardArray_[endPos], PieceType::empty, MoveType::normal);
				}
			}
		}


		void generateWhiteRookMoves()
		{
			uint64_t rooks = board_->bitBoards_[PieceType::whiteRook];

			while (rooks)
			{
				int startPos = 63 - Util::popLSB(rooks);
				assert(startPos >= 0 && startPos < 64);

				uint64_t moves = MoveGenUtil::getRookMoves(startPos, board_->allPieces_) & ~board_->whitePieces_;

				while (moves)
				{
					int endPos = 63 - Util::popLSB(moves);
					pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::whiteRook, board_->boardArray_[endPos], PieceType::empty, MoveType::normal);
				}
			}
		}


		void generateWhiteQueenMoves()
		{
			uint64_t queens = board_->bitBoards_[PieceType::whiteQueen];
			while (queens)
			{
				int startPos = 63 - Util::popLSB(queens);
				uint64_t moves = (MoveGenUtil::getBishopMoves(startPos, board_->allPieces_) | MoveGenUtil::getRookMoves(startPos, board_->allPieces_)) & ~board_->whitePieces_;
				while (moves)
				{
					int endPos = 63 - Util::popLSB(moves);
					pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::whiteQueen, board_->boardArray_[endPos], PieceType::empty, MoveType::normal);
				}
			}
		}


		void generateWhiteKingMoves()
		{
			int startPos = 63 - std::countr_zero(board_->bitBoards_[PieceType::whiteKing]);
			uint64_t kingMoves = MoveGenUtil::kingMoves_[startPos] & ~board_->whitePieces_;

			while (kingMoves)
			{
				int endPos = 63 - Util::popLSB(kingMoves);
				pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::whiteKing, board_->boardArray_[endPos], PieceType::empty, MoveType::normal);
			}

			if (board_->castlingRights_ & CastlingRights::whiteKingSide && !(board_->allPieces_ & (Util::squareMask(61) | Util::squareMask(62))))
			{
				pseudoLegalMoves_.emplace_back(60, 62, PieceType::whiteKing, PieceType::empty, PieceType::empty, MoveType::shortCastle);
			}
			if (board_->castlingRights_ & CastlingRights::whiteQueenSide && !(board_->allPieces_ & (Util::squareMask(59) | Util::squareMask(58) | Util::squareMask(57))))
			{
				pseudoLegalMoves_.emplace_back(60, 58, PieceType::whiteKing, PieceType::empty, PieceType::empty, MoveType::longCastle);
			}
		}


		void generateBlackPawnMoves()
		{
			uint64_t captureLeft = (board_->bitBoards_[PieceType::blackPawn] >> 7) & board_->whitePieces_ & ~FILE_H;
			uint64_t captureRight = (board_->bitBoards_[PieceType::blackPawn] >> 9) & board_->whitePieces_ & ~FILE_A;

			uint64_t promoLeft = captureLeft & RANK_1;
			uint64_t promoRight = captureRight & RANK_1;

			uint64_t normalLeft = captureLeft & ~RANK_1;
			uint64_t normalRight = captureRight & ~RANK_1;

			uint64_t singlePush = (board_->bitBoards_[PieceType::blackPawn] >> 8) & ~board_->allPieces_;

			uint64_t singlePushPromo = singlePush & RANK_1;
			uint64_t singlePushNonPromo = singlePush & ~RANK_1;

			uint64_t doublePush = ((singlePush & RANK_6) >> 8) & ~board_->allPieces_;

			uint64_t enPassantLeft = 0ULL;
			uint64_t enPassantRight = 0ULL;

			if (board_->enPassantSquare_ != -1)
			{
				enPassantLeft = (board_->bitBoards_[PieceType::blackPawn] >> 7) & Util::squareMask(board_->enPassantSquare_) & ~FILE_H;
				enPassantRight = (board_->bitBoards_[PieceType::blackPawn] >> 9) & Util::squareMask(board_->enPassantSquare_) & ~FILE_A;
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

				pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::blackPawn, board_->boardArray_[endPos], PieceType::empty, MoveType::normal);
			}

			while (normalRight)
			{
				int endPos = 63 - Util::popLSB(normalRight);
				int startPos = endPos - 9;

				pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::blackPawn, board_->boardArray_[endPos], PieceType::empty, MoveType::normal);
			}

			while (promoLeft)
			{
				int endPos = 63 - Util::popLSB(promoLeft);
				int startPos = endPos - 7;

				for (int i = 7; i < 11; i++)
				{
					pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::blackPawn, board_->boardArray_[endPos], static_cast<PieceType>(i), MoveType::normal);
				}
			}

			while (promoRight)
			{
				int endPos = 63 - Util::popLSB(promoRight);
				int startPos = endPos - 9;
				for (int i = 7; i < 11; i++)
				{
					pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::blackPawn, board_->boardArray_[endPos], static_cast<PieceType>(i), MoveType::normal);
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


		void generateBlackKnightMoves()
		{
			uint64_t knights = board_->bitBoards_[PieceType::blackKnight];

			while (knights)
			{
				int startPos = 63 - Util::popLSB(knights);
				uint64_t attacks = MoveGenUtil::knightMoves_[startPos] & ~board_->blackPieces_;

				while (attacks)
				{
					int endPos = 63 - Util::popLSB(attacks);
					pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::blackKnight, board_->boardArray_[endPos], PieceType::empty, MoveType::normal);
				}
			}
		}


		void generateBlackBishopMoves()
		{
			uint64_t bishops = board_->bitBoards_[PieceType::blackBishop];

			while (bishops)
			{
				int startPos = 63 - Util::popLSB(bishops);
				uint64_t moves = MoveGenUtil::getBishopMoves(startPos, board_->allPieces_) & ~board_->blackPieces_;

				while (moves)
				{
					int endPos = 63 - Util::popLSB(moves);
					pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::blackBishop, board_->boardArray_[endPos], PieceType::empty, MoveType::normal);
				}
			}
		}


		void generateBlackRookMoves()
		{
			uint64_t rooks = board_->bitBoards_[PieceType::blackRook];

			while (rooks)
			{
				int startPos = 63 - Util::popLSB(rooks);
				assert(startPos >= 0 && startPos < 64);
				uint64_t moves = MoveGenUtil::getRookMoves(startPos, board_->allPieces_) & ~board_->blackPieces_;

				while (moves)
				{
					int endPos = 63 - Util::popLSB(moves);
					pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::blackRook, board_->boardArray_[endPos], PieceType::empty, MoveType::normal);
				}
			}
		}


		void generateBlackQueenMoves()
		{
			uint64_t queens = board_->bitBoards_[PieceType::blackQueen];
			while (queens)
			{
				int startPos = 63 - Util::popLSB(queens);
				uint64_t moves = (MoveGenUtil::getBishopMoves(startPos, board_->allPieces_) | MoveGenUtil::getRookMoves(startPos, board_->allPieces_)) & ~board_->blackPieces_;
				while (moves)
				{
					int endPos = 63 - Util::popLSB(moves);
					pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::blackQueen, board_->boardArray_[endPos], PieceType::empty, MoveType::normal);
				}
			}
		}


		void generateBlackKingMoves()
		{
			int startPos = 63 - std::countr_zero(board_->bitBoards_[PieceType::blackKing]);
			uint64_t kingMoves = MoveGenUtil::kingMoves_[startPos] & ~board_->blackPieces_;

			while (kingMoves)
			{
				int endPos = 63 - Util::popLSB(kingMoves);
				pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::blackKing, board_->boardArray_[endPos], PieceType::empty, MoveType::normal);
			}

			if (board_->castlingRights_ & CastlingRights::blackKingSide && !(board_->allPieces_ & (Util::squareMask(5) | Util::squareMask(6))))
			{
				pseudoLegalMoves_.emplace_back(4, 6, PieceType::blackKing, PieceType::empty, PieceType::empty, MoveType::shortCastle);
			}
			if (board_->castlingRights_ & CastlingRights::blackQueenSide && !(board_->allPieces_ & (Util::squareMask(1) | Util::squareMask(2) | Util::squareMask(3))))
			{
				pseudoLegalMoves_.emplace_back(4, 2, PieceType::blackKing, PieceType::empty, PieceType::empty, MoveType::longCastle);
			}
		}


		bool isSquareAttacked(int square, Color attacker)
		{
			uint64_t squareMask = Util::squareMask(square);

			if (attacker == Color::white)
			{
				uint64_t attackers = ((squareMask >> 7) & ~FILE_H) | ((squareMask >> 9) & ~FILE_A);
				if (attackers & board_->bitBoards_[PieceType::whitePawn])
					return true;

				if (MoveGenUtil::knightMoves_[square] & board_->bitBoards_[PieceType::whiteKnight])
					return true;


				if (MoveGenUtil::kingMoves_[square] & board_->bitBoards_[PieceType::whiteKing])
					return true;

				if (MoveGenUtil::getBishopMoves(square, board_->allPieces_) & (board_->bitBoards_[PieceType::whiteBishop] | board_->bitBoards_[PieceType::whiteQueen]))
					return true;

				assert(square >= 0 && square < 64);

				if (MoveGenUtil::getRookMoves(square, board_->allPieces_) & (board_->bitBoards_[PieceType::whiteRook] | board_->bitBoards_[PieceType::whiteQueen]))
					return true;

			}
			else
			{
				uint64_t attackers = ((squareMask << 7) & ~FILE_A) | ((squareMask << 9) & ~FILE_H);
				if (attackers & board_->bitBoards_[PieceType::blackPawn])
					return true;

				if (MoveGenUtil::knightMoves_[square] & board_->bitBoards_[PieceType::blackKnight])
					return true;

				if (MoveGenUtil::kingMoves_[square] & board_->bitBoards_[PieceType::blackKing])
					return true;

				if (MoveGenUtil::getBishopMoves(square, board_->allPieces_) & (board_->bitBoards_[PieceType::blackBishop] | board_->bitBoards_[PieceType::blackQueen]))
					return true;

				assert(square >= 0 && square < 64);

				if (MoveGenUtil::getRookMoves(square, board_->allPieces_) & (board_->bitBoards_[PieceType::blackRook] | board_->bitBoards_[PieceType::blackQueen]))
					return true;
			}

			return false;
		}


		void generatePseudoLegalMoves()
		{
			pseudoLegalMoves_.clear();

			if (board_->sideToMove_ == Color::white)
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
			return;
		}
	}


	std::vector<Move> generateLegalMoves(Board* board)
	{
		board_ = board;

		legalMoves_.clear();
		generatePseudoLegalMoves();

		Color moving = board_->sideToMove_;
		uint64_t kingMask = 0ULL;

		for (const Move& move : pseudoLegalMoves_)
		{
			board_->makeMove(move);

			if (moving == Color::white)
			{
				kingMask = board_->bitBoards_[PieceType::whiteKing];
			}
			else
			{
				kingMask = board_->bitBoards_[PieceType::blackKing];
			}

			int kingPos = 63 - Util::popLSB(kingMask);

			if (!isSquareAttacked(kingPos, Util::opposite(moving)))
			{
				legalMoves_.push_back(move);
			}

			board_->unmakeMove(move);
		}
		return legalMoves_;
	}
}

