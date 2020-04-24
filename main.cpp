#include "defs.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <execinfo.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>

std::string sqrMapIntToStr[] = {
   "A1", "B1", "C1", "D1", "E1", "F1", "G1", "H1",
   "A2", "B2", "C2", "D2", "E2", "F2", "G2", "H2",
   "A3", "B3", "C3", "D3", "E3", "F3", "G3", "H3",
   "A4", "B4", "C4", "D4", "E4", "F4", "G4", "H4",
   "A5", "B5", "C5", "D5", "E5", "F5", "G5", "H5",
   "A6", "B6", "C6", "D6", "E6", "F6", "G6", "H6",
   "A7", "B7", "C7", "D7", "E7", "F7", "G7", "H7",
   "A8", "B8", "C8", "D8", "E8", "F8", "G8", "H8"
};

// TODO add a routine to show the mapping numbers to squares or ideally ask
// user for a field and translate into int

bool pickRandomMove(chessBoard& board, move& m, int prefMv = 0)
{
    move moves[MAX_NUM_OF_MVS];
    board.numOfMvs = 0;
    board.genMoves(moves);

    // check if preferred move can be played
    if (prefMv < board.numOfMvs)
    {
        chessBoard tmpBoard = board;
        if (tmpBoard.makeMove(moves[prefMv]))
        {
            m = moves[prefMv];
            return true;
        }
    }

    for (int i = 0; i < board.numOfMvs; i++)
    {
        chessBoard tmpBoard = board;
        if (tmpBoard.makeMove(moves[i]))
        {
            m = moves[i];
            return true;
        }
    }

    return false;
}

void printMove(const move& m)
{
    std::cout << "Move from "
              << sqrMapIntToStr[(int) m.from] << " to "
              << sqrMapIntToStr[(int) m.to]  << std::endl;

    // TODO castling and promotion handling

    std::cout << std::endl << std::endl;
}

int pickSearchDepth()
{
    int searchDepth = 0;
    bool pass = false;

    while (!pass)
    {
        std::cout << "Choose depth search - a number between 1-6" << std::endl;
        std::cin >> searchDepth;

        if (searchDepth >= 1 && searchDepth <= 6)
            pass = true;
    }

    return searchDepth;
}

move getInput()
{
    int intFrom = -1, intTo = -1;
    std::string strFrom(""), strTo("");

    while (intFrom == -1 || intTo == -1)
    {
        intFrom = -1, intTo = -1;

        std::cout << "Enter a valid move by specifing origin and target"
                  << " squares separated by a space. "
                  << std::endl
                  << "For example for the initial position E2 E4 is a valid"
                  << " move. "
                  << std::endl
                  << "For castling move, enter king's origin and target"
                  << "squares e.g. E1 G1."
                  << std::endl
                  << std::endl
                  << std::endl;

        std::cin >> strFrom >> strTo;

        // if user entered lower case e.g. e2 e4 we have to convert it to
        // upper case characters
        std::transform(strFrom.begin(), strFrom.end(), strFrom.begin(),
                       ::toupper);
        std::transform(strTo.begin(), strTo.end(), strTo.begin(),
                       ::toupper);

        for ( unsigned int i=0;
                i < sizeof(sqrMapIntToStr)/sizeof(sqrMapIntToStr[0]); i++ )
        {
            if (strFrom == sqrMapIntToStr[i])
                intFrom = i;
            if (strTo == sqrMapIntToStr[i])
                intTo = i;
        }
    }

    return move(intFrom, intTo, 0);
}

int promotionPickNewPiece()
{
    int ret;
    bool pass = false;
    while (!pass)
    {
        std::cout << "Pawn promotion! What piece do you want to promote the"
                  << " pawn to ? R - rook, N - knight, B - bishop, Q - queen "
                  << std::endl;

        char c;
        std::cin >> c;

        if (c == 'R')
        {
            ret = PROMOTE_R;
            pass = true;
        }
        else if (c == 'N')
        {
            ret = PROMOTE_N;
            pass = true;
        }
        else if (c == 'B')
        {
            ret = PROMOTE_B;
            pass = true;
        }
        else if (c == 'Q')
        {
            ret = PROMOTE_Q;
            pass = true;
        }
    }

    return ret;
}

bool gameEnded(chessBoard board){

    if (board.fifty >= 100)
    {
        std::cout << "Game ended in draw by fifty move rule " << std::endl;
        return true;
    } else if (board.currPosRepeats() >= 2)
    {
        std::cout << "Game ended in draw by repetition" << std::endl;
        return true;
    }

    move moves[MAX_NUM_OF_MVS];
    board.numOfMvs = 0;
    board.genMoves(moves);

    for (int i = 0; i < board.numOfMvs; i++)
    {
        chessBoard boardTmp = board;
        if (boardTmp.makeMove(moves[i]))
        {
            return false;
        }
    }
    // TODO can add a simple routine to check if king attacked.
    // also can be used in makeMove()

    std::cout << "check mate!" << std::endl;
    return true;
}

