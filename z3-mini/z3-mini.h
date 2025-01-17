#pragma once
#include "hedley.h"

HEDLEY_BEGIN_C_DECLS

struct z3_mini_ctx_impl;

typedef struct z3_mini_ctx_impl *z3_mini_ctx;

extern HEDLEY_PUBLIC z3_mini_ctx Z3_mini_init_context(z3_mini_ctx ctx);
extern HEDLEY_PUBLIC z3_mini_ctx Z3_mini_mk_context(void);
extern HEDLEY_PUBLIC void Z3_mini_deinit_context(z3_mini_ctx ctx);
extern HEDLEY_PUBLIC void Z3_mini_del_context(z3_mini_ctx ctx);
extern HEDLEY_PUBLIC int Z3_mini_check_sat(z3_mini_ctx ctx, const char *smt2);

HEDLEY_END_C_DECLS
