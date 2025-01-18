open Ctypes

(* This Types_generated module is an instantiation of the Types
	 functor defined in the type_description.ml file. It's generated by
	 a C program that Dune creates and runs behind the scenes. *)
module Types = Types_generated

module Functions (F : Ctypes.FOREIGN) = struct
  open Types
  open F

  module Z3_mini_ctx = struct
    include Z3_mini_ctx (* from Types *)

    let fill_rect = foreign "Z3_mini_del_context" (t @-> returning void)
  end
end
