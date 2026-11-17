#include "objid.h"
#include "../bisort/src/node.h"
#include "../bisort/src/proc.h"
#ifndef LEVELS
#define LEVELS 9
#endif
#if LEVELS > 9
#error "This inherited bisort algorithm is validated only through LEVELS=9"
#endif
extern int NDim;
extern int olden_random(int);
static unsigned count;
static unsigned long sum;
static int previous;
static void inspect(HANDLE *p, int sorted, int down) {
    if (!p) return;
    inspect(p->left,sorted,down);
    if (sorted && count && (down ? previous < p->value : previous > p->value)) fail(20);
    previous=p->value; ++count; sum+=p->value;
    inspect(p->right,sorted,down);
}
int main(void) {
    unsigned long start=cycle();
    NDim=0;
    HANDLE *root=RandTree(1<<LEVELS,12345768,0,0);
    int extra=olden_random(245867)%100;
    count=0;sum=0;inspect(root,0,0);
    unsigned expected_count=count;
    unsigned long expected_sum=sum+extra;
    HANDLE *descending=RandTree(1<<LEVELS,12345768,0,0);
    int descending_extra=extra;
    unsigned long built=cycle();
    extra=Bisort(root,extra,0);
    count=0;sum=0;inspect(root,1,0);
    if (count!=expected_count || sum+extra!=expected_sum || previous>extra) fail(21);
    extra=Bisort(descending,descending_extra,1);
    count=0;sum=0;inspect(descending,1,1);
    if (count!=expected_count || sum+extra!=expected_sum || previous<extra) fail(22);
#ifdef ALGORITHM_ONLY
    return cycle()-built;
#else
    return cycle()-start;
#endif
}
