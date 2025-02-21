#pragma once

#include <string>
#include <variant>
#include <vector>

#include <fmt/core.h>

// A minimal S-expression representation:
// - An atom is stored as a std::string
// - A list is stored as std::vector<SExpr>
struct SExpr {
    using SExprList = std::vector<SExpr>;
    std::variant<std::string, SExprList> value;
};

// Helper to check if an SExpr is an atom:
inline bool is_atom(const SExpr &sexpr) {
    return std::holds_alternative<std::string>(sexpr.value);
}

// Helper to check if an SExpr is a list:
inline bool is_list(const SExpr &sexpr) {
    return std::holds_alternative<SExpr::SExprList>(sexpr.value);
}

// A small ASCII-only function to check for whitespace
constexpr bool is_ascii_whitespace(char c) {
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
}

//--------------------------------------------------
// A simple Parser class that parses from a string_view
//--------------------------------------------------
class SExprParser {
public:
    explicit SExprParser(std::string_view input) : input_(input), pos_(0) {
        skip_whitespace();
    }

    // The main entry to parse a single top-level SExpr.
    // You can call it repeatedly if you expect multiple
    // top-level expressions in sequence.
    SExpr parse() {
        return parse_one();
    }

private:
    std::string_view input_;
    size_t pos_;

    // Skip ASCII whitespace characters
    void skip_whitespace() {
        while (!eof() && is_ascii_whitespace(peek())) {
            ++pos_;
        }
    }

    bool eof() const {
        return pos_ >= input_.size();
    }

    char peek() const {
        return input_[pos_];
    }

    char get() {
        return input_[pos_++];
    }

    // Parse a single SExpr (either an atom or a list).
    // Assumes well-formed input (no error handling).
    SExpr parse_one() {
        skip_whitespace();
        if (eof()) {
            // Return empty atom if we run out of input
            return SExpr{std::string{}};
        }
        return (peek() == '(') ? parse_list() : parse_atom();
    }

    // Parse a list: '(' <sexpr>* ')'
    SExpr parse_list() {
        // consume '('
        get();
        skip_whitespace();

        SExpr::SExprList elements;
        while (!eof() && peek() != ')') {
            elements.push_back(parse_one());
            skip_whitespace();
        }

        // consume ')'
        if (!eof() && peek() == ')') {
            get();
        }

        return SExpr{std::move(elements)};
    }

    // Parse an atom: collect until whitespace or '(' or ')' or end
    SExpr parse_atom() {
        std::string atom;
        while (!eof()) {
            char c = peek();
            if (is_ascii_whitespace(c) || c == '(' || c == ')') {
                break;
            }
            atom.push_back(c);
            get();
        }
        return SExpr{std::move(atom)};
    }
};

template <> struct fmt::formatter<SExpr> {
    constexpr auto parse(fmt::format_parse_context &ctx) -> decltype(ctx.begin()) {
        return ctx.begin();
    }
    constexpr fmt::format_context::iterator format(const SExpr &expr, fmt::format_context &ctx, int indent = 0) const {
        if (is_atom(expr)) {
            fmt::format_to(ctx.out(), "{:>{}}{}\n", "", indent, std::get<std::string>(expr.value));
        } else {
            const auto &list = std::get<SExpr::SExprList>(expr.value);
            fmt::format_to(ctx.out(), "{:>{}}(\n", "", indent);
            for (const auto &e : list) {
                format(e, ctx, indent + 2);
            }
            fmt::format_to(ctx.out(), "{:>{}})\n", "", indent);
        }
        return ctx.out();
    }
};

// void print_sexpr(const SExpr &expr, int indent = 0) {
//     if (is_atom(expr)) {
//         fmt::print("{:>{}}{}\n", "", indent, std::get<std::string>(expr.value));
//     } else {
//         const auto &list = std::get<SExpr::SExprList>(expr.value);
//         fmt::print("{:>{}}(\n", "", indent);
//         for (auto &e : list) {
//             print_sexpr(e, indent + 2);
//         }
//         fmt::print("{:>{}})\n", "", indent);
//     }
// }
