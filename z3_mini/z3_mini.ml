open Ctypes

module Z3_mini_ctx = struct
  include C.Type.Z3_mini_ctx
  include C.Function.Z3_mini_ctx

  let get_model_new ctx smt2 =
    let len_ptr = allocate size_t Unsigned.Size_t.zero in
    let model_cstr_ptr = get_model_with_len ctx smt2 len_ptr in
    let length = Unsigned.Size_t.to_int !@len_ptr in
    let str = string_from_ptr model_cstr_ptr ~length in
    del_string model_cstr_ptr;
    str

  let t_of_raw_address addr = coerce (ptr void) t (ptr_of_raw_address addr)
end
