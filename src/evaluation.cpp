#include "evaluation.h"
#include "common.h"
#include "util.h"

namespace Eval
{
	namespace
	{
		int mgTable_[12][64];
		int egTable_[12][64];
	
		int mgValue_[6] = {20000, 1025, 477, 365, 337, 82};
		int egValue_[6] = {20000, 936, 512, 297, 281, 94};

		int gamePhaseValue[6] = {0, 4, 2, 1, 1, 0	};
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

			mgTable_[6][i] = mgValue_[0] + mgKingTable[i ^ 56];
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
		}
	}

	int evaluate(const Board& board)
	{
		int mgVal = 0;
		int egVal = 0;
		int gamePhase = 0;


		for (int i = 0; i < 6; i++)
		{
			uint64_t pieceSquares = board.bitBoards_[i];

			while (pieceSquares)
			{
				int pos = Util::popLSB(pieceSquares);

				mgVal += mgTable_[i][pos];
				egVal += egTable_[i][pos];

				gamePhase += gamePhaseValue[i];
			}

			int blackPieceIndex = i + 6;
			pieceSquares = board.bitBoards_[blackPieceIndex];

			while (pieceSquares)
			{
				int pos = Util::popLSB(pieceSquares);

				mgVal -= mgTable_[blackPieceIndex][pos];
				egVal -= egTable_[blackPieceIndex][pos];

				gamePhase += gamePhaseValue[i];
			}
		}

		if(gamePhase > 24)
			gamePhase = 24;

		int eval = (mgVal * gamePhase + egVal * (24 - gamePhase)) /24;

		return (board.sideToMove_ == Color::white) ? eval : -eval;
	}
}