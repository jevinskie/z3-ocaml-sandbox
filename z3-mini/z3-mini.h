#pragma once

#ifdef __cplusplus
extern "C" {
#endif

struct z3_mini_ctx_impl;

typedef struct z3_mini_ctx_impl *z3_mini_ctx;

extern z3_mini_ctx Z3_mini_mk_context(void);
extern void Z3_mini_del_context(z3_mini_ctx ctx);
extern int Z3_mini_check_sat(z3_mini_ctx ctx, const char *smt2);

#ifdef __cplusplus
} // extern "C"
#endif
