open Ctypes

module Z3_mini_ctx = struct
  include C.Type
  include C.Type.Z3_mini_ctx
  include C.Function.Z3_mini_ctx

  let get_model ctx smt2 =
    let len_ptr = allocate size_t Unsigned.Size_t.zero in
    let model_cstr_ptr = get_model_with_len ctx smt2 len_ptr in
    let length = Unsigned.Size_t.to_int !@len_ptr in
    string_from_ptr model_cstr_ptr ~length

  let lbool_to_string lb =
    match lb with False -> "False" | Undef -> "Undef" | True -> "True"
end
