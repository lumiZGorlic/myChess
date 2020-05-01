#include "defs.h"
#include <random>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>

// TODO need to understand why there're two ways to calculate
//  it in calcMagics. Look at rhysre website (below the link)
//  to see how to generate assembler code to prove it works.
//  https://rhysre.net/2019/01/15/magic-bitboards.html
//  understand random_U64_fewbits and random_U64. maybe use c++ functions.


magics::magics()
{
    // populate things like diagNW etc as they're needed when generating magics
    populDiags();
    populLines();
    // generate permutations for lines and diags, will be stored in
    // occupLines and occupDiags
    genOccForLinesNDiags();

    //calcMagicsRook();
    //calcMagicsBishop();

    //saveInFile();
    readFromFile();

    calcAttackSqrKnight();
    calcAttackSqrKing();
    calcAttackSqrWPawn();
    calcAttackSqrBPawn();

    // populate rook and bishop masks
    populRook();
    populBishop();
}


// save magics in the file
void magics::saveInFile()
{
    std::ofstream magicFile ("magics.txt");
    if ( magicFile.is_open() )
    {
        for (int i = A1; i <= H8; i++)
            magicFile << rookMagics[i] << std::endl;

        magicFile << "------------------------------------------" << std::endl;

        for (int i = A1; i <= H8; i++)
            magicFile << bishopMagics[i] << std::endl;
    }
    else
        std::cout << "Unable to open file" << std::endl;    
}


// read magics fom the file and calculate attack arrays etc
void magics::readFromFile()
{
    std::ifstream magicFile ("magics.txt");
    if ( magicFile.is_open() )
    {
        for (int sqr = A1; sqr <= H8; sqr++)
            magicFile >> rookMagics[sqr];

        std::string tmpS;
        magicFile >> tmpS;

        for (int sqr = A1; sqr <= H8; sqr++)
            magicFile >> bishopMagics[sqr];
    }
    else
        std::cout << "Unable to open file" << std::endl;


    for (int sqr = A1; sqr <= H8; sqr++)
    {
        // TODO analyze the below so can understand it inside out
        //int shift = 64 - RBits[sqr];
        int shift = 32 - RBits[sqr];

        std::set<U64>::iterator it = occupLines[sqr].begin();
        for (; it != (occupLines[sqr]).end(); it++)
        {
            U64 occ = *it;
            //int idx = ( occ * magic ) >> shift;
            int idx = (unsigned)((int)occ*(int) rookMagics[sqr] 
                    ^ (int)(occ>>32)*(int)(rookMagics[sqr] >> 32)) >> shift;
            rookAttackArr[sqr][idx] = rookAttack(sqr, *it);
        }
    }

    for (int sqr = A1; sqr <= H8; sqr++)
    {
        // TODO analyze the below so can understand it inside out
        //int shift = 64 - BBits[sqr];
        int shift = 32 - BBits[sqr];

        std::set<U64>::iterator it = occupDiags[sqr].begin();
        for (; it != (occupDiags[sqr]).end(); it++)
        {
            U64 occ = *it;
            //int idx = ( occ * magic ) >> shift;
            int idx = (unsigned)((int)occ*(int) bishopMagics[sqr]
                    ^ (int)(occ>>32)*(int)(bishopMagics[sqr] >> 32)) >> shift;
            bishopAttackArr[sqr][idx] = bishopAttack(sqr, *it);
        }
    }
}


// 'random family' helper funcs section start

U64 magics::random_U64() {
  U64 u1, u2, u3, u4;
  u1 = (U64)(random()) & 0xFFFF; u2 = (U64)(random()) & 0xFFFF;
  u3 = (U64)(random()) & 0xFFFF; u4 = (U64)(random()) & 0xFFFF;
  return u1 | (u2 << 16) | (u3 << 32) | (u4 << 48);
}

U64 magics::random_U64_fewbits() {
  return random_U64() & random_U64() & random_U64();
}

int magics::count_1s(U64 b) {
  int r;
  for(r = 0; b; r++, b &= b - 1)
      ;
  return r;
}

U64 magics::rmask(int sq) {
  U64 result = 0ULL;
  int rk = sq/8, fl = sq%8, r, f;

  for(r = rk+1; r <= 6; r++)
      result |= (1ULL << (fl + r*8));

  for(r = rk-1; r >= 1; r--)
      result |= (1ULL << (fl + r*8));

  for(f = fl+1; f <= 6; f++)
      result |= (1ULL << (f + rk*8));

  for(f = fl-1; f >= 1; f--)
      result |= (1ULL << (f + rk*8));
  return result;
}

