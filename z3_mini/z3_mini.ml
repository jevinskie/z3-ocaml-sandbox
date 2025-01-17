(* open Ctypes *)

(* module Bindings (F : Ctypes.FOREIGN) =
struct
  open F
let _z3_mini_mk_context =
  foreign "z3_mini_mk_context" (void @-> returning z3_mini_ctx_valty)

end *)
open Ctypes

module Z3_mini_ctx = struct
  type t

  let t : t structure typ = structure "z3_mini_ctx"
  let () = seal t
end
