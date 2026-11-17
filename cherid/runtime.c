/* Bare-metal runtime; startup conventions from Toooba-mibench2/syscalls.c. */
#include "objid.h"
#include <cheri_init_globals.h>
#ifndef HEAP_BYTES
#define HEAP_BYTES (2 * 1024 * 1024)
#endif
static unsigned char heap[HEAP_BYTES] __attribute__((aligned(4096)));
static size_t page, offset;
static unsigned slot;
volatile unsigned long long tohost __attribute__((aligned(64)));
void _start_purecap(void) {
    cheri_init_globals_3(__builtin_cheri_global_data_get(),
        __builtin_cheri_program_counter_get(), __builtin_cheri_global_data_get());
}
void _exit(int status) {
    for (;;) tohost = ((unsigned long long)(unsigned)status << 1) | 1;
}
void fail(unsigned code) { _exit(0x40000000 | code); }
unsigned long cycle(void) {
    unsigned long n;
    __asm__ volatile("csrr %0, mcycle" : "=r"(n) :: "memory");
    return n;
}
void *objid_alloc(size_t bytes) {
    size_t size = (bytes + 15) & ~(size_t)15;
    if (!bytes || size > 4032) fail(1);
    if (slot == 64 || offset + size > 4032) { page += 4096; offset = 0; slot = 0; }
    if (page + 4096 > HEAP_BYTES) fail(2);
    void *p = &heap[page + offset];
    heap[page + 4032 + slot] = 1;
    __asm__ volatile("fence rw,rw" ::: "memory");
#ifndef CONTROL
    p = objid_id(objid_loc(objid_mode(p, 1), slot), 1);
#endif
    p = __builtin_cheri_bounds_set(p, bytes);
    offset += size; ++slot;
    return p;
}
/* Mutate footer bytes using uncoloured authority, for stale-ID fault tests. */
void objid_invalidate_all(void) {
    for (size_t p = 0; p <= page; p += 4096)
        for (unsigned s = 0; s < 64; ++s) heap[p + 4032 + s] = 2;
    __asm__ volatile("fence rw,rw" ::: "memory");
}
void *malloc(size_t n) { return objid_alloc(n); }
void free(void *p) { (void)p; }
void handle_trap(unsigned long cause, unsigned long epc) {
    (void)epc;
#ifdef EXPECT_TRAP
    /* CHERI exception code is 28; reject unrelated exceptions. */
    extern volatile unsigned long *probe_raw;
    if (cause == 28 && *probe_raw == 123) _exit(0x12345);
#endif
    fail(0x100 + (unsigned)cause);
}
int main(void);
void _init(int cid, int nc) { (void)cid; (void)nc; _exit(main()); }