U64 magics::bmask(int sq) {
  U64 result = 0ULL;
  int rk = sq/8, fl = sq%8, r, f;

  for(r=rk+1, f=fl+1; r<=6 && f<=6; r++, f++)
      result |= (1ULL << (f + r*8));

  for(r=rk+1, f=fl-1; r<=6 && f>=1; r++, f--)
      result |= (1ULL << (f + r*8));

  for(r=rk-1, f=fl+1; r>=1 && f<=6; r--, f++)
      result |= (1ULL << (f + r*8));

  for(r=rk-1, f=fl-1; r>=1 && f>=1; r--, f--)
      result |= (1ULL << (f + r*8));

  return result;
}

// 'random family' helper funcs section end


// other helper funcs section start

void magics::genOccForLinesNDiags() {

    /*
        EXPL:

        to generate all possible permutations for a row with the
        given square
        for each square sqr
            generate a number between 0 and 255 and then
            shift it right. the distance to shift depends on
            the square as per login in makeMaskHoriz

        to generate all possible permutations for a column with the
        given square
        for each square sqr
            generate a number between 0 and 255 and then
            map it to the first column using the array mapVert
            then shift it right depending on the square

    */


    int mapReal[64] = {
                      56,  57,  58,  59,  60,  61,  62,  63,
                      48,  49,  50,  51,  52,  53,  54,  55,
                      40,  41,  42,  43,  44,  45,  46,  47,
                      32,  33,  34,  35,  36,  37,  38,  39,
                      24,  25,  26,  27,  28,  29,  30,  31,
                      16,  17,  18,  19,  20,  21,  22,  23,
                      8,    9,  10,  11,  12,  13,  14,  15,
                      0,    1,   2,   3,   4,   5,   6,   7  };


    int mapVert[8] = {0, 8, 16, 24, 32, 40, 48, 56};

    int zerEdgeN[64] = {
      56, 57, 58, 59, 60, 61, 62, 63,
      56, 57, 58, 59, 60, 61, 62, 63,
      56, 57, 58, 59, 60, 61, 62, 63,
      56, 57, 58, 59, 60, 61, 62, 63,
      56, 57, 58, 59, 60, 61, 62, 63,
      56, 57, 58, 59, 60, 61, 62, 63,
      56, 57, 58, 59, 60, 61, 62, 63,
      56, 57, 58, 59, 60, 61, 62, 63
    };

    int zerEdgeS[64] = {
      0,  1,  2,  3,  4,  5,  6,  7,
      0,  1,  2,  3,  4,  5,  6,  7,
      0,  1,  2,  3,  4,  5,  6,  7,
      0,  1,  2,  3,  4,  5,  6,  7,
      0,  1,  2,  3,  4,  5,  6,  7,
      0,  1,  2,  3,  4,  5,  6,  7,
      0,  1,  2,  3,  4,  5,  6,  7,
      0,  1,  2,  3,  4,  5,  6,  7
    };

    int zerEdgeW[64] = {
      0,  0,  0,  0,  0,  0,  0,  0,
      8,  8,  8,  8,  8,  8,  8,  8,
     16, 16, 16, 16, 16, 16, 16, 16,
     24, 24, 24, 24, 24, 24, 24, 24,
     32, 32, 32, 32, 32, 32, 32, 32,
     40, 40, 40, 40, 40, 40, 40, 40,
     48, 48, 48, 48, 48, 48, 48, 48,
     56, 56, 56, 56, 56, 56, 56, 56
    };

    int zerEdgeE[64] = {
      7,  7,  7,  7,  7,  7,  7,  7,
     15, 15, 15, 15, 15, 15, 15, 15,
     23, 23, 23, 23, 23, 23, 23, 23,
     31, 31, 31, 31, 31, 31, 31, 31,
     39, 39, 39, 39, 39, 39, 39, 39,
     47, 47, 47, 47, 47, 47, 47, 47,
     55, 55, 55, 55, 55, 55, 55, 55,
     63, 63, 63, 63, 63, 63, 63, 63
    };

    for (int sqr = 0; sqr < 64; sqr++)
    {
        U64 theSqr = 1; theSqr = theSqr << sqr; theSqr = ~theSqr;

        U64 edgeS = 0, edgeN = 0, edgeW = 0, edgeE = 0;
        setBit(edgeS, zerEdgeS[sqr]);
        setBit(edgeN, zerEdgeN[sqr]);
        setBit(edgeE, zerEdgeE[sqr]);
        setBit(edgeW, zerEdgeW[sqr]);
        U64 zeroEdges = ~(edgeS | edgeN | edgeW | edgeE );

        for (U64 j = 0; j <= 255; j++)
        {
            int tmp = sqr/8;
            int shift = tmp*8;
            U64 horiz = (j << shift);

            for (U64 k = 0; k <= 255; k++)
            {
                /* bit 0 -> bit mapVer[0]
                   bit 1 -> bit mapVer[1]
                .... */

                U64 vert = 0;
                for (int l = 0; l < 8; l++)
                    if (k & (1 << l))
                        setBit(vert, mapVert[l]);

                shift = sqr % 8;
                vert <<= shift;

                occupLines[sqr].insert( (horiz | vert) & zeroEdges & theSqr);
            }
        }
    }


/* ----------------------------------------------------------------
   ----------------------------------------------------------------
*/

    int mapWE[64] = {
                      0,  1,  2,  3,  4,  5,  6,  7,
                      1,  2,  3,  4,  5,  6,  7,  8,
                      2,  3,  4,  5,  6,  7,  8,  9,
                      3,  4,  5,  6,  7,  8,  9, 10,
                      4,  5,  6,  7,  8,  9, 10, 11,
                      5,  6,  7,  8,  9, 10, 11, 12,
                      6,  7,  8,  9, 10, 11, 12, 13,
                      7,  8,  9, 10, 11, 12, 13, 14  };

    // diagonals running west-east i.e. 
    // {a8}, {a7,b8}, {a6,b7,c8} .... {g1,h2}, {h1}

    int diagWE0[]  = {56 };
    int diagWE1[]  = {48, 57 };
    int diagWE2[]  = {40, 49, 58 };
    int diagWE3[]  = {32, 41, 50, 59 };
    int diagWE4[]  = {24, 33, 42, 51, 60 };
    int diagWE5[]  = {16, 25, 34, 43, 52, 61 };
    int diagWE6[]  = { 8, 17, 26, 35, 44, 53, 62 };
    int diagWE7[]  = { 0,  9, 18, 27, 36, 45, 54, 63 };
    int diagWE8[]  = { 1, 10, 19, 28, 37, 46, 55 };
    int diagWE9[]  = { 2, 11, 20, 29, 38, 47 };
    int diagWE10[] = { 3, 12, 21, 30, 39 };
    int diagWE11[] = { 4, 13, 22, 31 };
    int diagWE12[] = { 5, 14, 23 };
    int diagWE13[] = { 6, 15 };
    int diagWE14[] = { 7 };


    std::vector<int> diagWE0V  (diagWE0,
            diagWE0  + sizeof(diagWE0)  / sizeof(diagWE0[0])  );
    std::vector<int> diagWE1V  (diagWE1,
            diagWE1  + sizeof(diagWE1)  / sizeof(diagWE1[0])  );
    std::vector<int> diagWE2V  (diagWE2,
            diagWE2  + sizeof(diagWE2)  / sizeof(diagWE2[0])  );
    std::vector<int> diagWE3V  (diagWE3,
            diagWE3  + sizeof(diagWE3)  / sizeof(diagWE3[0])  );
    std::vector<int> diagWE4V  (diagWE4,
            diagWE4  + sizeof(diagWE4)  / sizeof(diagWE4[0])  );
    std::vector<int> diagWE5V  (diagWE5,
            diagWE5  + sizeof(diagWE5)  / sizeof(diagWE5[0])  );
    std::vector<int> diagWE6V  (diagWE6,
            diagWE6  + sizeof(diagWE6)  / sizeof(diagWE6[0])  );
    std::vector<int> diagWE7V  (diagWE7,
            diagWE7  + sizeof(diagWE7)  / sizeof(diagWE7[0])  );
    std::vector<int> diagWE8V  (diagWE8,
            diagWE8  + sizeof(diagWE8)  / sizeof(diagWE8[0])  );
    std::vector<int> diagWE9V  (diagWE9,
            diagWE9  + sizeof(diagWE9)  / sizeof(diagWE9[0])  );
    std::vector<int> diagWE10V (diagWE10,
            diagWE10 + sizeof(diagWE10) / sizeof(diagWE10[0]) );
    std::vector<int> diagWE11V (diagWE11,
            diagWE11 + sizeof(diagWE11) / sizeof(diagWE11[0]) );
    std::vector<int> diagWE12V (diagWE12,
            diagWE12 + sizeof(diagWE12) / sizeof(diagWE12[0]) );
    std::vector<int> diagWE13V (diagWE13,
            diagWE13 + sizeof(diagWE13) / sizeof(diagWE13[0]) );
    std::vector<int> diagWE14V (diagWE14,
            diagWE14 + sizeof(diagWE14) / sizeof(diagWE14[0]) );


    std::vector< std::vector<int> > v1;
    v1.push_back(diagWE0V);
    v1.push_back(diagWE1V);
    v1.push_back(diagWE2V);
    v1.push_back(diagWE3V);
    v1.push_back(diagWE4V);
    v1.push_back(diagWE5V);
    v1.push_back(diagWE6V);
    v1.push_back(diagWE7V);
    v1.push_back(diagWE8V);
    v1.push_back(diagWE9V);
    v1.push_back(diagWE10V);
    v1.push_back(diagWE11V);
    v1.push_back(diagWE12V);
    v1.push_back(diagWE13V);
    v1.push_back(diagWE14V);

/* ----------------------------------------------------------------
   ----------------------------------------------------------------
*/

    int mapEW[64] = {
                      7,  8,  9, 10, 11, 12, 13, 14,
                      6,  7,  8,  9, 10, 11, 12, 13,
                      5,  6,  7,  8,  9, 10, 11, 12,
                      4,  5,  6,  7,  8,  9, 10, 11,
                      3,  4,  5,  6,  7,  8,  9, 10,
                      2,  3,  4,  5,  6,  7,  8,  9,
                      1,  2,  3,  4,  5,  6,  7,  8,
                      0,  1,  2,  3,  4,  5,  6,  7  };

    // diagonals running east-west i.e.
    // {a1}, {b1,a2}, {c1,b2,a3} .... {h7,g8}, {h8}

    int diagEW0[]  = { 0 };
    int diagEW1[]  = { 1,  8 };
    int diagEW2[]  = { 2,  9, 16 };
    int diagEW3[]  = { 3,  10, 17, 24 };
    int diagEW4[]  = { 4,  11, 18, 25, 32 };
    int diagEW5[]  = { 5,  12, 19, 26, 33, 40 };
    int diagEW6[]  = { 6,  13, 20, 27, 34, 41, 48 };
    int diagEW7[]  = { 7,  14, 21, 28, 35, 42, 49, 56 };
    int diagEW8[]  = { 15, 22, 29, 36, 43, 50, 57 };
    int diagEW9[]  = { 23, 30, 37, 44, 51, 58 };
    int diagEW10[] = { 31, 38, 45, 52, 59 };
    int diagEW11[] = { 39, 46, 53, 60 };
    int diagEW12[] = { 47, 54, 61 };
    int diagEW13[] = { 55, 62 };
    int diagEW14[] = { 63 };

    std::vector<int> diagEW0V  (diagEW0,
            diagEW0  + sizeof(diagEW0)  / sizeof(diagEW0[0])  );
    std::vector<int> diagEW1V  (diagEW1,
            diagEW1  + sizeof(diagEW1)  / sizeof(diagEW1[0])  );
    std::vector<int> diagEW2V  (diagEW2,
            diagEW2  + sizeof(diagEW2)  / sizeof(diagEW2[0])  );
    std::vector<int> diagEW3V  (diagEW3,
            diagEW3  + sizeof(diagEW3)  / sizeof(diagEW3[0])  );
    std::vector<int> diagEW4V  (diagEW4,
            diagEW4  + sizeof(diagEW4)  / sizeof(diagEW4[0])  );
    std::vector<int> diagEW5V  (diagEW5,
            diagEW5  + sizeof(diagEW5)  / sizeof(diagEW5[0])  );
    std::vector<int> diagEW6V  (diagEW6,
            diagEW6  + sizeof(diagEW6)  / sizeof(diagEW6[0])  );
    std::vector<int> diagEW7V  (diagEW7,
            diagEW7  + sizeof(diagEW7)  / sizeof(diagEW7[0])  );
    std::vector<int> diagEW8V  (diagEW8,
            diagEW8  + sizeof(diagEW8)  / sizeof(diagEW8[0])  );
    std::vector<int> diagEW9V  (diagEW9,
            diagEW9  + sizeof(diagEW9)  / sizeof(diagEW9[0])  );
    std::vector<int> diagEW10V (diagEW10,
            diagEW10 + sizeof(diagEW10) / sizeof(diagEW10[0]) );
    std::vector<int> diagEW11V (diagEW11,
            diagEW11 + sizeof(diagEW11) / sizeof(diagEW11[0]) );
    std::vector<int> diagEW12V (diagEW12,
            diagEW12 + sizeof(diagEW12) / sizeof(diagEW12[0]) );
    std::vector<int> diagEW13V (diagEW13,
            diagEW13 + sizeof(diagEW13) / sizeof(diagEW13[0]) );
    std::vector<int> diagEW14V (diagEW14,
            diagEW14 + sizeof(diagEW14) / sizeof(diagEW14[0]) );


    std::vector< std::vector<int> > v2;
    v2.push_back(diagEW0V);
    v2.push_back(diagEW1V);
    v2.push_back(diagEW2V);
    v2.push_back(diagEW3V);
    v2.push_back(diagEW4V);
    v2.push_back(diagEW5V);
    v2.push_back(diagEW6V);
    v2.push_back(diagEW7V);
    v2.push_back(diagEW8V);
    v2.push_back(diagEW9V);
    v2.push_back(diagEW10V);
    v2.push_back(diagEW11V);
    v2.push_back(diagEW12V);
    v2.push_back(diagEW13V);
    v2.push_back(diagEW14V);

/* ----------------------------------------------------------------
   ----------------------------------------------------------------
*/

    for (int sqr = 0; sqr < 64; sqr++)
    {
        U64 theSqr = 1; theSqr = theSqr << sqr; theSqr = ~theSqr;

        U64 edgeS = 0xFF;
        U64 edgeN = edgeS << 56;
        U64 edgeE = 0x8080808080808080;
        U64 edgeW = 0;
        for (int i = 0; i < 8; i++)
            setBit(edgeW, i*8);

        U64 zeroEdges = ~(edgeS | edgeN | edgeW | edgeE );

        // mapping the current square in the loop to the squares on the
        // diagonal west-east the square belongs to 
        std::vector<int> vTmp = v1[mapWE[mapReal[sqr]]];
        int N = vTmp.size();
        int maxVal = pow(2, N);

        // go through all the permutations that the diagonal can have
        // and apply the mapping
        for (U64 perm1 = 0; perm1 <= maxVal-1; perm1++)
        {
            U64 diag1 = 0;

            for(int i = 0; i < N; i++)
            {
                if (perm1 & (1 << i) )
                    setBit(diag1, vTmp[i]);
            }


            // mapping the current square in the loop to the squares on the
            // diagonal east-west the square belongs to 
            std::vector<int> vTmp2 = v2[mapEW[mapReal[sqr]]];
            int N2 = vTmp2.size();
            int maxVal2 = pow(2, N2);

            // go through all the permutations that the diagonal can have
            // and apply the mapping
            for (U64 perm2 = 0; perm2 <= maxVal2 - 1; perm2++)
            {
                U64 diag2 = 0;

                for(int j = 0; j < N2; j++)
                {
                    if (perm2 & (1 << j) )
                        setBit(diag2, vTmp2[j]);
                }

                occupDiags[sqr].insert( (diag1 | diag2) & zeroEdges & theSqr);
            }
        }
    }
}


