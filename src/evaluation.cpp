#include "evaluation.h"
#include "common.h"
#include "util.h"
#include "moveGenUtil.h"

namespace Eval
{
	namespace
	{
		int mgTable_[12][64];
		int egTable_[12][64];
	
		int mgValue_[6] = {20000, 1025, 477, 365, 337, 82};
		int egValue_[6] = {20000, 936, 512, 297, 281, 94};

		int gamePhaseValue[6] = {0, 4, 2, 1, 1, 0};
		int gamePhase_;

		uint64_t whitePassedPawnMask[64];
		uint64_t blackPassedPawnMask[64];


		int evaluateMaterial(const Board& board)
		{
			int mgVal = 0;
			int egVal = 0;
			gamePhase_ = 0;

			for (int i = 0; i < 6; i++)
			{
				uint64_t pieceSquares = board.bitBoards_[i];

				while (pieceSquares)
				{
					int pos = 63 - Util::popLSB(pieceSquares);

					mgVal += mgTable_[i][pos];
					egVal += egTable_[i][pos];

					gamePhase_ += gamePhaseValue[i];
				}

				int blackPieceIndex = i + 6;
				pieceSquares = board.bitBoards_[blackPieceIndex];

				while (pieceSquares)
				{
					int pos = 63 - Util::popLSB(pieceSquares);

					mgVal -= mgTable_[blackPieceIndex][pos];
					egVal -= egTable_[blackPieceIndex][pos];

					gamePhase_ += gamePhaseValue[i];
				}
			}

			if (gamePhase_ > 24)
				gamePhase_ = 24;

			return (mgVal * gamePhase_ + egVal * (24 - gamePhase_)) / 24;
		}


		int evaluatePawnStructure(const Board& board)//run after evaluateMaterial(const Board& board) bc it updates gamePhase
		{
			int eval = 0;

			int doubledWhitePawnCount = Util::bitCount(board.bitBoards_[PieceType::whitePawn] & (board.bitBoards_[PieceType::whitePawn] >> 8));
			int doubledBlackPawnCount = Util::bitCount(board.bitBoards_[PieceType::blackPawn] & (board.bitBoards_[PieceType::blackPawn] << 8));

			eval -= doubledPawnPentalty * (doubledWhitePawnCount);
			eval += doubledPawnPentalty * (doubledBlackPawnCount);

			uint64_t whitePawns = board.bitBoards_[PieceType::whitePawn];
			uint64_t blackPawns = board.bitBoards_[PieceType::blackPawn];
			
			while (whitePawns)
			{
				int pos = 63 - Util::popLSB(whitePawns);
				int file = pos % 8;
				
				if (!(adjacentFileMasks[file] & board.bitBoards_[PieceType::whitePawn]))
				{
					eval -= isolatedPawnPentalty;
				}

				if (!(whitePassedPawnMask[pos] & board.bitBoards_[PieceType::blackPawn]))
				{
					eval += whitePassedPawnBonus[pos];
				}
			}
			while (blackPawns)
			{
				int pos = 63 - Util::popLSB(blackPawns);
				int file = pos % 8;

				if (!(adjacentFileMasks[file] & board.bitBoards_[PieceType::blackPawn]))
				{
					eval += isolatedPawnPentalty;
				}
				if (!(blackPassedPawnMask[pos] & board.bitBoards_[PieceType::whitePawn]))
				{
					eval -= blackPassedPawnBonus[pos];
				}
			}

			return eval;
		}
	}


