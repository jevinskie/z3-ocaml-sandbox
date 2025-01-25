#pragma once
#include "hedley.h"

#include <stdbool.h>
#include <stddef.h>

HEDLEY_BEGIN_C_DECLS

typedef enum Z3_mini_lbool {
    Z3_MINI_L_FALSE = -1,
    Z3_MINI_L_UNDEF,
    Z3_MINI_L_TRUE,
    Z3_MINI_L_UNINIT,
} Z3_mini_lbool;

struct Z3_mini_ctx_impl;
typedef struct Z3_mini_ctx_impl *Z3_mini_ctx;

extern HEDLEY_PUBLIC Z3_mini_ctx Z3_mini_ctx_nil;

extern HEDLEY_PUBLIC Z3_mini_ctx Z3_mini_init_context(Z3_mini_ctx ctx, bool model);
extern HEDLEY_PUBLIC Z3_mini_ctx Z3_mini_mk_context(bool model);
extern HEDLEY_PUBLIC void Z3_mini_deinit_context(Z3_mini_ctx ctx);
extern HEDLEY_PUBLIC void Z3_mini_del_context(Z3_mini_ctx ctx);
extern HEDLEY_PUBLIC Z3_mini_lbool Z3_mini_check_sat(Z3_mini_ctx ctx, const char *smt2);
extern HEDLEY_PUBLIC Z3_mini_lbool Z3_mini_check_sat_push_pop(Z3_mini_ctx ctx, const char *smt2);
extern HEDLEY_PUBLIC Z3_mini_lbool Z3_mini_check_sat_reset_push_pop(Z3_mini_ctx ctx, const char *smt2);
extern HEDLEY_PUBLIC Z3_mini_lbool Z3_mini_check_sat_reset_push(Z3_mini_ctx ctx, const char *smt2);
extern HEDLEY_PUBLIC const char *Z3_mini_get_model(Z3_mini_ctx ctx, const char *smt2);
extern HEDLEY_PUBLIC const char *Z3_mini_get_model_with_len(Z3_mini_ctx ctx, const char *smt2, size_t *len);
extern HEDLEY_PUBLIC void Z3_mini_set_thread_priority_11(void);
extern HEDLEY_PUBLIC void Z3_mini_set_thread_priority_10(void);

HEDLEY_END_C_DECLS
