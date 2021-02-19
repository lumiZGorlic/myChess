#include "defs.h"
#include "board.h"
#include "magics.h"
#include "misc.h"

#include <map>
#include <vector>
#include <bitset>
#include <random>
#include <iostream>
#include <fstream>

// TODO: should int be used for square or some other type
// TODO: int should be unsigned or not ? why ?
// TODO: pass reference to const where appropriate
// TODO: also some functions should be const e.g. genMoves etc
// TODO: think about what should be inline and what shouldn't
// TODO identify if sth can be passed by ref/pnt rather than copied etc

// TODO investigate why this needs to be global etc
std::mt19937 mt(01234567);

pieceValue pValue;

unsigned long long int squaresHash[64][6][2];
unsigned long long int sideHash;
unsigned long long int epHash[64];

// opening book stuff
std::vector<std::string> openings;
std::string playedMoves;


chessBoard::chessBoard() :
    occ(0),
    white_Occ(0), black_Occ(0),
    wKing_Occ(0), wQueen_Occ(0), wRook_Occ(0), wBishop_Occ(0),
    wKnight_Occ(0), wPawn_Occ(0),
    bKing_Occ(0), bQueen_Occ(0), bRook_Occ(0), bBishop_Occ(0),
    bKnight_Occ(0), bPawn_Occ(0),
    side(true),
    castlRights(std::string("1111")),
    ep(-1),
    fifty(0),
    currPosHash(0),
    numOfMvs(0)
{
    for (int i = A1; i <= H8; i++)
        charBoard[i] = ' ';

    for (int i = A1; i <= H8; i++)
        for (int j = 0; j <= 5; j++)
            for (int k = 0; k <=1; k++)
                squaresHash[i][j][k] = genHash();

    for (int i = A1; i <= H8; i++)
        epHash[i] = genHash();

    sideHash = genHash();
}


bool chessBoard::isUnderAttack(int sqr, bool sideAttacking){

    U64 boardKing = 0;
    setBit(boardKing, sqr);

    for (int sqr=A1; sqr<=H8; sqr++)
    {
        if(charBoard[sqr] == ' ')
        {
            continue;
        }

        if (sideAttacking == true)
        {
            if(charBoard[sqr] == 'R')
            {
                U64 blockers = occ & mag.rookMask[sqr];
                int shift = 32 - mag.RBits[sqr];
                int key = (unsigned)((int)blockers * (int)mag.rookMagics[sqr]
                        ^ (int)(blockers>>32)*(int)(mag.rookMagics[sqr]>>32))
                        >> shift;
                U64 attackedSqrs = mag.rookAttackArr[sqr][key];
                if (attackedSqrs & boardKing)
                    return true;
            }
            else if(charBoard[sqr] == 'N')
            {
                U64 attackedSqrs = mag.knightAttackArr[sqr];
                if (attackedSqrs & boardKing)
                    return true;
            }
            else if(charBoard[sqr] == 'B')
            {
                U64 blockers = occ & mag.bishopMask[sqr];
                int shift = 32 - mag.BBits[sqr];
                int key = (unsigned)((int)blockers * (int)mag.bishopMagics[sqr]
                        ^ (int)(blockers>>32)*(int)(mag.bishopMagics[sqr]>>32))
                        >> shift;
                U64 attackedSqrs = mag.bishopAttackArr[sqr][key];
                if (attackedSqrs & boardKing)
                    return true;
            }
            else if(charBoard[sqr] == 'Q')
            {
                U64 blockers = occ & mag.rookMask[sqr];
                int shift = 32 - mag.RBits[sqr];
                int key = (unsigned)((int)blockers * (int)mag.rookMagics[sqr]
                        ^ (int)(blockers>>32) * (int)(mag.rookMagics[sqr]>>32))
                        >> shift;
                U64 attackedSqrs = mag.rookAttackArr[sqr][key];
                if (attackedSqrs & boardKing)
                    return true;

                blockers = occ & mag.bishopMask[sqr];
                shift = 32 - mag.BBits[sqr];
                key = (unsigned)((int)blockers * (int)mag.bishopMagics[sqr] ^
                      (int)(blockers>>32) * (int)(mag.bishopMagics[sqr]>>32))
                      >> shift;
                attackedSqrs = mag.bishopAttackArr[sqr][key];
                if (attackedSqrs & boardKing)
                    return true;
            }
            else if(charBoard[sqr] == 'K')
            {
                U64 attackedSqrs = mag.kingAttackArr[sqr];
                if (attackedSqrs & boardKing)
                    return true;
            }
            else if(charBoard[sqr] == 'P')
            {
                U64 attackedSqrs = mag.wPawnAttackArr[sqr];
                if (attackedSqrs & boardKing)
                    return true;
            }
        }
        else
        {
            if(charBoard[sqr] == 'r')
            {
                U64 blockers = occ & mag.rookMask[sqr];
                int shift = 32 - mag.RBits[sqr];
                int key = (unsigned)((int)blockers * (int)mag.rookMagics[sqr]
                        ^ (int)(blockers>>32) * (int)(mag.rookMagics[sqr]>>32))
                        >> shift;
                U64 attackedSqrs = mag.rookAttackArr[sqr][key];
                if (attackedSqrs & boardKing)
                    return true;
            }
            else if(charBoard[sqr] == 'n')
            {
                U64 attackedSqrs = mag.knightAttackArr[sqr];
                if (attackedSqrs & boardKing)
                    return true;
            }
            else if(charBoard[sqr] == 'b')
            {
                U64 blockers = occ & mag.bishopMask[sqr];
                int shift = 32 - mag.BBits[sqr];
                int key = (unsigned)((int)blockers * (int)mag.bishopMagics[sqr]
                        ^ (int)(blockers>>32)*(int)(mag.bishopMagics[sqr]>>32))
                        >> shift;
                U64 attackedSqrs = mag.bishopAttackArr[sqr][key];
                if (attackedSqrs & boardKing)
                    return true;
            }
            else if(charBoard[sqr] == 'q')
            {
                U64 blockers = occ & mag.rookMask[sqr];
                int shift = 32 - mag.RBits[sqr];
                int key = (unsigned)((int)blockers * (int)mag.rookMagics[sqr]
                        ^ (int)(blockers>>32) * (int)(mag.rookMagics[sqr]>>32))
                        >> shift;
                U64 attackedSqrs = mag.rookAttackArr[sqr][key];
                if (attackedSqrs & boardKing)
                    return true;

                blockers = occ & mag.bishopMask[sqr];
                shift = 32 - mag.BBits[sqr];
                key = (unsigned)((int)blockers * (int)mag.bishopMagics[sqr]
                    ^ (int)(blockers>>32) * (int)(mag.bishopMagics[sqr]>>32))
                    >> shift;
                attackedSqrs = mag.bishopAttackArr[sqr][key];
                if (attackedSqrs & boardKing)
                    return true;
            }
            else if(charBoard[sqr] == 'k')
            {
                U64 attackedSqrs = mag.kingAttackArr[sqr];
                if (attackedSqrs & boardKing)
                    return true;
            }
            else if(charBoard[sqr] == 'p')
            {
                U64 attackedSqrs = mag.bPawnAttackArr[sqr];
                if (attackedSqrs & boardKing)
                    return true;
            }
        }
    }

    return false;
}