U64 magics::makeMaskHoriz(int sqr)
{
    int tmp = sqr/8;
    int shift = tmp*8;

    U64 ret = 255;
    ret <<= shift;

    return ret;
}

U64 magics::makeMaskVertic(int sqr)
{
    U64 ret = 0;

    for (int i = sqr; i <= 63; i += 8)
    {
        setBit(ret, i);
    }

    for (int i = sqr; i >= 0; i -= 8)
    {
        setBit(ret, i);
    }

    return ret;
}

/*

 EXPL:
 the line "ret &= ~(one << sqr);" needed so that square 'sqr' set to '0'
   e.g. in case of 4x4 and we call makeMaskN(5) the output is:

   0 1 0 0
   0 1 0 0
   0 0 0 0
   0 0 0 0

  so setting the original square '5' to '0'. Without that line it would be:

   0 1 0 0
   0 1 0 0
   0 1 0 0
   0 0 0 0
*/

U64 magics::makeMaskS(int sqr)
{
    U64 ret = 0;

    for (int i = sqr; i >= 0; i -= 8)
    {
        setBit(ret, i);
    }

    U64 one = 1;
    ret &= ~(one << sqr);
    return ret;
}

U64 magics::makeMaskN(int sqr)
{
    U64 ret = 0;

    for (int i = sqr; i <= 63; i += 8)
    {
        setBit(ret, i);
    }

    U64 one = 1;
    ret &= ~(one << sqr);
    return ret;
}

