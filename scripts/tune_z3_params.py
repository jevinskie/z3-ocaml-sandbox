#!/usr/bin/env python3

import argparse
import typing

import rich
import rich.pretty
from path import Path
from rich import print
from typeguard import typechecked
from z3_params import ModuleParams, is_pyg_file, pyg2json

print("<do tuning here>")


@typechecked
def main(smt2_dir: Path, z3_dir: Path, work_dir: Path) -> None:
    res: list[ModuleParams] = []
    cast_is_pyg_file = typing.cast(typing.Callable[[str], bool], is_pyg_file)
    for f in Path(z3_dir).walkfiles(cast_is_pyg_file):
        relpath = f.relpath(z3_dir)
        mp = pyg2json(f, relpath)
        print("mp:")
        rich.print(mp)
        res.append(mp)


@typechecked
def get_arg_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser()
    parser.add_argument("-s", "--smt", required=True, dest="smt2_dir", type=Path, help="smt2 directory")
    parser.add_argument("-z", "--z3", required=True, dest="z3_dir", type=Path, help="z3 directory")
    parser.add_argument("-w", "--work", required=True, dest="work_dir", type=Path, help="work directory")
    return parser


if __name__ == "__main__":
    parser = get_arg_parser()
    args = parser.parse_args()
    main(args.smt2_dir, args.z3_dir, args.work_dir)