void chessBoard::genMovesHelper(move* mvs, int sqr, U64 attackedSqrs)
{
    int dist = 0;
    while (attackedSqrs)
    {
        int frwd = mag.bitscanForward(attackedSqrs);
        attackedSqrs >>= (frwd+1);
        dist += frwd+1;

        mvs[numOfMvs].from = sqr;
        mvs[numOfMvs].to = dist-1;
        mvs[numOfMvs].flag = 0;

        if (charBoard[dist-1] != ' ')
            mvs[numOfMvs].score = 10 * pValue.lookupValue(charBoard[dist-1]) -
                                  pValue.lookupValue(charBoard[sqr]);

        numOfMvs++;

        if (frwd == 63)
            attackedSqrs = 0;
    }
}

void chessBoard::genAggressiveMoves(move* mvs)
{
    // TODO need to generate a bitmap for a king to see if some
    // moves result in a check
    /* int kingSqr = ....

      U64 checkKingFrom = attacks for queen, knight, pawn from the kingSqr
      minus occupancy
    */

    U64 attackedSqrs = 0;
    for (int sqr=A1; sqr<=H8; sqr++)
    {
        if(charBoard[sqr] == ' ')
        {
            continue;
        }

        attackedSqrs = 0;
        if (side == true)
        {
            if(charBoard[sqr] == 'R')
            {
                U64 blockers = occ & mag.rookMask[sqr];
                int shift = 32 - mag.RBits[sqr];
                int key = (unsigned)((int)blockers * (int)mag.rookMagics[sqr]
                        ^ (int)(blockers>>32) * (int)(mag.rookMagics[sqr]>>32))
                        >> shift;
                attackedSqrs = mag.rookAttackArr[sqr][key];
            }
            else if(charBoard[sqr] == 'N')
            {
                attackedSqrs = mag.knightAttackArr[sqr];
            }
            else if(charBoard[sqr] == 'B')
            {
                U64 blockers = occ & mag.bishopMask[sqr];
                int shift = 32 - mag.BBits[sqr];
                int key = (unsigned)((int)blockers * (int)mag.bishopMagics[sqr]
                        ^ (int)(blockers>>32)*(int)(mag.bishopMagics[sqr]>>32))
                        >> shift;
                attackedSqrs = mag.bishopAttackArr[sqr][key];
            }
            else if(charBoard[sqr] == 'Q')
            {
                U64 blockers = occ & mag.rookMask[sqr];
                int shift = 32 - mag.RBits[sqr];
                int key = (unsigned)((int)blockers * (int)mag.rookMagics[sqr]
                        ^ (int)(blockers>>32) * (int)(mag.rookMagics[sqr]>>32))
                        >> shift;
                attackedSqrs = mag.rookAttackArr[sqr][key];

                blockers = occ & mag.bishopMask[sqr];
                shift = 32 - mag.BBits[sqr];
                key = (unsigned)((int)blockers * (int)mag.bishopMagics[sqr] ^
                      (int)(blockers>>32) * (int)(mag.bishopMagics[sqr]>>32))
                      >> shift;
                attackedSqrs |= mag.bishopAttackArr[sqr][key];
            }
            else if(charBoard[sqr] == 'K')
            {
                attackedSqrs = mag.kingAttackArr[sqr];
            }
            else if(charBoard[sqr] == 'P')
            {
                calcAttackSqrWhitePawn(mvs, sqr);
            }

            if (attackedSqrs)
            {
                attackedSqrs &= ~white_Occ;
                attackedSqrs &= black_Occ;
                genMovesHelper(mvs, sqr, attackedSqrs);
            }
        }
        else
        {
            if(charBoard[sqr] == 'r')
            {
                U64 blockers = occ & mag.rookMask[sqr];
                int shift = 32 - mag.RBits[sqr];
                int key = (unsigned)((int)blockers * (int)mag.rookMagics[sqr]
                        ^ (int)(blockers>>32) * (int)(mag.rookMagics[sqr]>>32))
                        >> shift;
                attackedSqrs = mag.rookAttackArr[sqr][key];
            }
            else if(charBoard[sqr] == 'n')
            {
                attackedSqrs = mag.knightAttackArr[sqr];
            }
            else if(charBoard[sqr] == 'b')
            {
                U64 blockers = occ & mag.bishopMask[sqr];
                int shift = 32 - mag.BBits[sqr];
                int key = (unsigned)((int)blockers * (int)mag.bishopMagics[sqr]
                        ^ (int)(blockers>>32)*(int)(mag.bishopMagics[sqr]>>32))
                       >> shift;
                attackedSqrs = mag.bishopAttackArr[sqr][key];
            }
            else if(charBoard[sqr] == 'q')
            {
                U64 blockers = occ & mag.rookMask[sqr];
                int shift = 32 - mag.RBits[sqr];
                int key = (unsigned)((int)blockers * (int)mag.rookMagics[sqr]
                        ^ (int)(blockers>>32) * (int)(mag.rookMagics[sqr]>>32))
                        >> shift;
                attackedSqrs = mag.rookAttackArr[sqr][key];

                blockers = occ & mag.bishopMask[sqr];
                shift = 32 - mag.BBits[sqr];
                key = (unsigned)((int)blockers * (int)mag.bishopMagics[sqr]
                        ^ (int)(blockers>>32)*(int)(mag.bishopMagics[sqr]>>32))
                        >> shift;
                attackedSqrs |= mag.bishopAttackArr[sqr][key];
            }
            else if(charBoard[sqr] == 'k')
            {
                attackedSqrs = mag.kingAttackArr[sqr];
            }
            else if(charBoard[sqr] == 'p')
            {
                calcAttackSqrBlackPawn(mvs, sqr);
            }

            if (attackedSqrs)
            {
                attackedSqrs &= ~black_Occ;
                attackedSqrs &= white_Occ;
                genMovesHelper(mvs, sqr, attackedSqrs);
            }
        }
    }
}

