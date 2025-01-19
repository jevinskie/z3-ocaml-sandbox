open Ctypes

module Z3_mini_ctx = struct
  include C.Type
  include C.Type.Z3_mini_ctx
  include C.Function.Z3_mini_ctx

  let with_z3_context model f =
    let ctx = mk model in
    let result = f ctx in
    del ctx;
    result

  let get_model ctx smt2 =
    let len_ptr = allocate size_t Unsigned.Size_t.zero in
    let model_cstr_ptr = get_model_with_len ctx smt2 len_ptr in
    let length = Unsigned.Size_t.to_int !@len_ptr in
    string_from_ptr model_cstr_ptr ~length

  let lbool_to_string lb =
    match lb with
    | False -> "False"
    | Undef -> "Undef"
    | True -> "True"
    | Uninit -> "Uninit"

  let pp_lbool fmt = function
    | False -> Format.fprintf fmt "False"
    | Undef -> Format.fprintf fmt "Undef"
    | True -> Format.fprintf fmt "True"
    | Uninit -> Format.fprintf fmt "Uninit"
end
