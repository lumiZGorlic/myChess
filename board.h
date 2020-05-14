
#ifndef BOARD_H
#define BOARD_H

#include "defs.h"

#include <map>
#include <vector>
#include <bitset>

struct pieceValue {

    std::map<char,int> values;

    int lookupValue(char p){
        // assert
        return values[p];
    }

    pieceValue() {
        values['R']=5; values['r']=5;
        values['B']=3; values['b']=3;
        values['N']=3; values['n']=3;
        values['Q']=9; values['q']=9;
        values['P']=1; values['p']=1;
        values['K']=10; values['k']=10;
    }
};


// TODO: if use char here then need to convert char to int and vice-versa
// is it good solution ?
struct move {

    char from, to, flag;
    int score;
    move(char f, char t, char fl)
        : from(f), to(t), flag(fl), score(0)
    { }
    move()
        : from(0), to(0), flag(0), score(0)
    { }
};

struct chessBoard {

    U64 occ, white_Occ, black_Occ,
        wKing_Occ, wQueen_Occ, wRook_Occ, wBishop_Occ, wKnight_Occ, wPawn_Occ,
        bKing_Occ, bQueen_Occ, bRook_Occ, bBishop_Occ, bKnight_Occ, bPawn_Occ;

    // ' ' - empty square, 'k' -  black king, 'N' - white knight etc
    char charBoard[64];

    // true = white, false = black
    bool side;
    std::bitset<4> castlRights;
    int ep;
    int fifty;

    unsigned long long int currPosHash;
    std::vector<unsigned long long int> historyHash;

    int numOfMvs;


    chessBoard();
    unsigned long long int genHash();

    void populateOccup ();
    U64* getBitBoard(char c);
    void populateCharBoardHelper(U64 b, char c);
    // TODO: should be inline ? also could be moved to readIn func so that
    // everyting is in one place
    void populateCharBoard();
    void printCharBoard ();
    void calcHash();
    int currPosRepeats();

    // TODO
    // check occupancy so that it makes sense, for debugging purposes
    /*bool checkOccup () {
    }*/

    void calcAttackSqrRook(move* mvs, int sqr);
    void calcAttackSqrBishop(move* mvs, int sqr);
    void calcAttackSqrQueen(move* mvs, int sqr);
    void calcAttackSqrKnight(move* mvs, int sqr);
    void calcAttackSqrKing(move* mvs, int sqr);
    void calcAttackSqrWhitePawn(move* mvs, int sqr);
    void calcAttackSqrBlackPawn(move* mvs, int sqr);
    void calcMoveSqrBlackPawn(move* mvs, int sqr);
    void calcMoveSqrWhitePawn(move* mvs, int sqr);

    void genAgMvsHelper(move* mvs, int sqr, U64 attackedSqrs);
    void genAggressiveMoves(move* mvs);

    void genMoves(move* mvs);
    bool makeMove(move mv);

    int squareWithKing(bool sideToFind);
    bool isUnderAttack(int sqr, bool sideAttacking);
    bool isInCheck(bool sideToFind);
};


// TODO: is it enough ? what about castling etc ?
extern unsigned long long int squaresHash[64][6][2];
extern unsigned long long int sideHash;
extern unsigned long long int epHash[64];

extern pieceValue pValue;


#endif
