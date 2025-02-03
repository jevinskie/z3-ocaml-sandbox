add_executable(z3-bench z3-bench.cpp sexpr.cpp)
# target_compile_options(z3-bench PRIVATE ${FAST_CXXFLAGS})
# target_link_options(z3-bench PRIVATE ${FAST_LDFLAGS})
target_link_libraries(z3-bench PRIVATE fmt::fmt folly thread-pool indicators::indicators sha2 libz3)

set_target_properties(z3-bench PROPERTIES
    CXX_STANDARD 23
    CXX_EXTENSIONS ON
    CXX_STANDARD_REQUIRED ON
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
)

install(TARGETS z3-bench)

add_executable(z3-test z3-test.cpp)
target_link_libraries(z3-test PRIVATE fmt::fmt GMP)

set_target_properties(z3-test PROPERTIES
    CXX_STANDARD 23
    CXX_EXTENSIONS ON
    CXX_STANDARD_REQUIRED ON
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
    EXCLUDE_FROM_ALL ON
)
