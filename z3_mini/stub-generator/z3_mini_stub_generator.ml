let c_headers = "#include <z3_mini.h>"

let main () =
  let ml_out = open_out "z3_mini_generated.ml" in
  let c_out = open_out "z3_mini_stubs.c" in
  let c_fmt = Format.formatter_of_out_channel c_out in
  let ml_fmt = Format.formatter_of_out_channel ml_out in
  Format.fprintf c_fmt "%s@\n" c_headers;
  Cstubs.write_c c_fmt ~prefix:"z3_mini_stub_"
    (module Z3_mini_bindings.Bindings);
  Cstubs.write_ml ml_fmt ~prefix:"z3_mini_stub_"
    (module Z3_mini_bindings.Bindings);
  Format.pp_print_flush ml_fmt ();
  Format.pp_print_flush c_fmt ();
  close_out ml_out;
  close_out c_out

let () = main ()
