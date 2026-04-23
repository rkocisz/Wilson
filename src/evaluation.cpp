#include "evaluation.h"
#include "common.h"
#include "util.h"
#include "moveGenUtil.h"

#include <iostream>

namespace Eval
{
	namespace
	{
		int mgTable_[12][64];
		int egTable_[12][64];
	
		int mgValue_[6] = {0, 1025, 477, 365, 337, 82};
		int egValue_[6] = {0, 936, 512, 297, 281, 94};

		int gamePhaseValue_[6] = {0, 4, 2, 1, 1, 0};

		uint64_t whitePassedPawnMask_[64];
		uint64_t blackPassedPawnMask_[64];

		uint64_t whiteKingSideCastleAreaMask = 0ULL;
		uint64_t whiteQueenSideCastleAreaMask = 0ULL;

		uint64_t blackKingSideCastleAreaMask = 0ULL;
		uint64_t blackQueenSideCastleAreaMask = 0ULL;

		uint64_t kingZoneMask_[2][64];

		int unitsAttackingWhiteKingZone = 0;
		int unitsAttackingBlackKingZone = 0;
	}

	int evaluateMaterial(Board& board)
	{
		board.gamePhase_ = 0;
		board.mgVal_ = 0;
		board.egVal_ = 0;

		for (int i = 0; i < 6; i++)
		{
			uint64_t pieceSquares = board.bitBoards_[i];

			while (pieceSquares)
			{
				int pos = 63 - Util::popLSB(pieceSquares);

				board.mgVal_ += mgTable_[i][pos];
				board.egVal_ += egTable_[i][pos];

				board.gamePhase_ += gamePhaseValue_[i];
			}

			int blackPieceIndex = i + 6;
			pieceSquares = board.bitBoards_[blackPieceIndex];

			while (pieceSquares)
			{
				int pos = 63 - Util::popLSB(pieceSquares);

				board.mgVal_ -= mgTable_[blackPieceIndex][pos];
				board.egVal_ -= egTable_[blackPieceIndex][pos];

				board.gamePhase_ += gamePhaseValue_[i];
			}
		}

		int gamePhaseForCalculations = board.gamePhase_;

		if (gamePhaseForCalculations > 24) gamePhaseForCalculations = 24;
		else if (gamePhaseForCalculations < 0) gamePhaseForCalculations = 0;

		return ((board.mgVal_ * gamePhaseForCalculations + board.egVal_ * (24 - gamePhaseForCalculations))) / 24;
	}


	int evaluatePawnStructure(Board& board)
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

