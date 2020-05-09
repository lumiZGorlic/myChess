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

    int searchDepth = 4;

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

    int searchDepth = 3;

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


// after bench() in tscp
void testSearch(){

    chessBoard board;
    //std::string pos( "rnbqkbnr/8/8/8/8/8/8/RNBQKBNR w KQkq - 0 1" );
    std::string pos( "1rb2rk1/p4ppp/1p1qp1n1/3n2N1/2pP4/2P3P1/PPQ2PBP/R1B1R1K1 w - - 0 1" );

    readInPosFromFEN (board, pos);
    board.populateOccup();
    board.populateCharBoard();
    board.calcHash();

    for (int i = 0; i < 5; i++)
    {
        clock_t begin = clock();

        searchEngine se;
        move m = se.doSearch(board, i+1);
        //move m = se.doSearch(board, 2);

        clock_t end = clock();
        double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

        std::cout << "time elapsed is: " << elapsed_secs << std::endl;
        std::cout << "number of nodes: " << se.nodes << std::endl;
        printMove(m);
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
pieceValue pValue;
transpositionTable tt;
unsigned long long int squaresHash[64][6][2];
unsigned long long int sideHash;
unsigned long long int epHash[64];

int main ()
{
    signal(SIGSEGV, handler);

    //testSearch();

    debugMoveGen();
    //testMoveGenSpeed();
    //playDummyGame();
    //engineVsEngine();
    //playChess();
    //Perft2();


    // the below stuff from https://www.sparkchess.com/mate-in/two
    // can be used to check if the best combinations are found. Should create
    // a separate function for it

    //solveChessPuzzle("r5rk/2p1Nppp/3p3P/pp2p1P1/4P3/2qnPQK1/8/R6R w - - 1 0");
    //solveChessPuzzle("kbK5/pp6/1P6/8/8/8/8/R7 w - -");

    //solveChessPuzzle("r2qk2r/pb4pp/1n2Pb2/2B2Q2/p1p5/2P5/2B2PPP/RN2R1K1 w - - 1 0");
    //solveChessPuzzle("6k1/pp4p1/2p5/2bp4/8/P5Pb/1P3rrP/2BRRN1K b - - 0 1");
    //solveChessPuzzle("8/2k2p2/2b3p1/P1p1Np2/1p3b2/1P1K4/5r2/R3R3 b - - 0 1");
    //solveChessPuzzle("6k1/5p2/1p5p/p4Np1/5q2/Q6P/PPr5/3R3K w - - 1 0");
    //solveChessPuzzle("r1b2k1r/ppppq3/5N1p/4P2Q/4PP2/1B6/PP5P/n2K2R1 w - - 1 0");
    //solveChessPuzzle("r1b2k1r/ppp1bppp/8/1B1Q4/5q2/2P5/PPP2PPP/R3R1K1 w - - 1 0");
    //solveChessPuzzle("rn2kb1r/ppp1pppp/8/8/4q3/3P1N1b/PPP1BPnP/RNBQ1K1R b kq - 0 1");
    //solveChessPuzzle("kbK5/pp6/1P6/8/8/8/8/R7 w - -");
    //solveChessPuzzle("4r1k1/pQ3pp1/7p/4q3/4r3/P7/1P2nPPP/2BR1R1K b - - 0 1");
    //solveChessPuzzle("r4R2/1b2n1pp/p2Np1k1/1pn5/4pP1P/8/PPP1B1P1/2K4R w - - 1 0");
    //solveChessPuzzle("7r/p3ppk1/3p4/2p1P1Kp/2Pb4/3P1QPq/PP5P/R6R b - - 0 1");
    //solveChessPuzzle("4r3/pbpn2n1/1p1prp1k/8/2PP2PB/P5N1/2B2R1P/R5K1 w - - 1 0");

    /*
    "r2qk2r/pb4pp/1n2Pb2/2B2Q2/p1p5/2P5/2B2PPP/RN2R1K1 w - - 1 0"  1. Qg6+ hxg6 2. Bxg6#
    "6k1/pp4p1/2p5/2bp4/8/P5Pb/1P3rrP/2BRRN1K b - - 0 1"  1... Rg1+ 2. Kxg1 Rxf1#
    "8/2k2p2/2b3p1/P1p1Np2/1p3b2/1P1K4/5r2/R3R3 b - - 0 1" 1... Bb5+ 2. Nc4 Rd2#
    "6k1/5p2/1p5p/p4Np1/5q2/Q6P/PPr5/3R3K w - - 1 0"  1. Qf8+ Kxf8 2. Rd8#
    "r1b2k1r/ppppq3/5N1p/4P2Q/4PP2/1B6/PP5P/n2K2R1 w - - 1 0" 1. Qxh6+ Rxh6 2. Rg8#
    "r1b2k1r/ppp1bppp/8/1B1Q4/5q2/2P5/PPP2PPP/R3R1K1 w - - 1 0" 1. Qd8+ Bxd8 2. Re8#
    "rn2kb1r/ppp1pppp/8/8/4q3/3P1N1b/PPP1BPnP/RNBQ1K1R b kq - 0 1" 1... Nh4+ 2. Ke1 Nxf3#
    "kbK5/pp6/1P6/8/8/8/8/R7 w - -" 1. Ra6 bxa6 2. b7#
    "4r1k1/pQ3pp1/7p/4q3/4r3/P7/1P2nPPP/2BR1R1K b - - 0 1"  1... Qxh2+ 2. Kxh2 Rh4#
    "r4R2/1b2n1pp/p2Np1k1/1pn5/4pP1P/8/PPP1B1P1/2K4R w - - 1 0" 1. h5+ Kh6 2. Nf7#
    "7r/p3ppk1/3p4/2p1P1Kp/2Pb4/3P1QPq/PP5P/R6R b - - 0 1" 1... Be3+ 2. Qxe3 Qg4#
    "4r3/pbpn2n1/1p1prp1k/8/2PP2PB/P5N1/2B2R1P/R5K1 w - - 1 0" 1. Rxf6+ Nxf6 2. g5#

    */


    return 0;
}
