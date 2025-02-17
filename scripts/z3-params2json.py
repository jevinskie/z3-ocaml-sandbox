#!/usr/bin/env python3

import argparse
import enum
import typing

import rich
from path import Path
from typeguard import typechecked

# from
# z3/scripts/mk_genfile_common.py

###############################################################################
# Functions and data structures for generating ``*_params.hpp`` files from
# ``*.pyg`` files
###############################################################################


class ParamType(enum.IntEnum):
    UINT = 0
    BOOL = 1
    DOUBLE = 2
    STRING = 3
    SYMBOL = 4
    UINT_MAX = 4294967295


TYPE2CPK = {
    ParamType.UINT: "CPK_UINT",
    ParamType.BOOL: "CPK_BOOL",
    ParamType.DOUBLE: "CPK_DOUBLE",
    ParamType.STRING: "CPK_STRING",
    ParamType.SYMBOL: "CPK_SYMBOL",
}
TYPE2CTYPE = {
    ParamType.UINT: "unsigned",
    ParamType.BOOL: "bool",
    ParamType.DOUBLE: "double",
    ParamType.STRING: "char const *",
    ParamType.SYMBOL: "symbol",
}
TYPE2GETTER = {
    ParamType.UINT: "get_uint",
    ParamType.BOOL: "get_bool",
    ParamType.DOUBLE: "get_double",
    ParamType.STRING: "get_str",
    ParamType.SYMBOL: "get_sym",
}

ParamDefault = int | str | bool | float
Param = tuple[str, ParamType, ParamDefault, str]


@typechecked
def pyg_default(p: Param) -> str:
    if p[1] == ParamType.BOOL:
        if p[2]:
            return "true"
        else:
            return "false"
    if p[1] != ParamType.STRING and isinstance(p[2], str):
        raise TypeError(f"got {type(p[2])} not str #1")
    if not isinstance(p[2], str):
        raise TypeError(f"got {type(p[2])} not str")
    return p[2]


@typechecked
def max_memory_param() -> Param:
    return (
        "max_memory",
        ParamType.UINT,
        int(ParamType.UINT_MAX),
        "maximum amount of memory in megabytes",
    )


@typechecked
def max_steps_param() -> Param:
    return ("max_steps", ParamType.UINT, int(ParamType.UINT_MAX), "maximum number of steps")


@typechecked
def pyg2json(pyg_path: Path, pyg: str) -> str:
    # Note OUTPUT_HPP_FILE cannot be a string as we need a mutable variable
    # for the nested function to modify
    OUTPUT_HPP_FILE = []

    # The function below has been nested so that it can use closure to capture
    # the above variables that aren't global but instead local to this
    # function. This avoids use of global state which makes this function safer.
    @typechecked
    def def_module_params(
        module_name: str,
        export: bool,
        params: tuple[Param, ...],
        class_name: str | None = None,
        description: str | None = None,
    ) -> None:
        rich.print(f"pyg_path: {pyg_path}")
        rich.print(f"module_name: {module_name}")
        rich.print(f"export: {export}")
        rich.print(f"class_name: {class_name}")
        sdesc = f"'{description}'" if description is not None else None
        rich.print(f"description: {sdesc}")
        rich.print("params:")
        # rich.inspect(params)
        rich.print(params)
        res = f"{len(params)}\n"
        OUTPUT_HPP_FILE.append(res)

    # Globals to use when executing the ``.pyg`` file.
    pyg_globals = {
        "UINT": ParamType.UINT,
        "BOOL": ParamType.BOOL,
        "DOUBLE": ParamType.DOUBLE,
        "STRING": ParamType.STRING,
        "SYMBOL": ParamType.SYMBOL,
        "UINT_MAX": ParamType.UINT_MAX,
        "max_memory_param": max_memory_param,
        "max_steps_param": max_steps_param,
        # Note that once this function is entered that function
        # executes with respect to the globals of this module and
        # not the globals defined here
        "def_module_params": def_module_params,
    }
    eval(pyg + "\n", pyg_globals, None)
    return "\n".join(OUTPUT_HPP_FILE)


@typechecked
def is_pyg_file(p: Path) -> bool:
    if not p.is_file():
        return False
    return p.suffix == ".pyg"


@typechecked
def main(in_dir_path: Path, out_json_path: Path) -> None:
    res = ""
    cast_is_pyg_file = typing.cast(typing.Callable[[str], bool], is_pyg_file)
    for f in Path(in_dir_path).walkfiles(cast_is_pyg_file):
        with open(f) as inf:
            pyg = inf.read()
            inf.close()
            relpath = f.relpath(in_dir_path)
            res += pyg2json(relpath, pyg)
    with open(out_json_path, "w") as of:
        of.write(res)


@typechecked
def get_arg_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser()
    parser.add_argument("in_dir", type=Path, help="Input directory")
    parser.add_argument("out_json", type=Path, help="Output json file")
    return parser


if __name__ == "__main__":
    parser = get_arg_parser()
    args = parser.parse_args()
    main(args.in_dir, args.out_json)