unsigned long long int chessBoard::genHash()
{
    // taken from the article on zobrist hashing on geeksforgeeks
    std::uniform_int_distribution<unsigned long long int> dist(0, UINT64_MAX);
    return dist(mt);
}

void chessBoard::calcHash()
{
    currPosHash = 0;

    int sideInt = (side == true) ? 0 : 1;
    for  (int i = A1; i <= H8; i++)
    {
        if (charBoard[i] == ' ')
            continue;

        if (charBoard[i] == 'R' || charBoard[i] == 'r')
            currPosHash ^= squaresHash[i][0][sideInt];
        else if (charBoard[i] == 'N' || charBoard[i] == 'n')
            currPosHash ^= squaresHash[i][1][sideInt];
        else if (charBoard[i] == 'B' || charBoard[i] == 'b')
            currPosHash ^= squaresHash[i][2][sideInt];
        else if (charBoard[i] == 'Q' || charBoard[i] == 'q')
            currPosHash ^= squaresHash[i][3][sideInt];
        else if (charBoard[i] == 'K' || charBoard[i] == 'k')
            currPosHash ^= squaresHash[i][4][sideInt];
        else if (charBoard[i] == 'P' || charBoard[i] == 'p')
            currPosHash ^= squaresHash[i][5][sideInt];
    }
    currPosHash ^= sideHash;
    if (ep != -1)
        currPosHash ^= epHash[ep];

    historyHash.push_back(currPosHash);
}

// TODO not sure if this func used ok in search.
// also should make use of fifty so the loop shorter
int chessBoard::currPosRepeats()
{
    int ret = 0;
    for (int i = 0; i < (int)historyHash.size(); i++)
        if (historyHash[i] == currPosHash)
            ret++;

    return ret;
}