U64 magics::makeMaskW(int sqr)
{
    U64 ret = 0;

    U64 tmp = makeMaskVertic(A1);
    for (int i = sqr; i >= 0; i -= 1)
    {
        setBit(ret, i);

        if (ret & tmp)
            break;
    }

    U64 one = 1;
    ret &= ~(one << sqr);
    return ret;
}

U64 magics::makeMaskE(int sqr)
{
    U64 ret = 0;

    U64 tmp = makeMaskVertic(H1);
    for (int i = sqr; i <= 63; i += 1)
    {
        setBit(ret, i);

        if (ret & tmp)
            break;
    }

    U64 one = 1;
    ret &= ~(one << sqr);
    return ret;
}

U64 magics::makeMaskSW(int sqr)
{
    U64 ret = 0;

    U64 tmp = makeMaskVertic(A1);
    for (int i = sqr; i >= 0; i -= 9)
    {
        setBit(ret, i);

        if (ret & tmp)
            break;
    }

    U64 one = 1;
    ret &= ~(one << sqr);
    return ret;
}

U64 magics::makeMaskSE(int sqr)
{
    U64 ret = 0;

    U64 tmp = makeMaskVertic(H1);
    for (int i = sqr; i >= 0; i -= 7)
    {
        setBit(ret, i);

        if (ret & tmp)
            break;
    }

    U64 one = 1;
    ret &= ~(one << sqr);
    return ret;
}

