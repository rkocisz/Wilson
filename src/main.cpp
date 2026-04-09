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

void playAsWhite()
{
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
            std::cout << i << ":" << Util::piecesEmotes_[legalMoves[i].moved] << " from: " << Util::squareToNotation(legalMoves[i].startPos) << " to: " << Util::squareToNotation(legalMoves[i].endPos) << "\n";
        }

        selectedMove = -1;

        while (selectedMove < 0 || selectedMove > legalMoves.size() - 1)
        {
            std::cout << "\n podaj id ruchu:";
            std::cin >> selectedMove;
        }

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
}

void handleUCIPos(std::string line, Board& board)
{
    std::stringstream ss(line);
    std::string word;

    ss >> word;

    while (ss >> word)
    {
        if (word == "startpos")
        {
            board = Board();
        }
        else if (word == "moves" || word == "fen")
        {
            continue;
        }
        else
        {
            Move m = Util::LANToMove(word, board);
            board.makeMove(m);
        }
    }
}

void UCI()
{
    std::string line, command;
    Board board;

    while (std::getline(std::cin, line)) 
    {
        std::stringstream ss(line);
        ss >> command;

        if (command == "uci") 
        {
            std::cout << "id name Wilson v1" << std::endl;
            std::cout << "id author rkocisz" << std::endl;
            std::cout << "uciok" << std::endl;
        }
        else if (command == "isready") 
        {
            std::cout << "readyok" << std::endl;
        }
        else if (command == "position") 
        {
            handleUCIPos(line, board);
        }
        else if (command == "go")
        {
            Move bestMove = findBestMove(board, 7);
            std::cout << "bestmove " << Util::moveToLAN(bestMove) << std::endl;
        }
        else if (command == "quit") 
        {
            break;
        }
        else
        {
            std::cerr << "unknown command!";
        }
    }
}

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
    searchPerft();

    //playAsWhite();

    //UCI();

    return 0;
}