#pragma once
#include "hedley.h"

#include <stddef.h>

HEDLEY_BEGIN_C_DECLS

struct Z3_mini_ctx_impl;

typedef struct Z3_mini_ctx_impl *Z3_mini_ctx;

extern HEDLEY_PUBLIC Z3_mini_ctx Z3_mini_init_context(Z3_mini_ctx ctx, int model);
extern HEDLEY_PUBLIC Z3_mini_ctx Z3_mini_mk_context(int model);
extern HEDLEY_PUBLIC void Z3_mini_deinit_context(Z3_mini_ctx ctx);
extern HEDLEY_PUBLIC void Z3_mini_del_context(Z3_mini_ctx ctx);
extern HEDLEY_PUBLIC int Z3_mini_check_sat(Z3_mini_ctx ctx, const char *smt2);
extern HEDLEY_PUBLIC char *Z3_mini_get_model_with_len(Z3_mini_ctx ctx, const char *smt2, size_t *len);
extern HEDLEY_PUBLIC char *Z3_mini_get_model(Z3_mini_ctx ctx, const char *smt2);
extern HEDLEY_PUBLIC void Z3_mini_del_string(char *s);

HEDLEY_END_C_DECLS