U64 magics::makeMaskNW(int sqr)
{
    U64 ret = 0;

    U64 tmp = makeMaskVertic(A1);
    for (int i = sqr; i <= 63; i += 7)
    {
        setBit(ret, i);

        if (ret & tmp)
            break;
    }

    U64 one = 1;
    ret &= ~(one << sqr);
    return ret;
}

U64 magics::makeMaskNE(int sqr)
{
    U64 ret = 0;

    U64 tmp = makeMaskVertic(H1);
    for (int i = sqr; i <= 63; i += 9)
    {
        setBit(ret, i);

        if (ret & tmp)
            break;
    }

    U64 one = 1;
    ret &= ~(one << sqr);
    return ret;
}

// other helper funcs section end


// classic move generation section start

void magics::populDiags()
{

    for (int i = A1; i <= H8; i++)
    {
        diagNW[i] = makeMaskNW(i);
    }

    for (int i = A1; i <= H8; i++)
    {
        diagNE[i] = makeMaskNE(i);
    }

    for (int i = A1; i <= H8; i++)
    {
        diagSW[i] = makeMaskSW(i);
    }

    for (int i = A1; i <= H8; i++)
    {
        diagSE[i] = makeMaskSE(i);
    }
}

void magics::populLines()
{

    for (int i = A1; i <= H8; i++)
    {
        lineN[i] = makeMaskN(i);
    }

    for (int i = A1; i <= H8; i++)
    {
        lineS[i] = makeMaskS(i);
    }

    for (int i = A1; i <= H8; i++)
    {
        lineW[i] = makeMaskW(i);
    }

    for (int i = A1; i <= H8; i++)
    {
        lineE[i] = makeMaskE(i);
    }
}