	int evaluatePieceStructure(const Board& board)
	{
		int eval = 0;
		uint64_t whiteKnights = board.bitBoards_[PieceType::whiteKnight];
		uint64_t blackKnights = board.bitBoards_[PieceType::blackKnight];

		uint64_t whiteBishops = board.bitBoards_[PieceType::whiteBishop];
		uint64_t blackBishops = board.bitBoards_[PieceType::blackBishop];

		uint64_t whiteRooks = board.bitBoards_[PieceType::whiteRook];
		uint64_t blackRooks = board.bitBoards_[PieceType::blackRook];

		uint64_t whiteQueens = board.bitBoards_[PieceType::whiteQueen];
		uint64_t blackQueens = board.bitBoards_[PieceType::blackQueen];

		if (Util::bitCount(board.bitBoards_[PieceType::whiteBishop]) >= 2)
			eval += 30;

		if (Util::bitCount(board.bitBoards_[PieceType::blackBishop]) >= 2)
			eval -= 30;

		while (whiteKnights)
		{
			int pos = 63 - Util::popLSB(whiteKnights);

			uint64_t moves = MoveGenUtil::knightMoves_[pos] & ~board.whitePieces_;
			eval += (Util::bitCount(moves) - 4) * 4;
		}
		while (blackKnights)
		{
			int pos = 63 - Util::popLSB(blackKnights);

			uint64_t moves = MoveGenUtil::knightMoves_[pos] & ~board.blackPieces_;
			eval -= (Util::bitCount(moves) - 4) * 4;
		}

		while (whiteBishops)
		{
			int pos = 63 - Util::popLSB(whiteBishops);

			uint64_t moves = MoveGenUtil::getBishopMoves(pos, board.allPieces_) & ~board.whitePieces_;
			eval += (Util::bitCount(moves) - 5) * 3;
		}
		while (blackBishops)
		{
			int pos = 63 - Util::popLSB(blackBishops);

			uint64_t moves = MoveGenUtil::getBishopMoves(pos, board.allPieces_) & ~board.blackPieces_;
			eval -= (Util::bitCount(moves) - 5) * 3;
		}

		while (whiteRooks)
		{
			int pos = 63 - Util::popLSB(whiteRooks);
			int file = pos % 8;

			uint64_t moves = MoveGenUtil::getRookMoves(pos, board.allPieces_) & ~board.whitePieces_;
			eval += (Util::bitCount(moves) - 6) * 2;

			if (!(fileMasks[file] & board.bitBoards_[PieceType::whitePawn]))
			{
				eval += 15;
				if (!(fileMasks[file] & board.bitBoards_[PieceType::blackPawn]))
				{
					eval += 10;
				}
			}
		}
		while (blackRooks)
		{
			int pos = 63 - Util::popLSB(blackRooks);
			int file = pos % 8;

			uint64_t moves = MoveGenUtil::getRookMoves(pos, board.allPieces_) & ~board.blackPieces_;
			eval -= (Util::bitCount(moves) - 6) * 2;

			if (!(fileMasks[file] & board.bitBoards_[PieceType::blackPawn]))
			{
				eval -= 15;
				if (!(fileMasks[file] & board.bitBoards_[PieceType::whitePawn]))
				{
					eval -= 10;
				}
			}
		}

		while (whiteQueens)
		{
			int pos = 63 - Util::popLSB(whiteQueens);

			uint64_t moves = (MoveGenUtil::getRookMoves(pos, board.allPieces_) | MoveGenUtil::getBishopMoves(pos, board.allPieces_)) & ~board.whitePieces_;
			eval += (Util::bitCount(moves) - 10);
		}
		while (blackQueens)
		{
			int pos = 63 - Util::popLSB(blackQueens);

			uint64_t moves = (MoveGenUtil::getRookMoves(pos, board.allPieces_) | MoveGenUtil::getBishopMoves(pos, board.allPieces_)) & ~board.blackPieces_;
			eval -= (Util::bitCount(moves) - 10);
		}

		return eval;
	}


	void init()
	{
		for(int i = 0; i < 64; i++)
		{
			mgTable_[0][i] = mgValue_[0] + mgKingTable[i];
			mgTable_[1][i] = mgValue_[1] + mgQueenTable[i];
			mgTable_[2][i] = mgValue_[2] + mgRookTable[i];
			mgTable_[3][i] = mgValue_[3] + mgBishopTable[i];
			mgTable_[4][i] = mgValue_[4] + mgKnightTable[i];
			mgTable_[5][i] = mgValue_[5] + mgPawnTable[i];

			mgTable_[6][i] = mgValue_[0] + mgKingTable[i ^ 56]; ///////////////////////////////////////
			mgTable_[7][i] = mgValue_[1] + mgQueenTable[i ^ 56];
			mgTable_[8][i] = mgValue_[2] + mgRookTable[i ^ 56];
			mgTable_[9][i] = mgValue_[3] + mgBishopTable[i ^ 56];
			mgTable_[10][i] = mgValue_[4] + mgKnightTable[i ^ 56];
			mgTable_[11][i] = mgValue_[5] + mgPawnTable[i ^ 56];

			egTable_[0][i] = egValue_[0] + egKingTable[i];
			egTable_[1][i] = egValue_[1] + egQueenTable[i];
			egTable_[2][i] = egValue_[2] + egRookTable[i];
			egTable_[3][i] = egValue_[3] + egBishopTable[i];
			egTable_[4][i] = egValue_[4] + egKnightTable[i];
			egTable_[5][i] = egValue_[5] + egPawnTable[i];

			egTable_[6][i] = egValue_[0] + egKingTable[i ^ 56];
			egTable_[7][i] = egValue_[1] + egQueenTable[i ^ 56];
			egTable_[8][i] = egValue_[2] + egRookTable[i ^ 56];
			egTable_[9][i] = egValue_[3] + egBishopTable[i ^ 56];
			egTable_[10][i] = egValue_[4] + egKnightTable[i ^ 56];
			egTable_[11][i] = egValue_[5] + egPawnTable[i ^ 56];

			for (int i = 8; i < 64; i++)
			{
				int file = i % 8;
				whitePassedPawnMask[i] = fileMasks[file] | adjacentFileMasks[file];
				whitePassedPawnMask[i] &= ~((1ULL << (i + 1)) - 1);
			}
			for (int i = 8; i < 56; i++)
			{
				blackPassedPawnMask[i] = whitePassedPawnMask[i ^ 56];
			}
		}
	}

	int evaluate(const Board& board)
	{
		int eval = 0;

		eval += evaluateMaterial(board);
		eval += evaluatePieceStructure(board);
		eval += evaluatePawnStructure(board);

		return (board.sideToMove_ == Color::white) ? eval : -eval;
	}
}