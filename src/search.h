#ifndef SEARCH_H
#define SEARCH_H

#include "board.h"
#include "evaluation.h"
#include "moveGen.h"
#include "common.h"

#include <algorithm>
#include <iostream>


int negamax(Board& board, int depth, int alpha, int beta)
{
	int zobristKey = board.getZobristKey();
	int TTIndex = zobristKey & TT_INDEX_MASK;


	if (Util::transpositionTable[TTIndex].zobristKey == zobristKey && Util::transpositionTable[TTIndex].depth >= depth)
	{
		return Util::transpositionTable[TTIndex].eval;
	}

    if (depth == 0)
    {
        return Eval::calculateUpdatedEval(board);
    }

    std::vector<Move> legalMoves = MoveGen::generateLegalMoves(&board);

    if (legalMoves.empty())
    {
        if (MoveGen::isInCheck(&board))
        {
			Util::transpositionTable[TTIndex].eval = MATE_EVAL - depth;
			Util::transpositionTable[TTIndex].depth = INT64_MAX;
            return MATE_EVAL - depth;
        }
		else
		{
			Util::transpositionTable[TTIndex].eval = 0;
			Util::transpositionTable[TTIndex].depth = INT64_MAX;
			return 0;
		}
    }

	Move ttMove = Move();
	if (Util::transpositionTable[TTIndex].zobristKey == zobristKey)
	{
		ttMove = Util::transpositionTable[TTIndex].bestMove;
	}
	Eval::scoreMoves(legalMoves, ttMove);


	int maxEval = -EVAL_INFINITY;
	Move bestMove = Move();

    for (int i = 0; i < legalMoves.size(); i++)
    {
		int bestMoveIndex = i;

		for (int j = i + 1; j < legalMoves.size(); j++)
		{
			if (legalMoves[j].score > legalMoves[bestMoveIndex].score)
			{
				bestMoveIndex = j;
			}
		}

		std::swap(legalMoves[i], legalMoves[bestMoveIndex]);

        board.makeMove(legalMoves[i]);

		int currentEval = 0;

		if (board.isPositionRepeatedThrice())
		{
			currentEval = 0;
		}
		else
		{
			currentEval = -negamax(board, depth - 1, -beta, -alpha);
		}

        board.unmakeMove(legalMoves[i]);

		if (currentEval > maxEval)
		{
			maxEval = currentEval;
			bestMove = legalMoves[i];
		}

        if (currentEval >= beta)
        {
			auto& entry = Util::transpositionTable[TTIndex];
			entry.eval = currentEval;
			entry.depth = depth;
			entry.bestMove = bestMove;
			entry.zobristKey = zobristKey;

            return beta;
        }

        if (currentEval > alpha)
        {
            alpha = currentEval;
        }
    }

	auto& entry = Util::transpositionTable[TTIndex];
	entry.eval = alpha;
	entry.depth = depth;
	entry.bestMove = bestMove;
	entry.zobristKey = zobristKey;

    return alpha;
}

Move findBestMove(Board& board, int depth)
{
	int maxEval = -EVAL_INFINITY;
	Move bestMove = Move();

	std::vector<Move> legalMoves = MoveGen::generateLegalMoves(&board);

	if (legalMoves.size() == 0)
	{
		if (MoveGen::isInCheck(&board))
		{
			board.gameState_ = (board.sideToMove_ == Color::white) ? GameState::winBlack : GameState::winWhite;
			return bestMove;
		}
		else
		{
			board.gameState_ = GameState::draw;
			return bestMove;
		}
	}

	for (const Move& move : legalMoves)
	{
		board.makeMove(move);
		int currentEval = -negamax(board, depth - 1, -EVAL_INFINITY, EVAL_INFINITY);

		board.unmakeMove(move);

		if(currentEval > maxEval)
		{
			maxEval = currentEval;
			bestMove = move;
		}
	}

	return bestMove;
}

#endif
