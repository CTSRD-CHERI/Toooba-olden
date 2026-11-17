# CHERI-D bare-metal Olden workloads

These drivers use the original treeadd and bisort algorithms with mode-1
ObjID-coloured allocations, independent of a private LLVM fork. The custom
instruction helpers encode Toooba's CSetTmode (0x1a), CSetTloc (0x18), and
CSetMTE (0x17). They were validated using CTSRD CHERI LLVM 13 revision
`c45239938ef27816bf169758f5f1b32639f6303c` and the bare-metal purecap SDK.
The public LLVM `ilmt` branch also contains custom-instruction support, but
these builds do not require its assembler or claim it is encoding-compatible.

## Build

Run from the source repository root, with Python 3 and a CHERI SDK:

```
python3 cherid/build.py --sdk /path/to/sdk --benchmark treeadd --levels 9 --output build/treeadd-objid.bin
python3 cherid/build.py --sdk /path/to/sdk --benchmark bisort --levels 7 --output build/bisort-objid.bin
python3 cherid/build.py --sdk /path/to/sdk --benchmark probe --output build/probe.bin
python3 cherid/build.py --sdk /path/to/sdk --negative --output build/negative-load.bin
python3 cherid/build.py --sdk /path/to/sdk --negative-store --output build/negative-store.bin
```

The SDK must contain `bin/clang`, `bin/ld.lld`, resource header
`cheri_init_globals.h`, and
`baremetal/baremetal-riscv64-purecap/riscv64-unknown-elf`.
The startup assembly and linker script are derived from Toooba-mibench2.
`.bin` outputs are ELF executables, not flat binary images. Build flags map
source and SDK debug paths to stable names to avoid recording local paths in
the committed ELF files.

The allocator packs up to 64 objects into each 4-KiB page; the last 64 bytes
store one ID byte per object. Objects are 16-byte aligned and bounded, with
mode=1, slot as Tloc, and nonzero expected ID=1. Allocations over 4032 bytes or
exhausting the fixed 2-MiB heap fail explicitly. `free` is a no-op; these drivers
do not need allocation reuse. The hardware's mode-1 check compares the low ID
bit, so this is not an eight-bit uniqueness test.

`--control` keeps the same packing and footer writes but omits colouring;
it is an experimental control, not a CHERI-D binary to publish in the suite.
`--algorithm-only` excludes allocation from timing; treeadd also performs an
untimed validation traversal first. `--repeats` sets treeadd traversals (default
4). End-to-end timing includes construction and correctness checks.

## Validation and termination

Treeadd verifies every traversal sums to the number of allocated nodes.
Bisort validates traversal order, node count, and sum including the extra
sort value, in both directions. The two sorts use separate identically
initialised trees: the inherited algorithm fails a descending re-sort of an
already ascending tree at 511 nodes, including in a native host build. The
sort algorithm itself is unchanged by this port. Larger bisort inputs also
fail native validation (observed at levels=11), so this driver rejects
levels greater than 9; the study uses levels=7.

The probe verifies normal load/store access. Negative variants set expected
ID=2 (opposite low bit to stored 1), require a CHERI exception, and check that
the underlying value remains 123. Zero would bypass the ID check. The negative
store test therefore also verifies that a faulting store does not write data.
The observed exception is the core's ObjID `cheriExcSealViolation` path.

Run ELF-to-hex in the isolated simulator run directory so `Mem.hex` and its
`symbol_table.txt` remain together, then run Bluesim with `+tohost`.
Successful benchmarks return measured body cycles in `tohost >> 1`.
Probe success returns 0; negative-probe success returns `0x12345`.
Failures return `0x40000000 | error`; treat assertions, timeouts, unexpected
exceptions, and these high-bit results as failures even if the process exits 0.
There is no dependency on console output or host wall-clock timing.

Binaries belong in `CTSRD-CHERI/toooba-sim-benchmarks:CHERID` after validation;
keep generated ELF files and simulation outputs out of this source repository.
