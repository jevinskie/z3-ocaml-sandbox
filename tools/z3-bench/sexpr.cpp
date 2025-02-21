//--------------------------------------------------
// Example usage (compiles only if S_EXPR_PARSER_TEST is defined)
//--------------------------------------------------
#include "sexpr.h"

// A small helper to pretty-print an SExpr using fmt
void print_sexpr(const SExpr &expr, int indent = 0) {
    if (is_atom(expr)) {
        fmt::print("{:>{}}{}\n", "", indent, std::get<std::string>(expr.value));
    } else {
        const auto &list = std::get<SExpr::SExprList>(expr.value);
        fmt::print("{:>{}}(\n", "", indent);
        for (auto &e : list) {
            print_sexpr(e, indent + 2);
        }
        fmt::print("{:>{}})\n", "", indent);
    }
}

int sexpr_main() {
    // Example input in a std::string
    std::string code = R"(
        (declare-fun x () Int)
        (assert (> x 0))
        (check-sat)
    )";

    // Create a parser from our string
    SExprParser parser{code};

    // Parse multiple top-level SExprs
    while (true) {
        SExpr expr = parser.parse();
        // If we got an empty atom, we're done
        if (is_atom(expr) && std::get<std::string>(expr.value).empty()) {
            break;
        }
        print_sexpr(expr);
    }

    return 0;
}
