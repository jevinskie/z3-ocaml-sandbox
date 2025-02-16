#!/usr/bin/env python3

import argparse
import typing

from path import Path

# from
# z3/scripts/mk_genfile_common.py

###############################################################################
# Functions and data structures for generating ``*_params.hpp`` files from
# ``*.pyg`` files
###############################################################################

UINT = 0
BOOL = 1
DOUBLE = 2
STRING = 3
SYMBOL = 4
UINT_MAX = 4294967295

TYPE2CPK = {
    UINT: "CPK_UINT",
    BOOL: "CPK_BOOL",
    DOUBLE: "CPK_DOUBLE",
    STRING: "CPK_STRING",
    SYMBOL: "CPK_SYMBOL",
}
TYPE2CTYPE = {
    UINT: "unsigned",
    BOOL: "bool",
    DOUBLE: "double",
    STRING: "char const *",
    SYMBOL: "symbol",
}
TYPE2GETTER = {
    UINT: "get_uint",
    BOOL: "get_bool",
    DOUBLE: "get_double",
    STRING: "get_str",
    SYMBOL: "get_sym",
}


def pyg_default(p):
    if p[1] == BOOL:
        if p[2]:
            return "true"
        else:
            return "false"
    return p[2]


def pyg_default_as_c_literal(p):
    if p[1] == BOOL:
        if p[2]:
            return "true"
        else:
            return "false"
    elif p[1] == STRING:
        return '"%s"' % p[2]
    elif p[1] == SYMBOL:
        return 'symbol("%s")' % p[2]
    elif p[1] == UINT:
        return "%su" % p[2]
    else:
        return p[2]


def to_c_method(s):
    return s.replace(".", "_")


def max_memory_param():
    return ("max_memory", UINT, UINT_MAX, "maximum amount of memory in megabytes")


def max_steps_param():
    return ("max_steps", UINT, UINT_MAX, "maximum number of steps")


def pyg2json(pyg: str) -> str:
    # Note OUTPUT_HPP_FILE cannot be a string as we need a mutable variable
    # for the nested function to modify
    OUTPUT_HPP_FILE = []

    # The function below has been nested so that it can use closure to capture
    # the above variables that aren't global but instead local to this
    # function. This avoids use of global state which makes this function safer.
    def def_module_params(module_name, export, params, class_name=None, description=None):
        res = ""
        if class_name is None:
            class_name = f"{module_name}_params"
        res += f"class_name: {class_name}\n"
        if export:
            res += "export=True\n"
        param_i = 0
        for param in params:
            res += f"param_{param_i}: 0: {param[0]} 1 raw: {param[1]} 1: {TYPE2CPK[param[1]]} 2: {param[2]} 3: {param[3]} def: {pyg_default(param)} mod: {module_name}\n"
            param_i += 1
        if export:
            res += f"desc: {description}\n"
        # Generated accessors
        param_i = 0
        for param in params:
            res += f"param_{param_i}: TYPE2CTYPE[param[1]]: {TYPE2CTYPE[param[1]]} to_c_method(param[0]): {to_c_method(param[0])} TYPE2GETTER[param[1]]: {TYPE2GETTER[param[1]]} pyg_default_as_c_literal(param): {pyg_default_as_c_literal(param)}\n"
            param_i += 1
        OUTPUT_HPP_FILE.append(res)

    # Globals to use when executing the ``.pyg`` file.
    pyg_globals = {
        "UINT": UINT,
        "BOOL": BOOL,
        "DOUBLE": DOUBLE,
        "STRING": STRING,
        "SYMBOL": SYMBOL,
        "UINT_MAX": UINT_MAX,
        "max_memory_param": max_memory_param,
        "max_steps_param": max_steps_param,
        # Note that once this function is entered that function
        # executes with respect to the globals of this module and
        # not the globals defined here
        "def_module_params": def_module_params,
    }
    eval(pyg + "\n", pyg_globals, None)
    return "\n".join(OUTPUT_HPP_FILE)


def is_pyg_file(ps: str) -> bool:
    p = typing.cast(Path, ps)
    if not p.is_file():
        return False
    return p.suffix == ".pyg"


def main(in_dir_path: Path, out_json_path: Path) -> None:
    res = ""
    for f in Path(in_dir_path).walkfiles(is_pyg_file):
        with open(f) as inf:
            pyg = inf.read()
            inf.close()
            res += pyg2json(pyg)
    with open(out_json_path, "w") as of:
        of.write(res)


def get_arg_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser()
    parser.add_argument("in_dir", type=Path, help="Input directory")
    parser.add_argument("out_json", type=Path, help="Output json file")
    return parser


if __name__ == "__main__":
    parser = get_arg_parser()
    args = parser.parse_args()
    main(args.in_dir, args.out_json)