void chessBoard::populateOccup () {
    white_Occ = wKing_Occ | wQueen_Occ | wRook_Occ | wBishop_Occ | wKnight_Occ
              | wPawn_Occ;
    black_Occ = bKing_Occ | bQueen_Occ | bRook_Occ | bBishop_Occ | bKnight_Occ
              | bPawn_Occ;
    occ = white_Occ | black_Occ;
}

void chessBoard::populateCharBoardHelper(U64 b, char c) {

    for (int i = A1; i <= H8; i++)
    {
        if((b >> i) & 1)
            charBoard[i] = c;
    }
}

// TODO: should be inline ? also could be moved to readIn func so that
// everyting is in one place
void chessBoard::populateCharBoard() {
    populateCharBoardHelper(wRook_Occ, 'R');
    populateCharBoardHelper(wKnight_Occ, 'N');
    populateCharBoardHelper(wBishop_Occ, 'B');
    populateCharBoardHelper(wQueen_Occ, 'Q');
    populateCharBoardHelper(wKing_Occ, 'K');
    populateCharBoardHelper(wPawn_Occ, 'P');

    populateCharBoardHelper(bRook_Occ, 'r');
    populateCharBoardHelper(bKnight_Occ, 'n');
    populateCharBoardHelper(bBishop_Occ, 'b');
    populateCharBoardHelper(bQueen_Occ, 'q');
    populateCharBoardHelper(bKing_Occ, 'k');
    populateCharBoardHelper(bPawn_Occ, 'p');

}

void chessBoard::printCharBoard ()
{
    int viewMap[64] = {
               A8, B8, C8, D8, E8, F8, G8, H8,
               A7, B7, C7, D7, E7, F7, G7, H7,
               A6, B6, C6, D6, E6, F6, G6, H6,
               A5, B5, C5, D5, E5, F5, G5, H5,
               A4, B4, C4, D4, E4, F4, G4, H4,
               A3, B3, C3, D3, E3, F3, G3, H3,
               A2, B2, C2, D2, E2, F2, G2, H2,
               A1, B1, C1, D1, E1, F1, G1, H1  };

    for (int i = 0; i <= 63; i++)
    {
        char piece = charBoard[viewMap[i]];
        if (piece == ' ')
            std::cout << ".";
        else
            std::cout << piece;

        if (i % 8 == 7)
            std::cout << std::endl;
    }

    std::cout << std::endl << std::endl;
}

int chessBoard::squareWithKing(bool sideToFind)
{
    int ret = -1;

    for (int i=A1; i<=H8; i++)
    {
        if (charBoard[i] == ' ')
            continue;

        if (sideToFind == true && charBoard[i] == 'K')
        {
            ret = i;
            break;
        }
        else if (sideToFind == false && charBoard[i] == 'k')
        {
            ret = i;
            break;
        }
    }

    // assert
    return ret;
}

bool chessBoard::isInCheck(bool sideToFind){

    int sqrKing = squareWithKing(sideToFind);

    if (isUnderAttack(sqrKing, !sideToFind))
        return true;

    return false;
}

void chessBoard::calcAttackSqrRook(move* mvs, int sqr)
{
    U64 blockers = occ & mag.rookMask[sqr];

    // TODO read about why two ways to calculate the key
    //U64 key = (blockers * rookMagics[square]) >> (64 - rookBits[square]);

    int shift = 32 - mag.RBits[sqr];
    int key = (unsigned)((int)blockers * (int)mag.rookMagics[sqr]
            ^ (int)(blockers>>32) * (int)(mag.rookMagics[sqr]>>32)) >> shift;
    U64 attackedSqrs = mag.rookAttackArr[sqr][key];

    //assert (attackedSqrs == (doAttackN(occ, sqr) | doAttackS(occ, sqr) |
    //                doAttackW(occ, sqr) | doAttackE(occ, sqr)) )

    if (side == true)
        attackedSqrs &= ~white_Occ;
    else
        attackedSqrs &= ~black_Occ;

    genMovesHelper(mvs, sqr, attackedSqrs);
}

void chessBoard::calcAttackSqrBishop(move* mvs, int sqr)
{
    U64 blockers = occ & mag.bishopMask[sqr];

    // TODO read about why two ways to calculate the key
    //U64 key = (blockers * bishopMagics[square]) >> (64 - bishopBits[square]);

    int shift = 32 - mag.BBits[sqr];
    int key = (unsigned)((int)blockers * (int)mag.bishopMagics[sqr]
            ^ (int)(blockers>>32) * (int)(mag.bishopMagics[sqr]>>32)) >> shift;
    U64 attackedSqrs = mag.bishopAttackArr[sqr][key];

    //assert( attackedSqrs == (doAttackNE(occ, sqr) | doAttackNW(occ, sqr) |
    //                doAttackSE(occ, sqr) | doAttackSW(occ, sqr)) );


    if (side == true)
        attackedSqrs &= ~white_Occ;
    else
        attackedSqrs &= ~black_Occ;

    genMovesHelper(mvs, sqr, attackedSqrs);
}

void chessBoard::calcAttackSqrQueen(move* mvs, int sqr)
{
    calcAttackSqrRook(mvs, sqr);
    calcAttackSqrBishop(mvs, sqr);
}

