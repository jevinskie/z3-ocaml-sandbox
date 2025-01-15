// https://gist.github.com/qaristote/6910cea4473dd7bc38b0eecd4cd4ae56

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <z3.h>

#undef NDEBUG
#include <assert.h>

static void my_parser_example(char const *const fileName) {
    assert(Z3_open_log("bindings-log.log"));
    Z3_config cfg;
    Z3_context ctx;
    cfg = Z3_mk_config();
    // Z3_set_param_value(cfg, "model", "true");
    Z3_set_param_value(cfg, "trace", "true");
    ctx = Z3_mk_context(cfg);
    Z3_del_config(cfg);

    FILE *file = fopen(fileName, "r");
    char line[8192];

    while (fgets(line, sizeof(line), file)) {
        printf("%s", Z3_eval_smtlib2_string(ctx, line));
    }
    fclose(file);
    Z3_del_context(ctx);
    Z3_close_log();
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        return -1;
    }
    char const *const fileName = argv[1];
    my_parser_example(fileName);
    return 0;
}
