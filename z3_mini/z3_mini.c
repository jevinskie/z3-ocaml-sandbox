#include "z3_mini.h"

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <z3.h>

struct Z3_mini_ctx_impl {
    Z3_context ctx;
    Z3_solver solver;
};

Z3_mini_ctx Z3_mini_init_context(Z3_mini_ctx ctx, bool model) {
    assert(ctx);
    Z3_config cfg = Z3_mk_config();
    if (!cfg) {
        return NULL;
    }
    Z3_set_param_value(cfg, "model", model ? "true" : "false");
    ctx->ctx = Z3_mk_context(cfg);
    Z3_del_config(cfg);
    if (!ctx->ctx) {
        return NULL;
    }
    ctx->solver = Z3_mk_solver(ctx->ctx);
    if (!ctx->solver) {
        Z3_del_context(ctx->ctx);
        return NULL;
    }
    Z3_solver_inc_ref(ctx->ctx, ctx->solver);
    return ctx;
}

Z3_mini_ctx Z3_mini_mk_context(bool model) {
    Z3_mini_ctx res = malloc(sizeof(*res));
    if (!res) {
        return NULL;
    }
    return Z3_mini_init_context(res, model);
}

void Z3_mini_deinit_context(Z3_mini_ctx ctx) {
    assert(ctx);
    Z3_solver_dec_ref(ctx->ctx, ctx->solver);
    Z3_del_context(ctx->ctx);
}

void Z3_mini_del_context(Z3_mini_ctx ctx) {
    assert(ctx);
    Z3_mini_deinit_context(ctx);
    free(ctx);
}

Z3_mini_lbool Z3_mini_check_sat(Z3_mini_ctx ctx, const char *smt2) {
    assert(ctx && smt2);
    Z3_solver_reset(ctx->ctx, ctx->solver);
    Z3_solver_from_string(ctx->ctx, ctx->solver, smt2);
    return (Z3_mini_lbool)Z3_solver_check(ctx->ctx, ctx->solver);
}

const char *Z3_mini_get_model(Z3_mini_ctx ctx, const char *smt2) {
    Z3_mini_lbool sat = Z3_mini_check_sat(ctx, smt2);
    if (sat != Z3_MINI_L_TRUE) {
        return NULL;
    }
    Z3_model model = Z3_solver_get_model(ctx->ctx, ctx->solver);
    assert(model);
    Z3_model_inc_ref(ctx->ctx, model);
    const char *model_str = Z3_model_to_string(ctx->ctx, model);
    assert(model_str);
    Z3_model_dec_ref(ctx->ctx, model);
    return model_str;
}

const char *Z3_mini_get_model_with_len(Z3_mini_ctx ctx, const char *smt2, size_t *len) {
    assert(len);
    const char *model_str = Z3_mini_get_model(ctx, smt2);
    if (!model_str) {
        *len = 0;
        return NULL;
    }
    *len = strlen(model_str);
    return model_str;
}