void chessBoard::calcAttackSqrKnight(move* mvs, int sqr)
{
    U64 attackedSqrs = mag.knightAttackArr[sqr];

    if (side == true)
        attackedSqrs &= ~white_Occ;
    else
        attackedSqrs &= ~black_Occ;

    genMovesHelper(mvs, sqr, attackedSqrs);
}

void chessBoard::calcAttackSqrKing(move* mvs, int sqr)
{
    U64 attackedSqrs = mag.kingAttackArr[sqr];

    if (side == true)
        attackedSqrs &= ~white_Occ;
    else
        attackedSqrs &= ~black_Occ;

    genMovesHelper(mvs, sqr, attackedSqrs);

    if (side == true)
    {
        // castling
        U64 kingRookGap = 0;
        if (castlRights[0] == 1)
        {
            kingRookGap = 0;
            setBit(kingRookGap, F1);
            setBit(kingRookGap, G1);

            if ((occ & kingRookGap) == false)
            {
                mvs[numOfMvs].from = E1;
                mvs[numOfMvs].to = G1;
                mvs[numOfMvs].flag = CASTLING;
                numOfMvs++;
            }
        }

        if (castlRights[1] == 1)
        {
            kingRookGap = 0;
            setBit(kingRookGap, B1);
            setBit(kingRookGap, C1);
            setBit(kingRookGap, D1);
            if ((occ & kingRookGap) == false)
            {
                mvs[numOfMvs].from = E1;
                mvs[numOfMvs].to = C1;
                mvs[numOfMvs].flag = CASTLING;
                numOfMvs++;
            }
        }
    }
    else
    {
        // castling
        U64 kingRookGap = 0;
        if (castlRights[2] == 1)
        {
            kingRookGap = 0;
            setBit(kingRookGap, F8);
            setBit(kingRookGap, G8);

            if ((occ & kingRookGap) == false)
            {
                mvs[numOfMvs].from = E8;
                mvs[numOfMvs].to = G8;
                mvs[numOfMvs].flag = CASTLING;
                numOfMvs++;
            }
        }

        if (castlRights[3] == 1)
        {
            kingRookGap = 0;
            setBit(kingRookGap, B8);
            setBit(kingRookGap, C8);
            setBit(kingRookGap, D8);
            if ((occ & kingRookGap) == false)
            {
                mvs[numOfMvs].from = E8;
                mvs[numOfMvs].to = C8;
                mvs[numOfMvs].flag = CASTLING;
                numOfMvs++;
            }
        }
    }
}

void chessBoard::calcAttackSqrWhitePawn(move* mvs, int sqr)
{
    U64 attackedSqrs = mag.wPawnAttackArr[sqr];
    attackedSqrs &= black_Occ;

    int dist = 0;
    while (attackedSqrs)
    {
        int frwd = mag.bitscanForward(attackedSqrs);
        attackedSqrs >>= frwd+1;
        dist += frwd+1;

        if (rank(dist-1) == 7)
        {
            for (int i=PROMOTE_R; i<=PROMOTE_Q; i++)
            {
                mvs[numOfMvs].from = sqr;
                mvs[numOfMvs].to = dist-1;
                mvs[numOfMvs].flag = i;

                if (charBoard[dist-1] != ' ')
                    mvs[numOfMvs].score = 10 *
                                          pValue.lookupValue(charBoard[dist-1])
                                          - pValue.lookupValue(charBoard[sqr]);

                numOfMvs++;
            }
        }
        else
        {
            mvs[numOfMvs].from = sqr;
            mvs[numOfMvs].to = dist-1;
            mvs[numOfMvs].flag = 0;

            if (charBoard[dist-1] != ' ')
                mvs[numOfMvs].score = 10 *
                                      pValue.lookupValue(charBoard[dist-1])
                                      - pValue.lookupValue(charBoard[sqr]);

            numOfMvs++;
        }

        if (frwd == 63)
            attackedSqrs = 0;
    }

    attackedSqrs = 1;
    if (ep != -1)
        if( (attackedSqrs << ep) & mag.wPawnAttackArr[sqr] )
        {
            mvs[numOfMvs].from = sqr;
            mvs[numOfMvs].to = ep;
            mvs[numOfMvs].flag = EP;

            mvs[numOfMvs].score = 9;

            numOfMvs++;
        }

}

