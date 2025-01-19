#!/usr/bin/env python3

import sys

from path import Path


def do_want(p: Path) -> bool:
    if not p.is_file():
        return False
    return p.suffix == ".smt2"


src = Path(sys.argv[1])
dst = Path(sys.argv[2])

for f in Path(src).walkfiles(do_want):
    with open(f, "r") as inf:
        smt = inf.read()
        inf.close()
        if "(get-model)" in smt:
            continue
        lines = []
        for l in smt.splitlines():
            if l.startswith(";"):
                continue
            if l == "":
                continue
            if l == "(push)":
                continue
            if l == "(pop)":
                continue
            if l == "(check-sat)":
                continue
            lines.append(l)
        tf = "\n".join(lines)
        relp = src.relpathto(f)
        dp = dst / relp
        dp.parent.makedirs_p()
        with open(dp, "w") as of:
            of.write(tf)
