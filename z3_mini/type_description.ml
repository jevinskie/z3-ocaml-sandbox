module Types (F : Ctypes.TYPE) = struct
  open Ctypes
  open F

  module Z3_mini_ctx : sig
    type t

    val t : t typ
  end = struct
    type t = unit ptr

    let t : t typ = typedef (ptr void) "z3_mini_ctx"
  end
end