void chessBoard::calcAttackSqrBlackPawn(move* mvs, int sqr)
{
    U64 attackedSqrs = mag.bPawnAttackArr[sqr];
    attackedSqrs &= white_Occ;

    int dist = 0;
    while (attackedSqrs)
    {
        int frwd = mag.bitscanForward(attackedSqrs);
        attackedSqrs >>= frwd+1;
        dist += frwd+1;

        if (rank(dist-1) == 0)
        {
            for (int i=PROMOTE_R; i<=PROMOTE_Q; i++)
            {
                mvs[numOfMvs].from = sqr;
                mvs[numOfMvs].to = dist-1;
                mvs[numOfMvs].flag = i;

                if (charBoard[dist-1] != ' ')
                    mvs[numOfMvs].score = 10 *
                                          pValue.lookupValue(charBoard[dist-1])
                                          - pValue.lookupValue(charBoard[sqr]);

                numOfMvs++;
            }
        }
        else
        {
            mvs[numOfMvs].from = sqr;
            mvs[numOfMvs].to = dist-1;
            mvs[numOfMvs].flag = 0;

            if (charBoard[dist-1] != ' ')
                mvs[numOfMvs].score = 10 *
                                      pValue.lookupValue(charBoard[dist-1])
                                      - pValue.lookupValue(charBoard[sqr]);

            numOfMvs++;
        }

        if (frwd == 63)
            attackedSqrs = 0;
    }

    attackedSqrs = 1;
    if (ep != -1)
        if( (attackedSqrs << ep) & mag.bPawnAttackArr[sqr] )
        {
            mvs[numOfMvs].from = sqr;
            mvs[numOfMvs].to = ep;
            mvs[numOfMvs].flag = EP;

            mvs[numOfMvs].score = 9;

            numOfMvs++;
        }
}

void chessBoard::calcMoveSqrWhitePawn(move* mvs, int sqr)
{
    // TODO could add some checks
    //if ( rankFileOK(rank(sqr) + 1) )

    int destSqr = square(rank(sqr) + 1, file(sqr));
    // TODO: read about if 'if + return' advisable
    if ((occ >> destSqr) & 1)
        return;

    if (rank(destSqr) == 7)
    {
        for (int i=PROMOTE_R; i<=PROMOTE_Q; i++)
        {
            mvs[numOfMvs].from = sqr;
            mvs[numOfMvs].to = destSqr;
            mvs[numOfMvs].flag = i;
            numOfMvs++;
        }
    }
    else
    {
        mvs[numOfMvs].from = sqr;
        mvs[numOfMvs].to = destSqr;
        mvs[numOfMvs].flag = 0;
        numOfMvs++;
    }

    if (rank(sqr) == 1)
    {
        destSqr = square(3, file(sqr));
        if ( !((occ >> destSqr) & 1) )
        {
            mvs[numOfMvs].from = sqr;
            mvs[numOfMvs].to = destSqr;
            mvs[numOfMvs].flag = 0;
            numOfMvs++;
        }
    }
}

void chessBoard::calcMoveSqrBlackPawn(move* mvs, int sqr)
{
    // TODO could add some checks
    //if ( rankFileOK(rank(sqr) - 1) )

    int destSqr = square(rank(sqr) - 1, file(sqr));
    // TODO: read about if 'if + return' advisable
    if ((occ >> destSqr) & 1)
        return;

    if (rank(destSqr) == 0)
    {
        for (int i=PROMOTE_R; i<=PROMOTE_Q; i++)
        {
            mvs[numOfMvs].from = sqr;
            mvs[numOfMvs].to = destSqr;
            mvs[numOfMvs].flag = i;
            numOfMvs++;
        }
    }
    else
    {
        mvs[numOfMvs].from = sqr;
        mvs[numOfMvs].to = destSqr;
        mvs[numOfMvs].flag = 0;
        numOfMvs++;
    }

    if (rank(sqr) == 6)
    {
        destSqr = square(4, file(sqr));
        if ( !((occ >> destSqr) & 1) )
        {
            mvs[numOfMvs].from = sqr;
            mvs[numOfMvs].to = destSqr;
            mvs[numOfMvs].flag = 0;
            numOfMvs++;
        }
    }
}

void chessBoard::genMoves(move* moves)
{
    for (int i=A1; i<=H8; i++)
    {
        if (charBoard[i] == ' ')
            continue;

        if (side == true)
        {
            switch (charBoard[i])
            {
                case 'R':
                    calcAttackSqrRook(moves, i);
                    break;
                case 'N':
                    calcAttackSqrKnight(moves, i);
                    break;
                case 'B':
                    calcAttackSqrBishop(moves, i);
                    break;
                case 'Q':
                    calcAttackSqrQueen(moves, i);
                    break;
                case 'K':
                    calcAttackSqrKing(moves, i);
                    break;
                case 'P':
                    calcMoveSqrWhitePawn(moves, i);
                    calcAttackSqrWhitePawn(moves, i);
                    break;
            }
        }
        else
        {
            switch (charBoard[i])
            {
                case 'r':
                    calcAttackSqrRook(moves, i);
                    break;
                case 'n':
                    calcAttackSqrKnight(moves, i);
                    break;
                case 'b':
                    calcAttackSqrBishop(moves, i);
                    break;
                case 'q':
                    calcAttackSqrQueen(moves, i);
                    break;
                case 'k':
                    calcAttackSqrKing(moves, i);
                    break;
                case 'p':
                    calcMoveSqrBlackPawn(moves, i);
                    calcAttackSqrBlackPawn(moves, i);
                    break;
            }
        }
    }
}

