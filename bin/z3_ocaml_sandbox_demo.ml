open Z3_ocaml_sandbox

let user = [%get_env_upper "user"]
let () = Format.printf "user: @[%s@]@." user
