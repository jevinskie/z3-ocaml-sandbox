(lang dune 3.17)

(using ctypes 0.3)

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
 (depends ocaml dune domainslib))

; (package
;  (name z3_mini_bindings)
;  (synopsis "Z3 Mini FFI")
;  (description "Z3 Mini FFI")
;  (depends ocaml dune ctypes))

; (package
;  (name z3_mini_stub_generator)
;  (synopsis "Z3 Mini FFI stubs gen")
;  (description "Z3 Mini FFI stubs gen")
;  (depends ocaml dune ctypes))

(package
 (name z3_mini)
 (synopsis "Z3 Mini")
 (description "Z3 Mini")
 (depends ocaml dune ctypes))
