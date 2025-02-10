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

# mimalloc-extern.c
# /opt/homebrew/opt/ccache/libexec/cc -g -arch arm64 -dynamiclib -Wl,-headerpad_max_install_names  -o tools/z3-bench/libmimalloc-tester-c.dylib -install_name @rpath/libmimalloc-tester-c.dylib 3rdparty/mimalloc/CMakeFiles/mimalloc-obj.dir/src/static.c.o tools/z3-bench/CMakeFiles/mimalloc-tester-c.dir/mimalloc-tester-dylib-c.c.o tools/z3-bench/CMakeFiles/mimalloc-tester-c.dir/noalloc-stdio.c.o tools/z3-bench/CMakeFiles/mimalloc-tester-c.dir/malloc-wrapped.c.o tools/z3-bench/CMakeFiles/mimalloc-tester-c.dir/mimalloc-fishhook.c.o tools/z3-bench/CMakeFiles/mimalloc-tester-c.dir/fishhook.c.o tools/z3-bench/CMakeFiles/mimalloc-tester-c.dir/dyld-interposing.c.o tools/z3-bench/CMakeFiles/mimalloc-tester-c.dir/mimalloc-extern.c.o
add_library(mimalloc-tester-c SHARED mimalloc-tester-dylib-c.c noalloc-stdio.c malloc-wrapped.c mimalloc-fishhook.c fishhook.c dyld-interposing.c mimalloc-extern.c)
target_link_libraries(mimalloc-tester-c PRIVATE mimalloc-obj)
set_target_properties(mimalloc-tester-c PROPERTIES
    C_STANDARD 17
    C_EXTENSIONS ON
    C_STANDARD_REQUIRED ON
    EXCLUDE_FROM_ALL ON
)

add_executable(mimalloc-tester mimalloc-tester-exe.c noalloc-stdio.c)
target_link_libraries(mimalloc-tester PRIVATE mimalloc-tester-c)
# bad: /opt/homebrew/opt/ccache/libexec/cc -g -arch arm64 -Wl,-search_paths_first -Wl,-headerpad_max_install_names -Wl,-upward_library,/Users/jevin/code/sail/git/z3-ocaml-sandbox/build/tools/z3-bench/libmimalloc-tester-c.dylib tools/z3-bench/CMakeFiles/mimalloc-tester.dir/mimalloc-tester-exe.c.o tools/z3-bench/CMakeFiles/mimalloc-tester.dir/noalloc-stdio.c.o -o tools/z3-bench/mimalloc-tester
# good: /opt/homebrew/opt/ccache/libexec/cc -g -arch arm64 -Wl,-search_paths_first -Wl,-headerpad_max_install_names  tools/z3-bench/CMakeFiles/mimalloc-tester.dir/mimalloc-tester-exe.c.o tools/z3-bench/CMakeFiles/mimalloc-tester.dir/noalloc-stdio.c.o -o tools/z3-bench/mimalloc-tester  -Wl,-rpath,/Users/jevin/code/sail/git/z3-ocaml-sandbox/build/tools/z3-bench  tools/z3-bench/libmimalloc-tester-c.dylib
add_dependencies(mimalloc-tester mimalloc-tester-c)
# target_link_options(mimalloc-tester PRIVATE "-Wl,-upward_library,$<TARGET_FILE:mimalloc-tester-c>,-rpath,$<PATH:GET_PARENT_PATH,$<TARGET_FILE:mimalloc-tester-c>>")
set_target_properties(mimalloc-tester PROPERTIES
    C_STANDARD 17
    C_EXTENSIONS ON
    C_STANDARD_REQUIRED ON
    EXCLUDE_FROM_ALL ON
)
