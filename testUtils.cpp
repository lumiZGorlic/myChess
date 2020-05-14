
#include "defs.h"
#include "testUtils.h"
#include "board.h"
#include "search.h"
#include "misc.h"

#include <vector>
#include <iostream>

U64 perft(chessBoard& board, int depth)
{
    U64 nodes = 0;

    if (depth == 0)
        return 1;

    move moves[MAX_NUM_OF_MVS];
    board.numOfMvs = 0;
    board.genMoves(moves);

    chessBoard boardCpy = board;

    for (int i = 0; i < board.numOfMvs; i++)
    {
        if (board.makeMove(moves[i]))
        {
            nodes += perft(board, depth - 1);
        }

        board = boardCpy;
    }

    return nodes;
}

U64 perft2()
{
    chessBoard board;

    std::string pos("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    readInPosFromFEN (board, pos);
    board.populateOccup();
    board.populateCharBoard();

    int depth = 4;
    std::cin >> depth;

    clock_t begin = clock();

    int res = perft(board, depth);

    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

    std::cout << "spent " << elapsed_secs
              << " at depth " << depth << std::endl;
    std::cout << "res is: " << res << std::endl;

    return res;
}

void debugMoveGen () {

    std::vector<debugItem> debugInput;


    // same positions, not as many moves
    /*debugInput.push_back(debugItem("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
                 {20, 400, 8902, 197281 }));
    debugInput.push_back(debugItem("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -",
                 { 48, 2039, 97862 }));
    debugInput.push_back(debugItem("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -",
                 { 14, 191, 2812, 43238, 674624 }));
    debugInput.push_back(debugItem("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
                 { 6, 264, 9467, 422333 }));
    debugInput.push_back(debugItem("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8 ",
                 { 44, 1486, 62379, 2103487 }));
    debugInput.push_back(debugItem("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10",
                 { 46, 2079, 89890, 3894594 }));*/


    debugInput.push_back(debugItem("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
                 {20, 400, 8902, 197281, 4865609, 119060324 }));
    debugInput.push_back(debugItem("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -",
                 { 48, 2039, 97862, 4085603, 193690690 }));
    debugInput.push_back(debugItem("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -",
                 { 14, 191, 2812, 43238, 674624, 11030083, 178633661 }));
    debugInput.push_back(debugItem("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
                 { 6, 264, 9467, 422333, 15833292, 706045033 }));
    debugInput.push_back(debugItem("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8 ",
                 { 44, 1486, 62379, 2103487, 89941194 }));
    debugInput.push_back(debugItem("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10",
                 { 46, 2079, 89890, 3894594, 164075551 }));

    std::vector<debugItem>::iterator it;
    for (it = debugInput.begin(); it != debugInput.end(); it++)
    {
        chessBoard board;

        readInPosFromFEN (board, it->pos);
        board.populateOccup();
        board.populateCharBoard();
        //board.printCharBoard();

        int max = (it->perftResults).size();
        for (int i = 0; i < max; i++)
        {
            clock_t begin = clock();

            U64 ret = perft(board, i+1);

            if(ret != (it->perftResults)[i])
                std::cout << "perft failed for position: " << it->pos
                          << " at depth " << i+1
                          << " ,should be " << (it->perftResults)[i]
                          << " but returned " << ret << std::endl;

            clock_t end = clock();
            double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
            std::cout << "spent " << elapsed_secs << " for " << it->pos
                      << " at depth " << i+1 << std::endl;
        }
    }
}


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

bool pickRandomMove(chessBoard& board, move& m, int prefMv)
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

                    chessBoard boardCpy = board;
                    if (board.makeMove(m))
                        pass = true;
                    else
                        board = boardCpy;
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


