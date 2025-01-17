#include "z3_mini.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <z3.h>

struct z3_mini_ctx_impl {
    Z3_context ctx;
    Z3_solver solver;
};

z3_mini_ctx Z3_mini_init_context(z3_mini_ctx ctx) {
    assert(ctx);
    Z3_config cfg = Z3_mk_config();
    if (!cfg) {
        return NULL;
    }
    Z3_set_param_value(cfg, "model", "false");
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
    return ctx;
}

z3_mini_ctx Z3_mini_mk_context(void) {
    z3_mini_ctx res = calloc(1, sizeof(*res));
    if (!res) {
        return NULL;
    }
    return Z3_mini_init_context(res);
}

void Z3_mini_deinit_context(z3_mini_ctx ctx) {
    assert(ctx);
    Z3_del_context(ctx->ctx);
}

void Z3_mini_del_context(z3_mini_ctx ctx) {
    assert(ctx);
    Z3_mini_deinit_context(ctx);
    free(ctx);
}

int Z3_mini_check_sat(z3_mini_ctx ctx, const char *smt2) {
    assert(ctx && smt2);
    Z3_solver_reset(ctx->ctx, ctx->solver);
    Z3_solver_from_string(ctx->ctx, ctx->solver, smt2);
    return Z3_solver_check(ctx->ctx, ctx->solver);
}