			if (!(whitePassedPawnMask_[pos] & board.bitBoards_[PieceType::blackPawn]))
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
			if (!(blackPassedPawnMask_[pos] & board.bitBoards_[PieceType::whitePawn]))
			{
				eval -= blackPassedPawnBonus[pos];
			}
		}

		return eval;
	}


	int evaluatePieceStructure(Board& board)
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

		uint64_t whiteKingMask = board.bitBoards_[PieceType::whiteKing];
		uint64_t blackKingMask = board.bitBoards_[PieceType::blackKing];

		unitsAttackingWhiteKingZone = 0;
		unitsAttackingBlackKingZone = 0;

		int whiteKingPos = 63 - Util::popLSB(whiteKingMask);
		int blackKingPos = 63 - Util::popLSB(blackKingMask);

		if (Util::bitCount(board.bitBoards_[PieceType::whiteBishop]) >= 2)
			eval += 30;

		if (Util::bitCount(board.bitBoards_[PieceType::blackBishop]) >= 2)
			eval -= 30;

		while (whiteKnights)
		{
			int pos = 63 - Util::popLSB(whiteKnights);

			uint64_t moves = MoveGenUtil::knightMoves_[pos] & ~board.whitePieces_;
			eval += (Util::bitCount(moves) - 4) * 4;

			if (moves & kingZoneMask_[1][blackKingPos])
			{
				unitsAttackingBlackKingZone += kingAttackingPieceUnits[0];
			}
		}
		while (blackKnights)
		{
			int pos = 63 - Util::popLSB(blackKnights);

			uint64_t moves = MoveGenUtil::knightMoves_[pos] & ~board.blackPieces_;
			eval -= (Util::bitCount(moves) - 4) * 4;

			if (moves & kingZoneMask_[0][whiteKingPos])
			{
				unitsAttackingWhiteKingZone += kingAttackingPieceUnits[0];
			}
		}

		while (whiteBishops)
		{
			int pos = 63 - Util::popLSB(whiteBishops);

			uint64_t moves = MoveGenUtil::getBishopMoves(pos, board.allPieces_) & ~board.whitePieces_;
			eval += (Util::bitCount(moves) - 5) * 3;

			if (moves & kingZoneMask_[1][blackKingPos])
			{
				unitsAttackingBlackKingZone += kingAttackingPieceUnits[1];
			}
			else
			{
				uint64_t xrayMoves = MoveGenUtil::getBishopMoves(pos, 0ULL);
				if (xrayMoves & kingZoneMask_[1][blackKingPos])
				{
					unitsAttackingBlackKingZone += kingAttackingPieceUnits[1] / 2;
				}
			}
		}
		while (blackBishops)
		{
			int pos = 63 - Util::popLSB(blackBishops);

			uint64_t moves = MoveGenUtil::getBishopMoves(pos, board.allPieces_) & ~board.blackPieces_;
			eval -= (Util::bitCount(moves) - 5) * 3;

			if (moves & kingZoneMask_[0][whiteKingPos])
			{
				unitsAttackingWhiteKingZone += kingAttackingPieceUnits[1];
			}
			else
			{
				uint64_t xrayMoves = MoveGenUtil::getBishopMoves(pos, 0ULL);
				if (xrayMoves & kingZoneMask_[0][whiteKingPos])
				{
					unitsAttackingWhiteKingZone += kingAttackingPieceUnits[1] / 2;
				}
			}
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

			if (Util::squareMask(pos) & RANK_7)
			{
				eval += 20;
			}

			if (moves & kingZoneMask_[1][blackKingPos])
			{
				unitsAttackingBlackKingZone += kingAttackingPieceUnits[2];
			}
			else
			{
				uint64_t xrayMoves = MoveGenUtil::getRookMoves(pos, 0ULL);
				if (xrayMoves & kingZoneMask_[1][blackKingPos])
				{
					unitsAttackingBlackKingZone += kingAttackingPieceUnits[2] / 2;
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

			if (Util::squareMask(pos) & RANK_2)
			{
				eval -= 20;
			}

			if (moves & kingZoneMask_[0][whiteKingPos])
			{
				unitsAttackingWhiteKingZone += kingAttackingPieceUnits[2];
			}
			else
			{
				uint64_t xrayMoves = MoveGenUtil::getRookMoves(pos, 0ULL);
				if (xrayMoves & kingZoneMask_[0][whiteKingPos])
				{
					unitsAttackingWhiteKingZone += kingAttackingPieceUnits[2] / 2;
				}
			}
		}

		while (whiteQueens)
		{
			int pos = 63 - Util::popLSB(whiteQueens);

			uint64_t moves = (MoveGenUtil::getRookMoves(pos, board.allPieces_) | MoveGenUtil::getBishopMoves(pos, board.allPieces_)) & ~board.whitePieces_;
			eval += (Util::bitCount(moves) - 8) / 2;

			if (moves & kingZoneMask_[1][blackKingPos])
			{
				unitsAttackingBlackKingZone += kingAttackingPieceUnits[3];
			}
			else
			{
				uint64_t xrayMoves = MoveGenUtil::getRookMoves(pos, 0ULL) | MoveGenUtil::getBishopMoves(pos, 0ULL);
				if (xrayMoves & kingZoneMask_[1][blackKingPos])
				{
					unitsAttackingBlackKingZone += kingAttackingPieceUnits[3] / 2;
				}
			}
		}
		while (blackQueens)
		{
			int pos = 63 - Util::popLSB(blackQueens);

			uint64_t moves = (MoveGenUtil::getRookMoves(pos, board.allPieces_) | MoveGenUtil::getBishopMoves(pos, board.allPieces_)) & ~board.blackPieces_;
			eval -= (Util::bitCount(moves) - 8) / 2;

			if (moves & kingZoneMask_[0][whiteKingPos])
			{
				unitsAttackingWhiteKingZone += kingAttackingPieceUnits[3];
			}
			else
			{
				uint64_t xrayMoves = MoveGenUtil::getRookMoves(pos, 0ULL) | MoveGenUtil::getBishopMoves(pos, 0ULL);
				if (xrayMoves & kingZoneMask_[0][whiteKingPos])
				{
					unitsAttackingWhiteKingZone += kingAttackingPieceUnits[3] / 2;
				}
			}
		}

		return eval;
	}

	int evaluateKingSafety(Board& board) // call after evaluateMaterial() and evaluatePieceStructure()
	{
		int eval = 0;
		int pawnPenalty = 0;

		uint64_t whiteKingMask = board.bitBoards_[PieceType::whiteKing];
		uint64_t blackKingMask = board.bitBoards_[PieceType::blackKing];

		if (whiteKingMask & whiteKingSideCastleAreaMask)
		{
			int pawnsInWall = Util::bitCount(whiteKingSideCastleAreaMask& board.bitBoards_[PieceType::whitePawn]);
			pawnPenalty -= (3 - pawnsInWall) * pawnShieldPenalty;
		}
		else if(whiteKingMask & whiteQueenSideCastleAreaMask)
		{
			int pawnsInWall = Util::bitCount(whiteQueenSideCastleAreaMask & board.bitBoards_[PieceType::whitePawn]);
			pawnPenalty -= (3 - pawnsInWall) * pawnShieldPenalty;
		}

		if (blackKingMask & blackKingSideCastleAreaMask)
		{
			int pawnsInWall = Util::bitCount(blackKingSideCastleAreaMask & board.bitBoards_[PieceType::blackPawn]);
			pawnPenalty += (3 - pawnsInWall) * pawnShieldPenalty;
		}
		else if (blackKingMask & blackQueenSideCastleAreaMask)
		{
			int pawnsInWall = Util::bitCount(blackQueenSideCastleAreaMask & board.bitBoards_[PieceType::blackPawn]);
			pawnPenalty += (3 - pawnsInWall) * pawnShieldPenalty;
		}

		if (unitsAttackingWhiteKingZone > 1 && unitsAttackingWhiteKingZone < 4) eval -= 20;
		else if(unitsAttackingWhiteKingZone > 3 && unitsAttackingWhiteKingZone < 6) eval -= 80;
		else if (unitsAttackingWhiteKingZone > 5 && unitsAttackingWhiteKingZone < 8) eval -= 200;
		else if (unitsAttackingWhiteKingZone > 7 && unitsAttackingWhiteKingZone < 10) eval -= 450;
		else if (unitsAttackingWhiteKingZone > 9) eval -= 850;

		if (unitsAttackingBlackKingZone > 1 && unitsAttackingBlackKingZone < 4) eval += 20;
		else if (unitsAttackingBlackKingZone > 3 && unitsAttackingBlackKingZone < 6) eval += 80;
		else if (unitsAttackingBlackKingZone > 5 && unitsAttackingBlackKingZone < 8) eval += 200;
		else if (unitsAttackingBlackKingZone > 7 && unitsAttackingBlackKingZone < 10) eval += 450;
		else if (unitsAttackingBlackKingZone > 9) eval += 850;


		eval += (pawnPenalty * board.gamePhase_) / 24;
		return eval;
	}

	void updateMaterial(Board& board, const Move& move) //updates the eval info in Board after move
	{
		if (move.moved < 6)
		{
			board.mgVal_ -= mgTable_[move.moved][move.startPos];
			board.mgVal_ += mgTable_[move.moved][move.endPos];
			board.egVal_ -= egTable_[move.moved][move.startPos];
			board.egVal_ += egTable_[move.moved][move.endPos];
		
			if (move.moveType == MoveType::longCastle)
			{
				board.mgVal_ -= mgTable_[PieceType::whiteRook][56];
				board.mgVal_ += mgTable_[PieceType::whiteRook][59];
				board.egVal_ -= egTable_[PieceType::whiteRook][56];
				board.egVal_ += egTable_[PieceType::whiteRook][59];
			}
			else if (move.moveType == MoveType::shortCastle)
			{
				board.mgVal_ -= mgTable_[PieceType::whiteRook][63];
				board.mgVal_ += mgTable_[PieceType::whiteRook][61];
				board.egVal_ -= egTable_[PieceType::whiteRook][63];
				board.egVal_ += egTable_[PieceType::whiteRook][61];
			}
			else if (move.captured != PieceType::empty && move.moveType != MoveType::enPassant)
			{
				board.mgVal_ += mgTable_[move.captured][move.endPos];
				board.egVal_ += egTable_[move.captured][move.endPos];
				board.gamePhase_ -= gamePhaseValue_[move.captured];
			}
			else if (move.moveType == MoveType::enPassant)
			{
				board.mgVal_ += mgTable_[move.captured][move.endPos + 8];
				board.egVal_ += egTable_[move.captured][move.endPos + 8];
				board.gamePhase_ -= gamePhaseValue_[move.captured];
			}

			if (move.promotion != PieceType::empty)
			{
				board.mgVal_ += mgTable_[move.promotion][move.endPos];
				board.egVal_ += egTable_[move.promotion][move.endPos];
				board.mgVal_ -= mgTable_[move.moved][move.endPos];
				board.egVal_ -= egTable_[move.moved][move.endPos];
			}
		}
		else
		{
			board.mgVal_ += mgTable_[move.moved][move.startPos];
			board.mgVal_ -= mgTable_[move.moved][move.endPos];
			board.egVal_ += egTable_[move.moved][move.startPos];
			board.egVal_ -= egTable_[move.moved][move.endPos];

			if (move.moveType == MoveType::longCastle)
			{
				board.mgVal_ += mgTable_[PieceType::blackRook][7];
				board.mgVal_ -= mgTable_[PieceType::blackRook][5];
				board.egVal_ += egTable_[PieceType::blackRook][7];
				board.egVal_ -= egTable_[PieceType::blackRook][5];
			}
			else if (move.moveType == MoveType::shortCastle)
			{
				board.mgVal_ += mgTable_[PieceType::blackRook][0];
				board.mgVal_ -= mgTable_[PieceType::blackRook][3];
				board.egVal_ += egTable_[PieceType::blackRook][0];
				board.egVal_ -= egTable_[PieceType::blackRook][3];
			}
			else if (move.captured != PieceType::empty && move.moveType != MoveType::enPassant)
			{
				board.mgVal_ -= mgTable_[move.captured][move.endPos];
				board.egVal_ -= egTable_[move.captured][move.endPos];
				board.gamePhase_ -= gamePhaseValue_[move.captured];
			}
			else if (move.moveType == MoveType::enPassant)
			{
				board.mgVal_ += mgTable_[move.captured][move.endPos - 8];
				board.egVal_ += egTable_[move.captured][move.endPos - 8];
				board.gamePhase_ -= gamePhaseValue_[move.captured % 6];
			}
			
			if (move.promotion != PieceType::empty)
			{
				board.mgVal_ -= mgTable_[move.promotion][move.endPos];
				board.egVal_ -= egTable_[move.promotion][move.endPos];
				board.mgVal_ += mgTable_[move.moved][move.endPos];
				board.egVal_ += egTable_[move.moved][move.endPos];
			}
		}
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
		
			uint64_t whiteKingZoneMask = MoveGenUtil::kingMoves_[i];	
			uint64_t blackKingZoneMask = MoveGenUtil::kingMoves_[i];
			
			int rank = i / 8;
			if (rank < 6)
			{
				whiteKingZoneMask |= whiteKingZoneMask << 8;
			}
			if (rank > 1)
			{
				blackKingZoneMask |= blackKingZoneMask >> 8;
			}

			kingZoneMask_[0][i] = whiteKingZoneMask;
			kingZoneMask_[1][i] = blackKingZoneMask;
		}

		for (int square = 0; square < 64; square++)
		{
			int file = square % 8;
			uint64_t fileAndAdjacentFilesMask = fileMasks[file] | adjacentFileMasks[file];

			int bitPos = 63 - square;
			uint64_t whiteForwardMask = (square / 8 == 0) ? 0ULL : ~((1ULL << (bitPos + 1)) - 1);
			uint64_t blackForwardMask = (square / 8 == 7) ? 0ULL : ((1ULL << bitPos) - 1);

			whitePassedPawnMask_[square] = fileAndAdjacentFilesMask & whiteForwardMask;
			blackPassedPawnMask_[square] = fileAndAdjacentFilesMask & blackForwardMask;
		}

		whiteKingSideCastleAreaMask |= (1ULL | (1ULL << 1) | (1ULL << 2));
		whiteKingSideCastleAreaMask |= (whiteKingSideCastleAreaMask << 8) | (whiteKingSideCastleAreaMask << 16);

		whiteQueenSideCastleAreaMask |= ((1ULL << 5) | (1ULL << 6) | (1ULL << 7));
		whiteQueenSideCastleAreaMask |= (whiteQueenSideCastleAreaMask << 8) | (whiteQueenSideCastleAreaMask << 16);

		blackKingSideCastleAreaMask |= ((1ULL << 56) | (1ULL << 57) | (1ULL << 58));
		blackKingSideCastleAreaMask |= (blackKingSideCastleAreaMask >> 8) | (blackKingSideCastleAreaMask >> 16);

		blackQueenSideCastleAreaMask |= ((1ULL << 61) | (1ULL << 62) | (1ULL << 63));
		blackQueenSideCastleAreaMask |= (blackQueenSideCastleAreaMask >> 8) | (blackQueenSideCastleAreaMask >> 16);
	}

	int evaluate(Board& board)//fully evaluates the board from the start and updates the eval info in Board
	{
		int eval = 0;

		eval += evaluateMaterial(board);
		eval += evaluatePieceStructure(board);
		eval += evaluatePawnStructure(board);
		eval += evaluateKingSafety(board);

		return (board.sideToMove_ == Color::white) ? eval : -eval;
	}

	int calculateUpdatedEval(Board& board)//use to get current evaluation more effectively (after evaluate() is called at least once)
	{
		int eval = 0;

		int gamePhaseForCalculations = board.gamePhase_;

		if (gamePhaseForCalculations > 24) gamePhaseForCalculations = 24;
		else if (gamePhaseForCalculations < 0) gamePhaseForCalculations = 0;

		eval = (board.mgVal_ * gamePhaseForCalculations + board.egVal_ * (24 - gamePhaseForCalculations)) / 24;

		eval += evaluatePieceStructure(board);
		eval += evaluatePawnStructure(board);
		eval += evaluateKingSafety(board);

		return (board.sideToMove_ == Color::white) ? eval : -eval;
	}

	void scoreMoves(std::vector<Move>& moves, Move ttBestMove)
	{
		for (Move& move : moves)
		{
			if (move == ttBestMove)
			{
				move.score = 2000000;
			}
			else if (move.captured != PieceType::empty)
			{
				move.score = 100000 + mgValue_[move.captured % 6] * 10 - mgValue_[move.moved % 6];
			}
			else
			{
				move.score = 0;
			}
		}
	}
}