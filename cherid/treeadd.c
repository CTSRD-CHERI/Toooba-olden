#include "objid.h"
#include "../treeadd/src/tree.h"
#ifndef LEVELS
#define LEVELS 9
#endif
#ifndef REPEATS
#define REPEATS 4
#endif
int NumNodes=1, runs=1;
int main(void) {
    unsigned long begin=cycle();
    tree_t *root=TreeAlloc(LEVELS,0,1);
#ifdef ALGORITHM_ONLY
    if (TreeAdd(0,root)!=(1<<LEVELS)-1) fail(10);
#endif
    unsigned long built=cycle();
    for (int i=0;i<REPEATS;++i)
        if (TreeAdd(0,root)!=(1<<LEVELS)-1) fail(10);
    unsigned long end=cycle();
#ifdef ALGORITHM_ONLY
    return end-built;
#else
    return end-begin;
#endif
}
