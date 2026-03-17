#include <windows.h>
#include <bitset>
#include <vector>
#include <algorithm>

#include "tests.h"
#include "board.h"
#include "util.h"
#include "moveGenUtil.h"
#include "evaluation.h"
#include "search.h"

int main()
{
    SetConsoleOutputCP(CP_UTF8);

    MoveGenUtil::init();
    Eval::init();

    //testMoveAndUnmakeMove();
    //testZobristKeyGeneration();
    //thriceRepetitionTest();
    //perft();
    //testEval();

    Board board;
    board = Board();
    int selectedMove = 0;

    while (board.gameState_ == GameState::playing)
    {
        board.draw();

        std::vector<Move> legalMoves = MoveGen::generateLegalMoves(&board);
        if (legalMoves.size() == 0)
        {
            std::cout << "koniec" << board.gameState_;
            break;
        }
        
        std::cout << "\ndostêpne ruchy:\n";
        for (int i = 0; i < legalMoves.size(); i++)
        {
            std::cout  << i << ":" << Util::piecesEmotes_[legalMoves[i].moved] << " from: " << Util::squareToNotation(legalMoves[i].startPos) << " to: " << Util::squareToNotation(legalMoves[i].endPos) << "\n";
        }

        std::cout << "\n podaj id ruchu:";
        std::cin >> selectedMove;

        board.makeMove(legalMoves[selectedMove]);

        board.draw();
        
        legalMoves = MoveGen::generateLegalMoves(&board);

        if (legalMoves.size() == 0)
        {
            std::cout << "koniec" << board.gameState_;
            break;
        }

        Move bestMove = findBestMove(board, 5);
     
        board.makeMove(bestMove);

        std::cout << "\n";
    }

    return 0;
}