(lang dune 3.17)

(using ctypes 0.3)

(name z3_ocaml_sandbox)

(generate_opam_files true)

(implicit_transitive_deps false)

(map_workspace_root false)

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
 (depends ocaml dune base domainslib sexplib))

(package
 (name z3_mini)
 (synopsis "Z3 Mini")
 (description "Z3 Mini")
 (depends ocaml dune ctypes integers))
