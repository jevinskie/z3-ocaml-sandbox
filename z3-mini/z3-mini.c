#include "z3-mini.h"

#include <stdlib.h>
#include <string.h>

#include <sys/_types/_null.h>
#include <z3.h>
#include <z3_api.h>

struct z3_mini_ctx_impl {
    Z3_context ctx;
    Z3_solver solver;
};

z3_mini_ctx Z3_mini_mk_context(void) {
    z3_mini_ctx res = calloc(1, sizeof(*res));
    if (!res) {
        return NULL;
    }
    Z3_config cfg = Z3_mk_config();
    if (!cfg) {
        free(res);
        return NULL;
    }
    // Z3_set_param_value(cfg, "model", "true");
    // Z3_set_param_value(cfg, "trace", "true");
    res->ctx = Z3_mk_context(cfg);
    Z3_del_config(cfg);
    if (!res->ctx) {
        free(res);
        return NULL;
    }
    res->solver = Z3_mk_solver(res->ctx);
    if (!res->solver) {
        Z3_del_context(res->ctx);
        free(res);
        return NULL;
    }
    return res;
}

void Z3_mini_del_context(z3_mini_ctx ctx) {
    if (!ctx) {
        abort();
    }
    Z3_del_context(ctx->ctx);
    free(ctx);
}

int Z3_mini_check_sat(z3_mini_ctx ctx, const char *smt2) {
    if (!ctx || !smt2) {
        abort();
    }
    Z3_solver_reset(ctx->ctx, ctx->solver);
    Z3_solver_from_string(ctx->ctx, ctx->solver, smt2);
    return Z3_solver_check(ctx->ctx, ctx->solver);
}