U64* chessBoard::getBitBoard(char c)
{
    // assert for c

    U64* ret=0;
    switch (c)
    {
        case 'R' :
            ret = &wRook_Occ;
        break;
        case 'N' :
            ret = &wKnight_Occ;
        break;
        case 'B' :
            ret = &wBishop_Occ;
        break;
        case 'Q' :
            ret = &wQueen_Occ;
        break;
        case 'K' :
            ret = &wKing_Occ;
        break;
        case 'P' :
            ret = &wPawn_Occ;
        break;

        case 'r' :
            ret = &bRook_Occ;
        break;
        case 'n' :
            ret = &bKnight_Occ;
        break;
        case 'b' :
            ret = &bBishop_Occ;
        break;
        case 'q' :
            ret = &bQueen_Occ;
        break;
        case 'k' :
            ret = &bKing_Occ;
        break;
        case 'p' :
            ret = &bPawn_Occ;
        break;

        default:
        break;
    }

    return ret;
}

bool chessBoard::makeMove(move mv){

    int from = (int)mv.from, to = (int)mv.to;

    if (charBoard[from] == 'P' || charBoard[to] != ' ')
        fifty = 0;
    else
        fifty++;

    U64 tmp = 0;
    setBit(tmp, from);
    tmp = ~tmp;

    U64* updt = getBitBoard(charBoard[from]);
    (*updt) &= tmp;
    setBit(*updt, to);

    updt = &occ;
    (*updt) &= tmp;
    setBit(*updt, to);

    if (side == true)
        updt = &white_Occ;
    else
        updt = &black_Occ;
    (*updt) &= tmp;
    setBit(*updt, to);

    if (mv.flag == EP)
    {
       if (side == true)
       {
           tmp = 0;
           setBit(tmp, to-8);
           tmp = ~tmp;

           updt = &bPawn_Occ;
           (*updt) &= tmp;

           updt = &black_Occ;
           (*updt) &= tmp;

           updt = &occ;
           (*updt) &= tmp;
       }
       else
       {
           tmp = 0;
           setBit(tmp, to+8);
           tmp = ~tmp;

           updt = &wPawn_Occ;
           (*updt) &= tmp;

           updt = &white_Occ;
           (*updt) &= tmp;

           updt = &occ;
           (*updt) &= tmp;
       }
    }
    else if (mv.flag == CASTLING)
    {
        int rookFrom = 0, rookTo = 0;
        if (to == C1)
        {
            rookFrom = A1, rookTo = D1;
        }
        else if (to == G1)
        {
            rookFrom = H1, rookTo = F1;
        }
        else if (to == C8)
        {
            rookFrom = A8, rookTo = D8;
        }
        else if (to == G8)
        {
            rookFrom = H8, rookTo = F8;
        }

        tmp = 0;
        setBit(tmp, rookFrom);
        tmp = ~tmp;

        if (side == true)
        {
            updt =  &wRook_Occ;
            (*updt) &= tmp;
            setBit(*updt, rookTo);

            updt = &white_Occ;
            (*updt) &= tmp;
            setBit(*updt, rookTo);
        }
        else
        {
            updt =  &bRook_Occ;
            (*updt) &= tmp;
            setBit(*updt, rookTo);

            updt = &black_Occ;
            (*updt) &= tmp;
            setBit(*updt, rookTo);
        }

        updt = &occ;
        (*updt) &= tmp;
        setBit(*updt, rookTo);
    }
    else
    {
        if (charBoard[to] != ' ')
        {
            tmp = 0;
            setBit(tmp, to);
            tmp = ~tmp;

            updt = getBitBoard(charBoard[to]);
            (*updt) &= tmp;

            if (side == true)
            {
                updt = &black_Occ;
                (*updt) &= tmp;
            }
            else
            {
                updt = &white_Occ;
                (*updt) &= tmp;
            }
        }
    }

    if (mv.flag == PROMOTE_R || mv.flag == PROMOTE_N
        || mv.flag == PROMOTE_B  || mv.flag == PROMOTE_Q )
    {
        tmp = 0;
        setBit(tmp, to);
        tmp = ~tmp;

        if (side == true)
            wPawn_Occ &= tmp;
        else
            bPawn_Occ &= tmp;

        if (side == true)
        {
            if (mv.flag == PROMOTE_R)
                updt = &wRook_Occ;
            else if (mv.flag == PROMOTE_N)
                updt = &wKnight_Occ;
            else if (mv.flag == PROMOTE_B)
                updt = &wBishop_Occ;
            else if (mv.flag == PROMOTE_Q)
                updt = &wQueen_Occ;
        }
        else
        {
            if (mv.flag == PROMOTE_R)
                updt = &bRook_Occ;
            else if (mv.flag == PROMOTE_N)
                updt = &bKnight_Occ;
            else if (mv.flag == PROMOTE_B)
                updt = &bBishop_Occ;
            else if (mv.flag == PROMOTE_Q)
                updt = &bQueen_Occ;
        }
        setBit(*updt, to);
    }

    charBoard[to] = charBoard[from];
    charBoard[from] = ' ';

    if (mv.flag == EP)
    {
       if (side == true)
           charBoard[to-8] = ' ';
       else
           charBoard[to+8] = ' ';
    }
    else if (mv.flag == CASTLING)
    {
        if (to == C1)
        {
            charBoard[A1] = ' '; charBoard[D1] = 'R';
        }
        else if (to == G1)
        {
            charBoard[H1] = ' '; charBoard[F1] = 'R';
        }
        else if (to == C8)
        {
            charBoard[A8] = ' '; charBoard[D8] = 'r';
        }
        else if (to == G8)
        {
            charBoard[H8] = ' '; charBoard[F8] = 'r';
        }
    }
    else if (mv.flag == PROMOTE_R)
    {
        if (side == true)
            charBoard[to] = 'R';
        else
            charBoard[to] = 'r';
    }
    else if (mv.flag == PROMOTE_N)
    {
        if (side == true)
            charBoard[to] = 'N';
        else
            charBoard[to] = 'n';
    }
    else if (mv.flag == PROMOTE_B)
    {
        if (side == true)
            charBoard[to] = 'B';
        else
            charBoard[to] = 'b';
    }
    else if (mv.flag == PROMOTE_Q)
    {
        if (side == true)
            charBoard[to] = 'Q';
        else
            charBoard[to] = 'q';
    }

    if (charBoard[to] == 'P' && to - from == 16)
        ep = to - 8;
    else if (charBoard[to] == 'p' && to - from == -16)
        ep = to + 8;
    else
        ep = -1;

    // TODO: can probably make use of side here
    if (charBoard[to] == 'K')
        castlRights[0] = castlRights[1] = false;
    else if (charBoard[to] == 'k')
        castlRights[2] = castlRights[3] = false;

    // TODO: simplify it using charBoard
    if (castlRights[0] == true && (from == H1 || to == H1))
        castlRights[0] = false;
    else if (castlRights[1] == true && (from == A1 || to == A1))
        castlRights[1] = false;

    if (castlRights[2] == true && (from == H8 || to == H8))
        castlRights[2] = false;
    else if (castlRights[3] == true && (from == A8 || to == A8))
        castlRights[3] = false;

    bool ret = true;

    if (isInCheck(side))
        ret = false;

    // TODO: a bit of a hack with pawns below so should think of a
    // better way of doing it
    if (mv.flag == CASTLING)
    {
        if ((int)mv.to == C1)
        {
            if (isUnderAttack(E1, !side) || isUnderAttack(D1, !side) ||
                charBoard[C2] == 'p' || charBoard[D2] == 'p' ||
                charBoard[E2] == 'p' || charBoard[F2] == 'p' )
                ret = false;
        }
        else if ((int)mv.to == G1)
        {
            if (isUnderAttack(E1, !side) || isUnderAttack(F1, !side) ||
                charBoard[G2] == 'p' || charBoard[D2] == 'p' ||
                charBoard[E2] == 'p' || charBoard[F2] == 'p' )
                ret = false;
        }
        else if ((int)mv.to == C8)
        {
            if (isUnderAttack(E8, !side) || isUnderAttack(D8, !side) ||
                charBoard[C7] == 'P' || charBoard[D7] == 'P' ||
                charBoard[E7] == 'P' || charBoard[F7] == 'P' )
                ret = false;
        }
        else if ((int)mv.to == G8)
        {
            if (isUnderAttack(E8, !side) || isUnderAttack(F8, !side) ||
                charBoard[G7] == 'P' || charBoard[D7] == 'P' ||
                charBoard[E7] == 'P' || charBoard[F7] == 'P' )
                ret = false;
        }
    }

    side = !side;
    calcHash();

    return ret;
}

