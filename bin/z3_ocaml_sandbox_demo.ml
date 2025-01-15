open Z3

let () = print_endline (Z3_ocaml_sandbox.hello ());
let () = print_endline (Z3_ocaml_sandbox.hello ());


let cfg = [("model", "true");] in
let ctx = (mk_context cfg) in
let foo = Format.printf "lol\n" in
foo ctx
