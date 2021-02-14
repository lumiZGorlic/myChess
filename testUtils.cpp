#include "defs.h"
#include "testUtils.h"
#include "board.h"
#include "search.h"
#include "misc.h"

#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>

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


void bratkoKopec()
{
    std::string sqrMapIntToStr2[] = {
       "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
       "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
       "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
       "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
       "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
       "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
       "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
       "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8"
    };

    std::string positions[] = {
        "rnbqkb1r/p3pppp/1p6/2ppP3/3N4/2P5/PPP1QPPP/R1B1KB1R w KQkq -",
        "3r1k2/4npp1/1ppr3p/p6P/P2PPPP1/1NR5/5K2/2R5 w - -",
        "2q1rr1k/3bbnnp/p2p1pp1/2pPp3/PpP1P1P1/1P2BNNP/2BQ1PRK/7R b - -",
        "rnbqkb1r/p3pppp/1p6/2ppP3/3N4/2P5/PPP1QPPP/R1B1KB1R w KQkq -",
        "r1b2rk1/2q1b1pp/p2ppn2/1p6/3QP3/1BN1B3/PPP3PP/R4RK1 w - -",
        "2r3k1/pppR1pp1/4p3/4P1P1/5P2/1P4K1/P1P5/8 w - -",
        "1nk1r1r1/pp2n1pp/4p3/q2pPp1N/b1pP1P2/B1P2R2/2P1B1PP/R2Q2K1 w - -",
        "4b3/p3kp2/6p1/3pP2p/2pP1P2/4K1P1/P3N2P/8 w - -",
        "2kr1bnr/pbpq4/2n1pp2/3p3p/3P1P1B/2N2N1Q/PPP3PP/2KR1B1R w - -",
        "3rr1k1/pp3pp1/1qn2np1/8/3p4/PP1R1P2/2P1NQPP/R1B3K1 b - -",
        "2r1nrk1/p2q1ppp/bp1p4/n1pPp3/P1P1P3/2PBB1N1/4QPPP/R4RK1 w - -",
        "r3r1k1/ppqb1ppp/8/4p1NQ/8/2P5/PP3PPP/R3R1K1 b - -",
        "r2q1rk1/4bppp/p2p4/2pP4/3pP3/3Q4/PP1B1PPP/R3R1K1 w - -",
        "rnb2r1k/pp2p2p/2pp2p1/q2P1p2/8/1Pb2NP1/PB2PPBP/R2Q1RK1 w - -",
        "2r3k1/1p2q1pp/2b1pr2/p1pp4/6Q1/1P1PP1R1/P1PN2PP/5RK1 w - -",
        "r1bqkb1r/4npp1/p1p4p/1p1pP1B1/8/1B6/PPPN1PPP/R2Q1RK1 w kq -",
        "r2q1rk1/1ppnbppp/p2p1nb1/3Pp3/2P1P1P1/2N2N1P/PPB1QP2/R1B2RK1 b - -",
        "r1bq1rk1/pp2ppbp/2np2p1/2n5/P3PP2/N1P2N2/1PB3PP/R1B1QRK1 b - -",
        "3rr3/2pq2pk/p2p1pnp/8/2QBPP2/1P6/P5PP/4RRK1 b - -",
        "r4k2/pb2bp1r/1p1qp2p/3pNp2/3P1P2/2N3P1/PPP1Q2P/2KRR3 w - -",
        "3rn2k/ppb2rpp/2ppqp2/5N2/2P1P3/1P5Q/PB3PPP/3RR1K1 w - -",
        "2r2rk1/1bqnbpp1/1p1ppn1p/pP6/N1P1P3/P2B1N1P/1B2QPP1/R2R2K1 b - -",
        "r1bqk2r/pp2bppp/2p5/3pP3/P2Q1P2/2N1B3/1PP3PP/R4RK1 b kq -",
        "r2qnrnk/p2b2b1/1p1p2pp/2pPpp2/1PP1P3/PRNBB3/3QNPPP/5RK1 w - -"
    };

    std::string bestMoves[] = {
        "d6 d1",
        "d4 d5",
        "f6 f5",
        "e5 e6",
        "c3 d5",
        "g5 g6",
        "h5 f6",
        "f4 f5",
        "f4 f5",
        "c6 e5",
        "f2 f4",
        "d7 f5",
        "b2 b4",
        "d1 d2",
        "g4 g7",
        "d2 e4",
        "h7 h5",
        "c5 b3",
        "e8 e4",
        "g3 g4",
        "f5 h6",
        "b7 e4",
        "f7 f6",
        "f2 f4"
    };

    for (int i = 0; i < sizeof(bestMoves)/sizeof(bestMoves[0]); i++){
        chessBoard board;
        searchEngine searchEn;
        move m(0,0,0);

        readInPosFromFEN (board, positions[i]);
        board.populateOccup();
        board.populateCharBoard();
        board.calcHash();

        int searchDepth = 10;

        //board.printCharBoard();

        m = searchEn.doSearch(board, searchDepth);

        std::cout << "moves found and best are: "
              << sqrMapIntToStr2[(int) m.from] << " "
              << sqrMapIntToStr2[(int) m.to]
              << "  /  "
              << bestMoves[i] << std::endl;
    }
}

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
}

int pickSearchDepth()
{
    int searchDepth = 0;
    std::string input;
    while (1)
    {
        std::cout << "Choose depth search - a number between 1-10" << std::endl;
        getline(std::cin, input);
        std::stringstream(input) >> searchDepth;
        if (searchDepth >= 1 && searchDepth <= 10)
            return searchDepth;
    }

    return 0;
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
                  << "For castling move, enter king's origin and target "
                  << "squares e.g. E1 G1."
                  << std::endl
                  << std::endl
                  << std::endl;

        std::string input;
        getline(std::cin, input);

        if (
            (input.size() < 5) ||
            (input[2] != ' ')  ||
            !(input[0] >= 'a' && input[0] <= 'h') ||
            !(input[3] >= 'a' && input[3] <= 'h') ||
            !(input[1] >= '1' && input[1] <= '8') ||
            !(input[4] >= '1' && input[4] <= '8')
        )
        {
            continue;
        }

       strFrom = input.substr(0, 2);
       strTo = input.substr(3, 2);

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
                    if (board.makeMove(m)){
                        pass = true;
                    }
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
