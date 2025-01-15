// https://gist.github.com/qaristote/6910cea4473dd7bc38b0eecd4cd4ae56

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <z3.h>

static void my_parser_example(char const *const fileName) {
    Z3_config cfg;
    Z3_context ctx;
    cfg = Z3_mk_config();
    Z3_set_param_value(cfg, "model", "true");
    ctx = Z3_mk_context(cfg);
    Z3_del_config(cfg);

    FILE *file = fopen(fileName, "r");
    char line[8192];

    while (fgets(line, sizeof(line), file)) {
        printf("%s", Z3_eval_smtlib2_string(ctx, line));
    }
    fclose(file);
    Z3_del_context(ctx);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        return -1;
    }
    char *foo                  = malloc(1024);
    char const *const fileName = argv[1];
    my_parser_example(fileName);
    return 0;
}
