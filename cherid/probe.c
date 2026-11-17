#include "objid.h"
volatile unsigned long *probe_raw;
int main(void) {
    volatile unsigned long *p = objid_alloc(16);
    *p = 123;
    if (*p != 123) fail(3);
#ifdef EXPECT_TRAP
    /* ID 2 has the opposite low bit to stored ID 1; ID 0 bypasses checks. */
    probe_raw = objid_id((void *)p, 0);
    p = objid_id((void *)p, 2);
#ifdef NEGATIVE_STORE
    *p = 456;
#else
    unsigned long n = *p;
    (void)n;
#endif
    fail(4);
#endif
    return 0;
}
