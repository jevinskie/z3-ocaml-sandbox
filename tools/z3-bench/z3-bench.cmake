add_library(z3-bench-main SHARED z3-bench.cpp sexpr.cpp)
# target_compile_options(z3-bench-main PRIVATE ${FAST_CXXFLAGS})
# target_link_options(z3-bench-main PRIVATE ${FAST_LDFLAGS})
target_link_libraries(z3-bench-main PRIVATE fmt::fmt thread-pool GMP structopt::structopt argparse::argparse nlohmann_json::nlohmann_json indicators::indicators sha2 libz3)

set_target_properties(z3-bench-main PROPERTIES
    CXX_STANDARD 23
    CXX_EXTENSIONS ON
    CXX_STANDARD_REQUIRED ON
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
    C_STANDARD 17
    C_EXTENSIONS ON
    C_STANDARD_REQUIRED ON
)

install(TARGETS z3-bench-main)

add_executable(z3-bench z3-bench-exe.c)
target_link_libraries(z3-bench z3-bench-main)
# target_compile_options(z3-bench PRIVATE ${FAST_CXXFLAGS})
# target_link_options(z3-bench PRIVATE ${FAST_LDFLAGS})

set_target_properties(z3-bench PROPERTIES
    C_STANDARD 17
    C_EXTENSIONS ON
    C_STANDARD_REQUIRED ON
)

install(TARGETS z3-bench)
