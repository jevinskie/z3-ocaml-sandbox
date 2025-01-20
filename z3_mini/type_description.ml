open Ctypes
open Ctypes_static
open Ctypes_ptr

module Types (F : Ctypes.TYPE) = struct
  open F

  module Z3_mini_ctx : sig
    type t

    val t : t typ
  end = struct
    type t = unit ptr

    let t : t typ = typedef (ptr void) "Z3_mini_ctx"
  end

  let nil : unit ptr = CPointer (Fat.make ~managed:None ~reftyp:Void Raw.null)
  let lbool_false = constant "Z3_MINI_L_FALSE" int64_t
  let lbool_undef = constant "Z3_MINI_L_UNDEF" int64_t
  let lbool_true = constant "Z3_MINI_L_TRUE" int64_t
  let lbool_uninit = constant "Z3_MINI_L_UNINIT" int64_t

  type lbool = False | Undef | True | Uninit

  let lbool =
    enum "Z3_mini_lbool"
      [
        (False, lbool_false);
        (Undef, lbool_undef);
        (True, lbool_true);
        (Uninit, lbool_uninit);
      ]
end