int magics::bitscanForward(U64 board) {
  if (board == 0) {
    return -1;
  }
  return __builtin_ffsll(board) - 1;
}

int magics::bitscanReverse(U64 board) {
  if (board == 0) {
    return -1;
  }
  return 63 - __builtin_clzll(board);
}


// the below funcs calculate squares under attack using a classic approach
// and are used by the procs generating magics

U64 magics::doAttackNW (U64 occup, int sqr) {
    U64 ret = 0;

    ret |= diagNW[sqr];
    if (diagNW[sqr] & occup) {
        int blockerIndex = bitscanForward(diagNW[sqr] & occup);
        ret &= ~diagNW[blockerIndex];
    }

    return ret;
}

U64 magics::doAttackNE (U64 occup, int sqr) {
    U64 ret = 0;

    ret |= diagNE[sqr];

    if (diagNE[sqr] & occup) {
        int blockerIndex = bitscanForward(diagNE[sqr] & occup);
        ret &= ~diagNE[blockerIndex];
    }

    return ret;
}

U64 magics::doAttackSW (U64 occup, int sqr) {
    U64 ret = 0;

    ret |= diagSW[sqr];
    if (diagSW[sqr] & occup) {
        int blockerIndex = bitscanReverse(diagSW[sqr] & occup);
        ret &= ~diagSW[blockerIndex];
    }

    return ret;
}

U64 magics::doAttackSE (U64 occup, int sqr) {
    U64 ret = 0;

    ret |= diagSE[sqr];
    if (diagSE[sqr] & occup) {
        int blockerIndex = bitscanReverse(diagSE[sqr] & occup);
        ret &= ~diagSE[blockerIndex];
    }

    return ret;
}

U64 magics::doAttackN (U64 occup, int sqr) {
    U64 ret = 0;

    ret |= lineN[sqr];
    if (lineN[sqr] & occup) {
        int blockerIndex = bitscanForward(lineN[sqr] & occup);
        ret &= ~lineN[blockerIndex];
    }

    return ret;
}

U64 magics::doAttackE (U64 occup, int sqr) {
    U64 ret = 0;

    ret |= lineE[sqr];
    if (lineE[sqr] & occup) {
        int blockerIndex = bitscanForward(lineE[sqr] & occup);
        ret &= ~lineE[blockerIndex];
    }

    return ret;
}

U64 magics::doAttackS (U64 occup, int sqr) {
    U64 ret = 0;

    ret |= lineS[sqr];
    if (lineS[sqr] & occup) {
        int blockerIndex = bitscanReverse(lineS[sqr] & occup);
        ret &= ~lineS[blockerIndex];
    }

    return ret;
}

U64 magics::doAttackW (U64 occup, int sqr) {
    U64 ret = 0;

    ret |= lineW[sqr];
    if (lineW[sqr] & occup) {
        int blockerIndex = bitscanReverse(lineW[sqr] & occup);
        ret &= ~lineW[blockerIndex];
    }

    return ret;
}

U64 magics::rookAttack(int sqr, U64 occ) {
    U64 ret =  doAttackN(occ, sqr) | doAttackS(occ, sqr) |
               doAttackW(occ, sqr) | doAttackE(occ, sqr);

    return ret;
}

U64 magics::bishopAttack(int sqr, U64 occ) {
    U64 ret =  doAttackNE(occ, sqr) | doAttackNW(occ, sqr) |
               doAttackSE(occ, sqr) | doAttackSW(occ, sqr);

    return ret;
}

// classic move generation section end


// calculate magics section start