void testMoveGenSpeed()
{
    chessBoard board;
    //std::string pos( "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" );
    std::string pos( "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -" );
    //std::string pos( "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -" );
    //std::string pos( "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1" );

    readInPosFromFEN (board, pos);
    board.populateOccup();
    board.populateCharBoard();

    move moves[MAX_NUM_OF_MVS];

    clock_t begin = clock();
    for (int j = 0; j<1000000; j++)
    {
        board.numOfMvs = 0;
        board.genMoves(moves);
    }
    clock_t end = clock();

    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    std::cout << "spent " << elapsed_secs << std::endl;
    std::cout << "number of moves  " << board.numOfMvs << std::endl;
}

void playDummyGame()
{
    chessBoard board;
    searchEngine searchEn;
    move m(0,0,0);

    std::string pos("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    readInPosFromFEN (board, pos);
    board.populateOccup();
    board.populateCharBoard();
    board.calcHash();

    int searchDepth = 3, prefMv = 25;

    board.printCharBoard();
    while (true)
    {
        if (gameEnded(board))
        {
            board.printCharBoard();
            break;
        }

        if (board.side == true)
        {
            m = searchEn.doSearch(board, searchDepth);

            std::string tmpFEN;
            savePosInFEN(board, tmpFEN);
            std::cout << "temp FEN is ... " << tmpFEN << std::endl;
        }
        else
            pickRandomMove(board, m, prefMv);

        printMove(m);
        board.makeMove(m);
        board.printCharBoard();
    }


    // TODO testing only
    // to test if searching of some positions failing
    /*board.printCharBoard();
    m = searchEn.doSearch(board, 3);
    printMove(m);
    return;*/

}

// TODO get chess puzzles and see if the engine solves them
void solveChessPuzzle(std::string pos)
{
    chessBoard board;
    searchEngine searchEn;
    move m(0,0,0);

    readInPosFromFEN (board, pos);
    board.populateOccup();
    board.populateCharBoard();
    board.calcHash();

    int searchDepth = 6;

    board.printCharBoard();
    while (true)
    {
        if (gameEnded(board))
        {
            board.printCharBoard();
            std::cout << "Game Over ... " << std::endl;
            break;
        }

        m = searchEn.doSearch(board, searchDepth);

        printMove(m);
        board.makeMove(m);
        board.printCharBoard();
    }
}


void engineVsEngine()
{
    chessBoard board;
    searchEngine searchEn;
    move m(0,0,0);

    std::string pos("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    readInPosFromFEN (board, pos);
    board.populateOccup();
    board.populateCharBoard();
    board.calcHash();

    int searchDepth = 3, prefMv = 25;

    board.printCharBoard();
    while (true)
    {
        if (gameEnded(board))
        {
            board.printCharBoard();
            break;
        }

        m = searchEn.doSearch(board, searchDepth);

        std::string tmpFEN;
        savePosInFEN(board, tmpFEN);
        std::cout << "temp FEN is ... " << tmpFEN << std::endl;

        printMove(m);
        board.makeMove(m);
        board.printCharBoard();
    }

}

// TODO user should be able to pick side
void playChess ()
{
    chessBoard board;
    searchEngine searchEn;
    move m(0,0,0);


    std::string pos("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    readInPosFromFEN (board, pos);
    board.populateOccup();
    board.populateCharBoard();
    board.calcHash();

    int searchDepth = pickSearchDepth();

    board.printCharBoard();
    while (true)
    {
        if (gameEnded(board))
        {
            board.printCharBoard();
            break;
        }

        move moves[MAX_NUM_OF_MVS];
        board.numOfMvs = 0;
        board.genMoves(moves);

        bool pass = false;
        while (!pass)
        {
            m = getInput();

            for (int i = 0; i < board.numOfMvs; i++)
            {
                if (moves[i].from == m.from && moves[i].to == m.to)
                {
                    // moves generated by genMoves are complete as they have
                    // all the fields ('from, 'to', 'flag') set. User-generated
                    //move has only 'to' and 'from' so we need to use the
                    // one by genMoves
                    m = moves[i];

                    // promotion is a special case so allow the user
                    // to choose the new piece
                    if(moves[i].flag == PROMOTE_R ||
                       moves[i].flag == PROMOTE_B ||
                       moves[i].flag == PROMOTE_N ||
                       moves[i].flag == PROMOTE_Q)
                        m.flag = promotionPickNewPiece();

                    chessBoard tmp = board;
                    if (board.makeMove(m))
                        pass = true;
                    else
                        board = tmp;
                }
            }
        }
        board.printCharBoard();

        if (gameEnded(board))
        {
            board.printCharBoard();
            break;
        }

        m = searchEn.doSearch(board, searchDepth);

        // TODO check if the move legal
        printMove(m);
        board.makeMove(m);
        board.printCharBoard();
    }
}


void handler(int sig) {
  void *array[10];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 10);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}


magics mag;



int main ()
{
    signal(SIGSEGV, handler);

    debugMoveGen();
    //testMoveGenSpeed();
    //playDummyGame();
    //engineVsEngine();
    //playChess();
    //Perft2();

    //solveChessPuzzle("r5rk/2p1Nppp/3p3P/pp2p1P1/4P3/2qnPQK1/8/R6R w - - 1 0");

    return 0;
}
