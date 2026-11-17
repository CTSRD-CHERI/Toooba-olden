#!/usr/bin/env python3
"""Validate driver expectations independently of ObjID hardware and simulation."""
from pathlib import Path
import subprocess
import tempfile
root=Path(__file__).resolve().parent
with tempfile.TemporaryDirectory() as tmp:
    for name in ('treeadd','bisort'):
        sources=([root/'../treeadd/src/node.c',root/'../treeadd/src/par-alloc.c']
                 if name=='treeadd' else [root/'../bisort/src/bitonic.c'])
        for levels in ((5,7,9,11) if name=='treeadd' else (5,7,9)):
            output=Path(tmp)/name
            subprocess.run(['cc','-O2','-DCHERID_STUDY','-DTORONTO',f'-DLEVELS={levels}',
                            str(root/(name+'.c')),str(root/'native_checks.c'),
                            *map(str,sources),'-o',str(output)],check=True)
            subprocess.run([str(output)],check=True)
            print(f'PASS {name} levels={levels}')