/*
    how we use magics - say we have a rook on square i, the transformations
    we do to arrive at attack table is: 
    ( current position (occupancy), RBits[i], rookMagics[i] ) -> idx ->
       -> rookAttackArr[i][idx]

*/
void magics::calcMagicsRook () {
    // TODO analyze it so can understand it inside out
    // 4096 = 2^12 as e.g. for a rook on A1 we care about occupancy on 12
    // squares (excluding edges) 
    U64 used[4096];

    for (int sqr = A1; sqr <= H8; sqr++)
    {
        U64 mask = rmask(sqr);
        for (int j = 0; j < 10000000; j++)
        {
            for(int k = 0; k < 4096; k++)
                used[k] = 0;

            U64 magic;
            magic = random_U64_fewbits();
            if(count_1s((mask * magic) & 0xFF00000000000000ULL) < 6)
                continue;

            bool isMagic = true;

            // TODO analyze the below so can understand it inside out
            //int shift = 64 - RBits[sqr];
            int shift = 32 - RBits[sqr];

            std::set<U64>::iterator it = occupLines[sqr].begin();
            for (; it != (occupLines[sqr]).end(); it++)
            {
                U64 occ = *it;

                //int idx = ( occ * magic ) >> shift;
                int idx = (unsigned)((int)occ*(int)magic
                        ^ (int)(occ>>32)*(int)(magic>>32)) >> shift;

                U64 rookAttackTmp = rookAttack(sqr, *it);
                if ( used[idx] == 0 || used[idx] == rookAttackTmp )
                    used[idx] = rookAttackTmp;
                else
                {
                    isMagic = false;
                    break;
                }
            }

            if (isMagic)
            {
                rookMagics[sqr] = magic;
                for (int i = 0; i < 4096; i++)
                   rookAttackArr[sqr][i] = used[i];
                break;
            }
        }
    }
}


/*
    how we use magics - say we have a bishop on square i, the transformations
    we do to arrive at attack table is: 
    ( current position (occupancy), BBits[i], bishopMagics[i] ) -> idx ->
       -> bishopAttackArr[i][idx]

*/
void magics::calcMagicsBishop () {
    // TODO analyze it so can understand it inside out
    // 4096 = 2^12 but not sure for bishop we need as many
    U64 used[4096];

    for (int sqr = A1; sqr <= H8; sqr++)
    {
        U64 mask = bmask(sqr);
        for (int j = 0; j < 10000000; j++)
        {
            for(int k = 0; k < 4096; k++)
                used[k] = 0;

            U64 magic;
            magic = random_U64_fewbits();
            if(count_1s((mask * magic) & 0xFF00000000000000ULL) < 6)
                continue;

            bool isMagic = true;

            // TODO analyze the below so can understand it inside out
            //int shift = 64 - RBits[i];
            int shift = 32 - BBits[sqr];

            std::set<U64>::iterator it = occupDiags[sqr].begin();
            for (; it != (occupDiags[sqr]).end(); it++)
            {
                U64 occ = *it;

                //int idx = ( occ * magic ) >> shift;
                int idx = (unsigned)((int)occ*(int)magic
                        ^ (int)(occ>>32)*(int)(magic>>32)) >> shift;


                U64 bishopAttackTmp = bishopAttack(sqr, *it);
                if ( used[idx] == 0 || used[idx] == bishopAttackTmp )
                    used[idx] = bishopAttackTmp;
                else
                {
                    isMagic = false;
                    break;
                }
            }

            if (isMagic)
            {
                bishopMagics[sqr] = magic;
                for (int i = 0; i < 4096; i++)
                   bishopAttackArr[sqr][i] = used[i];
                break;
            }
        }
    }
}

// calculate magics section end


// calculate attack arrays section start

void magics::calcAttackSqrKnight() {

    for (int sqr = A1; sqr <= H8; sqr++)
    {
        U64 attack = 0;
        for (int j = -2; j <= 2; j += 4)
        {
            for (int k = -1; k <= 1; k += 2)
            {
                if ( rankFileOK(rank(sqr)+j) && rankFileOK(file(sqr)+k) )
                {
                    int destSqr = square(rank(sqr)+j, file(sqr)+k);
                    setBit(attack, destSqr);
                }
                if ( rankFileOK(rank(sqr)+k) && rankFileOK(file(sqr)+j) )
                {
                    int destSqr = square(rank(sqr)+k, file(sqr)+j);
                    setBit(attack, destSqr);
                }
            }
        }
        knightAttackArr[sqr] = attack;
    }
}

void magics::calcAttackSqrKing()
{

    for (int sqr = A1; sqr <= H8; sqr++)
    {
        U64 attack = 0;
        for (int j = -1; j <= 1; j += 1)
        {
            for (int k = -1; k <= 1; k += 1)
            {
                if(j==0 && k==0)
                   continue;

                if ( rankFileOK(rank(sqr)+j) && rankFileOK(file(sqr)+k) )
                {
                    int destSqr = square(rank(sqr)+j, file(sqr)+k);
                    setBit(attack, destSqr);
                }
            }
        }
        kingAttackArr[sqr] = attack;
    }
}

