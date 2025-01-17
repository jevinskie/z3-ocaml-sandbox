open Ctypes

type z3_mini_ctx = unit ptr

let z3_mini_ctx : z3_mini_ctx typ = ptr void

module Bindings (F : Ctypes.FOREIGN) = struct
  open F

  let _z3_mini_mk_context =
    foreign "Z3_mini_mk_context" (void @-> returning z3_mini_ctx)
end
