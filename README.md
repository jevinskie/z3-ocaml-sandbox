# z3-ocaml-sandbox
Playing around with z3 in OCaml

## Build
```shell
cmake -W Dev -G Ninja .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_INSTALL_PREFIX=$HOME/base/z3-bench -DCMAKE_MODULE_PATH=/opt/homebrew/opt/boost/lib/cmake --fresh
```
