
#ifndef TT_H
#define TT_H

#include "defs.h"


// value biger than max alpha/beta so safe to use it to
// signal that no valid entry in tt
#define NO_TT_ENTRY   22000000
#define NUM_OF_TT_ENTRIES 33554432

enum ttEntryType {ALPHA, BETA, FULL};

struct ttEntry {

    U64 hash;
    int depth;
    ttEntryType eType;
    int alpha;
    int beta;

};

struct transpositionTable {

    ttEntry table[NUM_OF_TT_ENTRIES];

    void save(U64 hash, int depth, ttEntryType eType, int alpha, int beta);
    int  probe(U64 hash, int depth, int alpha, int beta);

};

extern transpositionTable tt;


#endif
