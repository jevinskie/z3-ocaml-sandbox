open Ctypes

module CGContext = struct
  include C.Type.Z3_mini_ctx
  include C.Function.Z3_mini_ctx

  let t_of_raw_address addr = coerce (ptr void) t (ptr_of_raw_address addr)
end
