#ifndef SEARCH_H
#define SEARCH_H

#include "board.h"
#include "evaluation.h"
#include "moveGen.h"
#include "common.h"

#include <algorithm>
#include <cstddef>
#include <iostream>


int negamax(Board& board, int depth, int alpha, int beta)
{
	const uint64_t zobristKey = board.getZobristKey();
	const std::size_t TTIndex = zobristKey & TT_INDEX_MASK;
	const int originalAlpha = alpha;
	const int originalBeta = beta;


	auto& ttEntry = Util::transpositionTable[TTIndex];
	if (ttEntry.zobristKey == zobristKey && ttEntry.depth >= depth)
	{
		if (ttEntry.flag == TTFlag::exact)
		{
			return ttEntry.eval;
		}

		if (ttEntry.flag == TTFlag::lowerBound)
		{
			if (ttEntry.eval > alpha)
			{
				alpha = ttEntry.eval;
			}
		}
		else if (ttEntry.flag == TTFlag::upperBound)
		{
			if (ttEntry.eval < beta)
			{
				beta = ttEntry.eval;
			}
		}

		if (alpha >= beta)
		{
			return ttEntry.eval;
		}
	}

    if (depth == 0)
    {
		int eval1 = Eval::calculateUpdatedEval(board);
		//int eval2 = Eval::evaluate(board);

		//if (eval1 != eval2)
		//{
		//	std::cout << "NIE zgadza sie eval!!!!!!!!!!!!!!!!!!! incremental: " << eval1 << "static: " << eval2;
		//}

        return eval1;
    }

    std::vector<Move> legalMoves = MoveGen::generateLegalMoves(&board);

    if (legalMoves.empty())
    {
		int terminalEval = 0;
        if (MoveGen::isInCheck(&board))
        {
			terminalEval = MATE_EVAL - depth;
        }

		ttEntry.eval = terminalEval;
		ttEntry.depth = depth;
		ttEntry.bestMove = Move();
		ttEntry.zobristKey = zobristKey;
		ttEntry.flag = TTFlag::exact;
		return terminalEval;
    }

	Move ttMove = Move();
	if (ttEntry.zobristKey == zobristKey)
	{
		ttMove = ttEntry.bestMove;
	}
	Eval::scoreMoves(legalMoves, ttMove);


	int bestEval = -EVAL_INFINITY;
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

		if (currentEval > bestEval)
		{
			bestEval = currentEval;
			bestMove = legalMoves[i];
		}

        if (currentEval >= alpha)
        {
			alpha = currentEval;
        }

		if (alpha >= beta)
		{
			break;
		}
    }

	ttEntry.eval = bestEval;
	ttEntry.depth = depth;
	ttEntry.bestMove = bestMove;
	ttEntry.zobristKey = zobristKey;

	if (bestEval <= originalAlpha)
	{
		ttEntry.flag = TTFlag::upperBound;
	}
	else if (bestEval >= originalBeta)
	{
		ttEntry.flag = TTFlag::lowerBound;
	}
	else
	{
		ttEntry.flag = TTFlag::exact;
	}

	return bestEval;
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
