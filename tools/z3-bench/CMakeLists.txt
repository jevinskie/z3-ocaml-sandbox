add_library(z3-bench-main SHARED z3-bench.cpp sexpr.cpp mimalloc-fishhook.c fishhook.c mimalloc-extern.c dyld-interposing.c)
# target_compile_options(z3-bench-main PRIVATE ${FAST_CXXFLAGS})
# target_link_options(z3-bench-main PRIVATE ${FAST_LDFLAGS})
target_link_libraries(z3-bench-main PRIVATE fmt::fmt thread-pool mimalloc-obj indicators::indicators sha2 libz3)

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

add_library(interposing SHARED dyld-interposing.c)
# target_link_libraries(interposing PUBLIC z3-bench-main)
target_link_libraries(interposing PRIVATE z3-bench-main c++abi)
set_target_properties(interposing PROPERTIES
    C_STANDARD 17
    C_EXTENSIONS ON
    C_STANDARD_REQUIRED ON
)
install(TARGETS interposing)

add_executable(z3-bench z3-bench-exe.c)
target_link_libraries(z3-bench interposing z3-bench-main)
# target_compile_options(z3-bench PRIVATE ${FAST_CXXFLAGS})
# target_link_options(z3-bench PRIVATE ${FAST_LDFLAGS})

set_target_properties(z3-bench PROPERTIES
    C_STANDARD 17
    C_EXTENSIONS ON
    C_STANDARD_REQUIRED ON
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

add_library(mimalloc-tester-c SHARED mimalloc-tester-dylib-c.c noalloc-stdio.c)
set_target_properties(mimalloc-tester-c PROPERTIES
    C_STANDARD 17
    C_EXTENSIONS ON
    C_STANDARD_REQUIRED ON
    EXCLUDE_FROM_ALL ON
)

add_executable(mimalloc-tester mimalloc-tester-exe.c noalloc-stdio.c)
target_link_libraries(mimalloc-tester PRIVATE mimalloc-tester-c)
set_target_properties(mimalloc-tester PROPERTIES
    C_STANDARD 17
    C_EXTENSIONS ON
    C_STANDARD_REQUIRED ON
    EXCLUDE_FROM_ALL ON
)
