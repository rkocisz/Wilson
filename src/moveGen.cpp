#include "moveGen.h"
#include "enums.h"
#include "structs.h"
#include "util.h"
#include "moveGenUtil.h"

#include <vector>


namespace MoveGen
{
	namespace
	{
		std::vector<Move> pseudoLegalMoves_;
		std::vector<Move> legalMoves_;

		void generatePseudoLegalMoves()
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
			return;
		}



		void generateWhitePawnMoves()
		{
			uint64_t captureLeft = (bitBoards_[PieceType::whitePawn] << 9) & blackPieces_ & ~FILE_H;
			uint64_t captureRight = (bitBoards_[PieceType::whitePawn] << 7) & blackPieces_ & ~FILE_A;

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

			if (enPassantSquare_ != -1)
			{
				enPassantLeft = (bitBoards_[PieceType::whitePawn] << 9) & Util::squareMask(enPassantSquare_) & ~FILE_H;
				enPassantRight = (bitBoards_[PieceType::whitePawn] << 7) & Util::squareMask(enPassantSquare_) & ~FILE_A;
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

			while (doublePush)
			{
				int endPos = 63 - Util::popLSB(doublePush);
				int startPos = endPos + 16;
				pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::whitePawn, PieceType::empty, PieceType::empty, MoveType::normal);
			}
		}

		void generateWhiteKnightMoves()
		{
			uint64_t knights = bitBoards_[PieceType::whiteKnight];

			while (knights)
			{
				int startPos = 63 - Util::popLSB(knights);
				uint64_t attacks = Util::knightMoves_[startPos] & ~whitePieces_;

				while (attacks)
				{
					int endPos = 63 - Util::popLSB(attacks);
					pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::whiteKnight, board_[endPos], PieceType::empty, MoveType::normal);
				}
			}
		}

		void generateWhiteBishopMoves()
		{
			uint64_t bishops = bitBoards_[PieceType::whiteBishop];

			while (bishops)
			{
				int startPos = 63 - Util::popLSB(bishops);
				uint64_t moves = Util::getBishopMoves(startPos, allPieces_) & ~whitePieces_;

				while (moves)
				{
					int endPos = 63 - Util::popLSB(moves);
					pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::whiteBishop, board_[endPos], PieceType::empty, MoveType::normal);
				}
			}
		}

		void generateWhiteRookMoves()
		{
			uint64_t rooks = bitBoards_[PieceType::whiteRook];

			while (rooks)
			{
				int startPos = 63 - Util::popLSB(rooks);
				uint64_t moves = Util::getRookMoves(startPos, allPieces_) & ~whitePieces_;

				while (moves)
				{
					int endPos = 63 - Util::popLSB(moves);
					pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::whiteRook, board_[endPos], PieceType::empty, MoveType::normal);
				}
			}
		}

		void generateWhiteQueenMoves()
		{
			uint64_t queens = bitBoards_[PieceType::whiteQueen];
			while (queens)
			{
				int startPos = 63 - Util::popLSB(queens);
				uint64_t moves = (Util::getBishopMoves(startPos, allPieces_) | Util::getRookMoves(startPos, allPieces_)) & ~whitePieces_;
				while (moves)
				{
					int endPos = 63 - Util::popLSB(moves);
					pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::whiteQueen, board_[endPos], PieceType::empty, MoveType::normal);
				}
			}
		}

		void generateWhiteKingMoves()
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

		void generateBlackPawnMoves()
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

		void generateBlackKnightMoves()
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

		void generateBlackBishopMoves()
		{
			uint64_t bishops = bitBoards_[PieceType::blackBishop];

			while (bishops)
			{
				int startPos = 63 - Util::popLSB(bishops);
				uint64_t moves = Util::getBishopMoves(startPos, allPieces_) & ~blackPieces_;

				while (moves)
				{
					int endPos = 63 - Util::popLSB(moves);
					pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::blackBishop, board_[endPos], PieceType::empty, MoveType::normal);
				}
			}
		}

		void generateBlackRookMoves()
		{
			uint64_t rooks = bitBoards_[PieceType::blackRook];

			while (rooks)
			{
				int startPos = 63 - Util::popLSB(rooks);
				uint64_t moves = Util::getRookMoves(startPos, allPieces_) & ~blackPieces_;

				while (moves)
				{
					int endPos = 63 - Util::popLSB(moves);
					pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::blackRook, board_[endPos], PieceType::empty, MoveType::normal);
				}
			}
		}

		void generateBlackQueenMoves()
		{
			uint64_t queens = bitBoards_[PieceType::blackQueen];
			while (queens)
			{
				int startPos = 63 - Util::popLSB(queens);
				uint64_t moves = (Util::getBishopMoves(startPos, allPieces_) | Util::getRookMoves(startPos, allPieces_)) & ~blackPieces_;
				while (moves)
				{
					int endPos = 63 - Util::popLSB(moves);
					pseudoLegalMoves_.emplace_back(startPos, endPos, PieceType::blackQueen, board_[endPos], PieceType::empty, MoveType::normal);
				}
			}
		}

		void generateBlackKingMoves()
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

		bool isSquareAttacked(int square, Color attacker)
		{
			uint64_t squareMask = Util::squareMask(square);

			if (attacker == Color::white)
			{
				uint64_t attackers = ((squareMask >> 7) & ~FILE_H) | ((squareMask >> 9) & ~FILE_A);
				if (attackers & bitBoards_[PieceType::whitePawn])
					return true;

				if (Util::knightMoves_[square] & bitBoards_[PieceType::whiteKnight])
					return true;


				if (Util::kingMoves_[square] & bitBoards_[PieceType::whiteKing])
					return true;

				if (Util::getBishopMoves(square, allPieces_) & (bitBoards_[PieceType::whiteBishop] | bitBoards_[PieceType::whiteQueen]))
					return true;

				if (Util::getRookMoves(square, allPieces_) & (bitBoards_[PieceType::whiteRook] | bitBoards_[PieceType::whiteQueen]))
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

				if (Util::getBishopMoves(square, allPieces_) & (bitBoards_[PieceType::blackBishop] | bitBoards_[PieceType::blackQueen]))
					return true;

				if (Util::getRookMoves(square, allPieces_) & (bitBoards_[PieceType::blackRook] | bitBoards_[PieceType::blackQueen]))
					return true;
			}

			return false;
		}
	}

	std::vector<Move> generateLegalMoves(Board& board)
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

	
}

