#include <windows.h>
#include <bitset>
#include <vector>

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

    while (board.gameState_ == GameState::playing)
    {
        std::vector<Move> legalMoves = MoveGen::generateLegalMoves(&board);
        
        std::cout << "dostêpne ruchy:";
        for (int i = 0; i < legalMoves.size(); i++)
        {
            std::cout  << i << ":" << Util::piecesEmotes_[legalMoves[i].moved] << " from: " << legalMoves[i].startPos << " to: " << legalMoves[i].endPos << "\n";
        }
    }

    return 0;
}