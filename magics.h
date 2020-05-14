
#ifndef MAGICS_H
#define MAGICS_H

#include "defs.h"

#include <set>

struct magics {

    /*

    EXPL:
    occupLines and occupDiags contain all the possible occupancy arrays
    excluding the border squares, so for 4x4 occupLines[4] will have the
    following ('x' signifies the staring square and is set to '0'):

      0 0 0 0
      1 0 0 0
      x 0 0 0
      0 0 0 0

      0 0 0 0
      1 0 0 0
      x 1 0 0
      0 0 0 0

      0 0 0 0
      1 0 0 0
      x 1 1 0
      0 0 0 0

      0 0 0 0
      1 0 0 0
      x 0 1 0
      0 0 0 0

      0 0 0 0
      0 0 0 0
      x 0 0 0
      0 0 0 0

      0 0 0 0
      0 0 0 0
      x 1 0 0
      0 0 0 0

      0 0 0 0
      0 0 0 0
      x 1 1 0
      0 0 0 0

      0 0 0 0
      0 0 0 0
      x 0 1 0
      0 0 0 0

    */
    std::set<U64> occupLines[64];
    std::set<U64> occupDiags[64];


    U64 diagNW[64], diagNE[64], diagSW[64], diagSE[64];
    U64 lineN[64],  lineS[64],  lineW[64],  lineE[64];


    int RBits[64] = {
      12, 11, 11, 11, 11, 11, 11, 12,
      11, 10, 10, 10, 10, 10, 10, 11,
      11, 10, 10, 10, 10, 10, 10, 11,
      11, 10, 10, 10, 10, 10, 10, 11,
      11, 10, 10, 10, 10, 10, 10, 11,
      11, 10, 10, 10, 10, 10, 10, 11,
      11, 10, 10, 10, 10, 10, 10, 11,
      12, 11, 11, 11, 11, 11, 11, 12
    };

    U64 rookMask[64];
    U64 rookAttackArr[64][4096];
    U64 rookMagics[64];


    int BBits[64] = {
      6, 5, 5, 5, 5, 5, 5, 6,
      5, 5, 5, 5, 5, 5, 5, 5,
      5, 5, 7, 7, 7, 7, 5, 5,
      5, 5, 7, 9, 9, 7, 5, 5,
      5, 5, 7, 9, 9, 7, 5, 5,
      5, 5, 7, 7, 7, 7, 5, 5,
      5, 5, 5, 5, 5, 5, 5, 5,
      6, 5, 5, 5, 5, 5, 5, 6
    };

    U64 bishopMask[64];
    U64 bishopAttackArr[64][4096];
    U64 bishopMagics[64];


    U64 knightAttackArr[64];
    U64 kingAttackArr[64];
    U64 wPawnAttackArr[64];
    U64 bPawnAttackArr[64];


    magics();

    void saveInFile();
    void readFromFile();

    U64 random_U64();
    U64 random_U64_fewbits();
    int count_1s(U64 b);
    U64 rmask(int sq);
    U64 bmask(int sq);

    void genOccForLinesNDiags();
    U64 makeMaskHoriz(int sqr);
    U64 makeMaskVertic(int sqr);
    U64 makeMaskS(int sqr);
    U64 makeMaskN(int sqr);
    U64 makeMaskW(int sqr);
    U64 makeMaskE(int sqr);
    U64 makeMaskSW(int sqr);
    U64 makeMaskSE(int sqr);
    U64 makeMaskNW(int sqr);
    U64 makeMaskNE(int sqr);

    void populDiags();
    void populLines();
    int bitscanForward(U64 board);
    int bitscanReverse(U64 board);
    U64 doAttackNW (U64 occup, int sqr);
    U64 doAttackNE (U64 occup, int sqr);
    U64 doAttackSW (U64 occup, int sqr);
    U64 doAttackSE (U64 occup, int sqr);
    U64 doAttackN (U64 occup, int sqr);
    U64 doAttackE (U64 occup, int sqr);
    U64 doAttackS (U64 occup, int sqr);
    U64 doAttackW (U64 occup, int sqr);
    U64 rookAttack(int sqr, U64 occ);
    U64 bishopAttack(int sqr, U64 occ);

    void calcMagicsRook ();
    void calcMagicsBishop ();

    void calcAttackSqrKnight();
    void calcAttackSqrKing();
    void calcAttackSqrWPawn();
    void calcAttackSqrBPawn();

    void populRook();
    void populBishop();

};

extern magics mag;


#endif
