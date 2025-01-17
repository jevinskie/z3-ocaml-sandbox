let Z3_mini_mk_context =
  foreign "Z3_mini_mk_context" (ptr z3_mini_ctx @-> returning ptr)
