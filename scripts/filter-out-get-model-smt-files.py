#!/usr/bin/env python3

import argparse

from path import Path


def do_want(p: Path) -> bool:
    if not p.is_file():
        return False
    return p.suffix == ".smt2"


def main(in_path: Path, out_path: Path, push_pop: bool = False, check_sat: bool = False) -> None:
    for f in Path(in_path).walkfiles(do_want):
        with open(f) as inf:
            smt = inf.read()
            inf.close()
            if "(get-model)" in smt:
                continue
            lines = []
            for ln in smt.splitlines():
                if ln.startswith(";"):
                    continue
                if ln == "":
                    continue
                if ln == "(push)" and not push_pop:
                    continue
                if ln == "(pop)" and not push_pop:
                    continue
                if ln == "(check-sat)" and not check_sat:
                    continue
                lines.append(ln)
            tf = "\n".join(lines)
            relp = in_path.relpathto(f)
            dp = out_path / relp
            dp.parent.makedirs_p()
            with open(dp, "w") as of:
                of.write(tf)


def get_arg_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser()
    parser.add_argument("-p", "--push-pop", action="store_true", help="Add push/pop statements")
    parser.add_argument("-c", "--check-sat", action="store_true", help="Add check-sat statement")
    parser.add_argument("in_dir", type=Path, help="Input directory")
    parser.add_argument("out_dir", type=Path, help="Output directory")
    return parser


if __name__ == "__main__":
    parser = get_arg_parser()
    args = parser.parse_args()
    main(args.in_dir, args.out_dir, push_pop=args.push_pop, check_sat=args.check_sat)