// opening book stuff

std::string intToStr[] = {
   "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
   "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
   "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
   "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
   "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
   "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
   "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
   "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8"
};

void addToPlayedMoves(const move& m){
    std::string ms = intToStr[(int)m.from] + intToStr[(int)m.to];

    if (!playedMoves.empty()) playedMoves += ' ';
    playedMoves += ms;
}

void openBook(){
    std::ifstream f("book.txt");
    std::string line;

    while (std::getline(f, line)){
        if (line.substr(0,2) != "--")
            openings.push_back(line);
    }
}

move tryBook(){
    std::vector<std::string> possibleMoves;

    int sz=playedMoves.size();

    for (std::string o: openings){
        if (o.size() > sz && o.substr(0, sz) == playedMoves){
            if (!sz)
                possibleMoves.push_back(o.substr(sz, 4));
            else
                possibleMoves.push_back(o.substr(sz+1, 4));
        }
    }

    sz = possibleMoves.size();
    // return invalid move
    if (!sz) return move(-1, -1, 0);

    srand((unsigned)time(0));
    int randomIdx = (rand()%sz);
    std::string ms = possibleMoves[randomIdx];

    std::string from = ms.substr(0,2), to = ms.substr(2,2);
    int fromI = -1, toI = -1;

    for (int i = 0; i < sizeof(intToStr)/sizeof(intToStr[0]); i++)
        if (from == intToStr[i]) fromI = i;
        else if (to == intToStr[i]) toI = i;

    return move(fromI, toI, 0);
}
