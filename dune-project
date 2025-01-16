(lang dune 3.17)

(name z3_ocaml_sandbox)

(generate_opam_files true)

(source
 (github jevinskie/z3_ocaml_sandbox))

(authors "Jevin Sweval")

(maintainers "Jevin Sweval")

(license BSD-2-Clause)

(documentation https://github.com/jevinskie/z3_ocaml_sandbox)

(package
 (name z3_ocaml_sandbox)
 (synopsis "Z3 OCaml sandbox")
 (description "Z3 OCaml sandbox")
 (depends ocaml dune ctypes))
