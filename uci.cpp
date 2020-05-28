#include "defs.h"
#include "uci.h"
#include "board.h"
#include "search.h"
#include "misc.h"

#include <iostream>
#include <sstream>


std::string mp[] = {
   "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
   "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
   "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
   "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
   "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
   "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
   "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
   "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8"
};


move tokenToMove(const chessBoard& board, const std::string& token){
    // assert token's size etc

    std::string from = token.substr(0,2), to = token.substr(2,2);
    int fromI, toI;
    char flag = 0;

    for (int i = 0; i < sizeof(mp)/sizeof(mp[0]); i++)
        if (from == mp[i]) fromI = i;
        else if (to == mp[i]) toI = i;

    if(tolower(board.charBoard[fromI]) == 'k' && abs(int(fromI)-int(toI)) == 2)
        flag = CASTLING;

    else if (tolower(board.charBoard[fromI]) == 'p' && toI == board.ep)
        flag = EP;

    else if (tolower(board.charBoard[fromI]) == 'p' && (toI > 8 || toI < 55))
    {
        switch (token[4]) {
            case 'q':
                flag = PROMOTE_Q;
                break;
            case 'r':
                flag = PROMOTE_R;
                break;
            case 'b':
                flag = PROMOTE_B;
                break;
            case 'n':
                flag = PROMOTE_N;
            break;
        }
    }

    return move(fromI, toI, flag);
}

void go(chessBoard& board, std::istringstream& is){

    // TODO here we're setting fixed time limit 10 secs per search
    // need to write sth smarter, read wtime, btime from 'is'
    // and calculate based on that
    searchEngine searchEn(10.0);
    move m = searchEn.doSearch(board, 25);

    std::string flag("");
    switch (m.flag) {
        case PROMOTE_Q:
            flag = "q";
            break;
        case PROMOTE_R:
            flag = "r";
            break;
        case PROMOTE_B:
            flag = "b";
            break;
        case PROMOTE_N:
            flag = "n";
            break;
    }

    std::string moveS = mp[(int)m.from] + mp[(int)m.to] + flag;
    std::cout << "bestmove " << moveS << std::endl;
}

void position(chessBoard& board, std::istringstream& is){
    std::string startFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    std::string token, fen;

    is >> token;

    if (token == "startpos")
    {
        fen = startFEN;
        is >> token;
    }
    else if (token == "fen")
        while (is >> token && token != "moves")
            fen += token + " ";
    else
        return;

    // no better way to clear board now
    board = chessBoard();
    readInPosFromFEN (board, fen);
    board.populateOccup();
    board.populateCharBoard();
    board.calcHash();

    while (is >> token)
    {
        move m = tokenToMove(board, token);
        board.makeMove(m);
    }
}

void uciLoop()
{
    chessBoard board;
    std::string token, cmd;

    do {
        if (!getline(std::cin, cmd))
            cmd = "quit";

        std::istringstream is(cmd);

        token.clear();
        is >> std::skipws >> token;

        if (token == "quit" || token == "stop")
            return;

        else if (token == "go")
            go(board, is);

        else if (token == "position")
            position(board, is);

        else if (token == "isready")
            std::cout << "readyok" << std::endl;

        else
            std::cout << "uknown command " << cmd << std::endl;

    } while (true);
}
