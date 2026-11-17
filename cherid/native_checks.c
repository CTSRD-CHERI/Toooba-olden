/* Host-only algorithm check support; never linked into the RISC-V workload. */
#include <stdio.h>
#include <stdlib.h>
unsigned long cycle(void) { return 0; }
void fail(unsigned n) { fprintf(stderr,"Validation failed: %u\n", n); exit(1); }
