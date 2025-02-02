#include <fmt/format.h>
#include <gmp.h>

int main(void) {
    fmt::print("mpz_init: {}\n", fmt::ptr(mpz_init));
    return 0;
}
