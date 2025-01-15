// https://gist.github.com/qaristote/2bb18ab65a3d7974ca9b19fd01e4f41a

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <z3.h>

#undef NDEBUG
#include <assert.h>

static char *open_file(char const *const fileName, size_t *len) {
    FILE *file = fopen(fileName, "r");
    assert(file);
    char *buffer = NULL;
    getdelim(&buffer, len, '\0', file);
    assert(buffer);
    fclose(file);
    return buffer;
}

static void my_parser_example(char const *const fileName, char const *const fileName2) {
    size_t f1_sz = 0;
    char *f1     = open_file(fileName, &f1_sz);
    size_t f2_sz = 0;
    char *f2     = open_file(fileName2, &f2_sz);

    // assert(Z3_open_log("bindings-oneshot-log.log"));
    Z3_config cfg;
    Z3_context ctx;
    cfg = Z3_mk_config();
    // Z3_set_param_value(cfg, "model", "true");
    // Z3_set_param_value(cfg, "trace", "true");
    ctx = Z3_mk_context(cfg);
    Z3_del_config(cfg);

    Z3_solver solver = Z3_mk_solver(ctx);
    // Z3_solver_inc_ref(ctx, solver);
    // printf("solver init: %s\n", Z3_solver_to_string(ctx, solver));

    // printf("f1:\n%s\n", Z3_eval_smtlib2_string(ctx, f1));
    // Z3_solver_eval_smtlib2_string
    Z3_solver_from_string(ctx, solver, f1);
    printf("f1 solver pre-check: %s\n", Z3_solver_to_string(ctx, solver));
    printf("f1 Z3_solver_check: %d\n", Z3_solver_check(ctx, solver));
    printf("f1 solver post-check: %s\n", Z3_solver_to_string(ctx, solver));

    Z3_solver_reset(ctx, solver);
    // printf("solver post-reset: %s\n", Z3_solver_to_string(ctx, solver));
    // Z3_solver_dec_ref(ctx, solver);
    // Z3_solver solver2 = solver;
    // Z3_solver solver2 = Z3_mk_solver(ctx);
    // printf("solver init2: %s\n", Z3_solver_to_string(ctx, solver2));

    // printf("f2:\n%s\n", Z3_eval_smtlib2_string(ctx, f2));
    Z3_solver_from_string(ctx, solver, f2);
    printf("f2 solver pre-check: %s\n", Z3_solver_to_string(ctx, solver));
    printf("f2 Z3_solver_check: %d\n", Z3_solver_check(ctx, solver));
    printf("f2 solver post-check: %s\n", Z3_solver_to_string(ctx, solver));

    Z3_del_context(ctx);
    // Z3_close_log();

    free(f2);
    free(f1);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        return -1;
    }
    char const *const fileName  = argv[1];
    char const *const fileName2 = argv[2];
    my_parser_example(fileName, fileName2);
    return 0;
}
