#include "defs.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <execinfo.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>

std::string sqrMapIntToStr2[] = {
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

bool pickRandomMove(chessBoard& board, move& m)
{   
    std::vector<move> moves;
    std::vector<move>::iterator mvsIt;
    board.genMoves(moves);

    for (mvsIt = moves.begin(); mvsIt != moves.end(); mvsIt++)
    {
        chessBoard tmpBoard = board;
        if (tmpBoard.makeMove(*mvsIt))
        {
            m = *mvsIt;
            return true;
        }
    }

    return false;
}

void printMove(const move& m)
{
    std::cout << "Move from " << sqrMapIntToStr2[(int) m.from]
              << " to " << sqrMapIntToStr2[(int) m.to]  << std::endl;

    // TODO castling and promotion handling

    std::cout << std::endl << std::endl;
}

int pickSearchDepth()
{
    int searchDepth = 0;
    bool pass = false;

    while (!pass)
    {
        std::cout << "Choose depth search - a number between 1-6 " << std::endl;
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

        std::cout << "Enter a valid move by specifing origin and target squares separated by a space. "<< std::endl
                  << "For example for the initial position E2 E4 is a valid move. "<< std::endl
                  << "For castling move, enter king's origin and target squares e.g. E1 G1."<< std::endl
                  << std::endl << std::endl;

        std::cin >> strFrom >> strTo;

        for ( unsigned int i=0; i < sizeof(sqrMapIntToStr2)/sizeof(sqrMapIntToStr2[0]); i++ )
        {
            // if user entered lower case e.g. e2 e4 we have to convert it to
            // upper case characters
            std::transform(strFrom.begin(), strFrom.end(), strFrom.begin(),
                           ::toupper);
            std::transform(strTo.begin(), strTo.end(), strTo.begin(),
                           ::toupper);

            if (strFrom == sqrMapIntToStr2[i])
                intFrom = i;
            else if (strTo == sqrMapIntToStr2[i])
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
        std::cout << "Pawn promotion! What piece do you want to promote the pawn to ? "
                  << "R - rook, N - knight, B - bishop, Q - queen " << std::endl;

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

    std::vector<move> mvs;
    std::vector<move>::iterator mvsIt;

    board.genMoves(mvs);
    for (mvsIt = mvs.begin(); mvsIt != mvs.end(); mvsIt++)
    {
        // TODO debug
        //std::cout << sqrMapIntToStr2[((int)(*mvsIt).from)] << ", " << sqrMapIntToStr2[((int)(*mvsIt).to)] << std::endl;

        chessBoard boardTmp = board;
        if (boardTmp.makeMove(*mvsIt))
        {
            return false;
        }
    }
    // TODO can add a simple routine to check if king attacked.
    // also can be used in makeMove()

    std::cout << "check mate!" << std::endl;
    return true;
}

void playChess ()
{
    chessBoard board;
    searchEngine searchEn;
    move m(0,0,0);

    // TODO testing only
    //std::string pos("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    //std::string pos ("Bnb2bnr/2k1PQ1p/4p3/4P3/3N1B2/PRN5/P1P2PPR/7K w - 0 1");
    std::string pos ("k7/3N4/8/K7/8/8/8/8 w - 0 1");

    readInPosFromFEN (board, pos);
    board.populateOccup();
    board.populateCharBoard();
    board.calcHash();


    // TODO testing only
    board.printCharBoard();
    m = searchEn.doSearch(board, 2);
    printMove(m);
    return;


    int searchDepth = pickSearchDepth();

    board.printCharBoard();
    while (true)
    {
        /*std::vector<move> mvs;
        std::vector<move>::iterator it;
        board.genMoves(mvs);
        // check if game over 

        bool pass = false;
        while (!pass)
        {
            m = getInput();

            for (it = mvs.begin(); it != mvs.end(); it++)
            {
                if (it->from == m.from && it->to == m.to)
                {
                    // moves generated by genMoves are complete as they have all
                    // the fields ('from, 'to', 'flag') set. User-generated move
                    // has only 'to' and 'from' so need to use the one by genMoves 
                    m = *it;

                    // promotion is a special case so allow the user
                    // to choose the new piece
                    if (it->flag == PROMOTE_R || it->flag == PROMOTE_B ||
                        it->flag == PROMOTE_N || it->flag == PROMOTE_Q)
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

        mvs.clear();
        board.genMoves(mvs);
        // check if game over
        */

        if (gameEnded(board))
        {
            board.printCharBoard();
            break;
        }

        // TODO experiment with searchDepth - e.g flip between values so
        // that one side stronger and no draw
        if (board.side == true)
        {
            m = searchEn.doSearch(board, searchDepth);

            // TODO testing only
            std::string tmpFEN;
            savePosInFEN(board, tmpFEN);
            std::cout << "temp FEN is ... " << tmpFEN << std::endl;
        }
        else
            pickRandomMove(board, m);

        //m = searchEn.doSearch(board, searchDepth);


        // TODO check if the move legal
        board.makeMove(m);
        //printMove(m);
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

int main ()
{
    signal(SIGSEGV, handler);

    //int* foo = (int*)-1;
    //printf("%d\n", *foo);       // causes segfault

    debugMoveGen();
    //playChess();


    /*char c; int i;
    std::cin >> i;
    c = i;
    std::cout << c << std::endl;
    std::cout << (int)c << std::endl;*/

    /*chessBoard board;
    // initial position
    std::string pos("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    readInPosFromFEN (board, pos);
    board.populateOccup();
    board.populateCharBoard();
    board.printCharBoard();*/

    /*std::cout << eval(board) << std::endl;

    move m = searchMain(board, 3);
    std::cout << "from " << sqrMapIntToStr2[(int)m.from]
              << " to " << sqrMapIntToStr2[(int)m.to] << std::endl;*/

    return 0;
}