void magics::calcAttackSqrWPawn()
{
    for (int sqr = A2; sqr <= H7; sqr++)
    {
        U64 attack = 0;

        if (sqr % 8 != 0)
        {
            setBit(attack, sqr+7);
        }

        if (sqr % 8 != 7)
        {
            setBit(attack, sqr+9);
        }

        wPawnAttackArr[sqr] = attack;
    }
}

void magics::calcAttackSqrBPawn()
{
    for (int sqr = A2; sqr <= H7; sqr++)
    {
        U64 attack = 0;

        if (sqr % 8 != 0)
        {
            setBit(attack, sqr-9);
        }

        if (sqr % 8 != 7)
        {
            setBit(attack, sqr-7);
        }

        bPawnAttackArr[sqr] = attack;
    }
}

// calculate attack arrays section


// calculate rook and bishop masks section start

// this stuff needed for move generation using magics

// TODO interesting excercise would be to demonstrate why
// the edges should be zero. the idea is kind of clear to me but
// still could demonstrate it neatly

void magics::populRook()
{
    int zerEdgeN[64] = {
      56, 57, 58, 59, 60, 61, 62, 63,
      56, 57, 58, 59, 60, 61, 62, 63,
      56, 57, 58, 59, 60, 61, 62, 63,
      56, 57, 58, 59, 60, 61, 62, 63,
      56, 57, 58, 59, 60, 61, 62, 63,
      56, 57, 58, 59, 60, 61, 62, 63,
      56, 57, 58, 59, 60, 61, 62, 63,
      56, 57, 58, 59, 60, 61, 62, 63
    };

    int zerEdgeS[64] = {
      0,  1,  2,  3,  4,  5,  6,  7,
      0,  1,  2,  3,  4,  5,  6,  7,
      0,  1,  2,  3,  4,  5,  6,  7,
      0,  1,  2,  3,  4,  5,  6,  7,
      0,  1,  2,  3,  4,  5,  6,  7,
      0,  1,  2,  3,  4,  5,  6,  7,
      0,  1,  2,  3,  4,  5,  6,  7,
      0,  1,  2,  3,  4,  5,  6,  7
    };

    int zerEdgeW[64] = {
      0,  0,  0,  0,  0,  0,  0,  0,
      8,  8,  8,  8,  8,  8,  8,  8,
     16, 16, 16, 16, 16, 16, 16, 16,
     24, 24, 24, 24, 24, 24, 24, 24,
     32, 32, 32, 32, 32, 32, 32, 32,
     40, 40, 40, 40, 40, 40, 40, 40,
     48, 48, 48, 48, 48, 48, 48, 48,
     56, 56, 56, 56, 56, 56, 56, 56
    };

    int zerEdgeE[64] = {
      7,  7,  7,  7,  7,  7,  7,  7,
     15, 15, 15, 15, 15, 15, 15, 15,
     23, 23, 23, 23, 23, 23, 23, 23,
     31, 31, 31, 31, 31, 31, 31, 31,
     39, 39, 39, 39, 39, 39, 39, 39,
     47, 47, 47, 47, 47, 47, 47, 47,
     55, 55, 55, 55, 55, 55, 55, 55,
     63, 63, 63, 63, 63, 63, 63, 63
    };


    for (int i = A1; i <= H8; i++)
    {
        rookMask[i] = makeMaskVertic(i) | makeMaskHoriz(i);
        U64 one = 1;
        rookMask[i] &= ~(one << i);

        U64 edgeS = 0, edgeN = 0, edgeW = 0, edgeE = 0;
        setBit(edgeS, zerEdgeS[i]);
        setBit(edgeN, zerEdgeN[i]);
        setBit(edgeE, zerEdgeE[i]);
        setBit(edgeW, zerEdgeW[i]);
        U64 zeroEdges = ~(edgeS | edgeN | edgeW | edgeE );
        rookMask[i] &= zeroEdges;
    }
}

// calculate bishop masks
void magics::populBishop()
{

    for (int i = A1; i <= H8; i++)
    {
        bishopMask[i] = makeMaskNW(i) | makeMaskNE(i) | makeMaskSW(i)
                      | makeMaskSE(i);
        U64 one = 1;
        bishopMask[i] &= ~(one << i);

        U64 edgeS = 0xFF;
        U64 edgeN = edgeS << 56;
        U64 edgeE = 0x8080808080808080;

        // TODO not sure why doesn't work
        //U64 edgeW = edgeE >> 8;

        U64 edgeW = 0;
        for (int ii = 0; ii < 8; ii++)
            setBit(edgeW, ii*8);

        U64 zeroEdges = ~(edgeS | edgeN | edgeW | edgeE );
        bishopMask[i] &= zeroEdges;
    }
}

// calculate rook and bishop masks section end

