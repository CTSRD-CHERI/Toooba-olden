#!/usr/bin/env python3
"""Build isolated bare-metal purecap CHERI-D workloads, without a private SDK."""
import argparse
from pathlib import Path
import subprocess
p=argparse.ArgumentParser()
p.add_argument('--sdk',type=Path,default=Path('/local/scratch/jdw57/cheri/output/sdk'))
p.add_argument('--levels',type=int,default=9)
p.add_argument('--repeats',type=int,default=4)
p.add_argument('--algorithm-only',action='store_true')
p.add_argument('--control',action='store_true')
p.add_argument('--negative',action='store_true')
p.add_argument('--negative-store',action='store_true')
p.add_argument('--benchmark',choices=['probe','treeadd','bisort'],default='probe')
p.add_argument('--output',type=Path,required=True)
a=p.parse_args(); assert 2<=a.levels<=14 and 1<=a.repeats<=100
if a.benchmark=="bisort" and a.levels>9: p.error("bisort is validated only through levels=9")
root=Path(__file__).resolve().parent
a.output=a.output.resolve(); a.output.parent.mkdir(parents=True,exist_ok=True)
sysroot=a.sdk/'baremetal/baremetal-riscv64-purecap/riscv64-unknown-elf'
cmd=[str(a.sdk/'bin/clang'),'--target=riscv64-unknown-elf','-march=rv64imafdcxcheri','-mabi=l64pc128d','-mcmodel=medium','-mno-relax','--sysroot='+str(sysroot),'-O2','-g','-ffreestanding','-fno-builtin','-nostdlib','-fuse-ld=lld','-Wl,-T,'+str(root/'test.ld'),'-I'+str(root)]
cmd+=['-DCHERID_STUDY','-DTORONTO','-DLEVELS='+str(a.levels),'-DREPEATS='+str(a.repeats)]
if a.algorithm_only:cmd+=['-DALGORITHM_ONLY']
if a.benchmark=='treeadd':cmd+=[str(root/'../treeadd/src/node.c'),str(root/'../treeadd/src/par-alloc.c')]
if a.benchmark=='bisort':cmd+=[str(root/'../bisort/src/bitonic.c')]
if a.control:cmd+=['-DCONTROL']
if a.negative or a.negative_store:cmd+=['-DEXPECT_TRAP']
if a.negative_store:cmd+=['-DNEGATIVE_STORE']
cmd+=[str(root/'crt.S'),str(root/'runtime.c'),str(root/(a.benchmark+'.c')),'-o',str(a.output)]
print(' '.join(cmd),flush=True);subprocess.run(cmd,check=True)
