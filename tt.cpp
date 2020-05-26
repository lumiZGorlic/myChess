#include "defs.h"
#include "tt.h"


transpositionTable tt;


void transpositionTable::save(U64 hash, int depth, ttEntryType eType,
                              int alpha, int beta)
{
    int idx = (hash & (NUM_OF_TT_ENTRIES-1) );
    //assert idx not ouf of bounds

    if ( table[idx].hash == hash && table[idx].depth > depth)
        return;

    table[idx].hash = hash;
    table[idx].depth = depth;
    table[idx].eType = eType;
    table[idx].alpha = alpha;
    table[idx].beta = beta;
}


int transpositionTable::probe(U64 hash, int depth, int alpha, int beta)
{
    int idx = (hash & (NUM_OF_TT_ENTRIES-1) );
    //assert

    // TODO check if 0 by default guaranteed
    if ( table[idx].hash == 0 )
        return NO_TT_ENTRY;
    if ( table[idx].hash != hash)
        return NO_TT_ENTRY;

    if(table[idx].depth >= depth)
    {
        if (table[idx].eType == FULL)
            return table[idx].alpha;
        else if (table[idx].eType == BETA && beta <= table[idx].beta)
            return beta;
        else if (table[idx].eType == ALPHA && alpha >= table[idx].alpha)
            return alpha;
    }

    return NO_TT_ENTRY;
}
