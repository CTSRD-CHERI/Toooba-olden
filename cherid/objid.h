/* CHERI-D mode-1 helpers. Encodings match Toooba ISA_Decls_CHERI.bsv.
 * Fixed registers let the ordinary CHERI LLVM13 assembler emit custom ops.
 * Compile-time defaults retain coloured accesses; CONTROL builds omit colouring.
 */
#ifndef OBJID_H
#define OBJID_H
#include <stddef.h>
#include <stdint.h>
#ifdef __CHERI_PURE_CAPABILITY__
#define SETTER(name, encoding) \
static __attribute__((noinline)) void *name(void *p, unsigned long value) { \
    register void *cap __asm__("ca0") = p; \
    register unsigned long arg __asm__("a1") = value; \
    __asm__ volatile(".4byte " #encoding : "+C"(cap) : "r"(arg)); \
    return cap; \
}
SETTER(objid_mode, 0x34b5055b)
SETTER(objid_loc,  0x30b5055b)
SETTER(objid_id,   0x2eb5055b)
#endif
void *objid_alloc(size_t bytes);
void objid_invalidate_all(void);
unsigned long cycle(void);
void fail(unsigned code);
#endif
