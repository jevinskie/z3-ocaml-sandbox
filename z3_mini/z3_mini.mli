open Ctypes

type z3_mini_ctx = unit ptr

val z3_mini_ctx : z3_mini_ctx typ
val z3_mini_mk_context : unit -> z3_mini_ctx
