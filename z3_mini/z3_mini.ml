open Ctypes

module Z3_mini_ctx = struct
  include C.Type
  include C.Type.Z3_mini_ctx
  include C.Function.Z3_mini_ctx

  type ctx = t

  let nil_ctx = nil
  let to_string c = Nativeint.to_string (raw_address_of_ptr (to_voidp c))

  let with_z3_context model f =
    let c = mk model in
    let result = f c in
    del c;
    result

  let get_model c smt2 =
    let len_ptr = allocate size_t Unsigned.Size_t.zero in
    let model_cstr_ptr = get_model_with_len c smt2 len_ptr in
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
