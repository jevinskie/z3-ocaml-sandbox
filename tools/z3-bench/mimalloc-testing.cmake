add_library(mimalloc-interposing SHARED dyld-interposing.c)
set_target_properties(mimalloc-interposing PROPERTIES
    C_STANDARD 17
    C_EXTENSIONS ON
    C_STANDARD_REQUIRED ON
    EXCLUDE_FROM_ALL ON
)

add_library(mimalloc-tester-c SHARED mimalloc-tester-dylib-c.c noalloc-stdio.c malloc-wrapped.c mimalloc-fishhook.c fishhook.c dyld-interposing.c mimalloc-extern.c)
# target_link_libraries(mimalloc-tester-c PRIVATE mimalloc-obj mimalloc-interposing)
target_link_libraries(mimalloc-tester-c PRIVATE mimalloc-obj)
set_target_properties(mimalloc-tester-c PROPERTIES
    C_STANDARD 17
    C_EXTENSIONS ON
    C_STANDARD_REQUIRED ON
    EXCLUDE_FROM_ALL ON
)

add_library(mimalloc-interposing-depped SHARED dyld-interposing.c)
target_link_libraries(mimalloc-interposing-depped PRIVATE  "$<LINK_LIBRARY:NEEDED_LIBRARY,$<TARGET_FILE:mimalloc-tester-c>>")
set_target_properties(mimalloc-interposing-depped PROPERTIES
    C_STANDARD 17
    C_EXTENSIONS ON
    C_STANDARD_REQUIRED ON
    EXCLUDE_FROM_ALL ON
)

add_library(mimalloc-tester-c-reexported SHARED dyld-interposing.c)
target_link_libraries(mimalloc-tester-c-reexported PRIVATE  "$<LINK_LIBRARY:REEXPORT_LIBRARY,$<TARGET_FILE:mimalloc-tester-c>>")
set_target_properties(mimalloc-tester-c-reexported PROPERTIES
    C_STANDARD 17
    C_EXTENSIONS ON
    C_STANDARD_REQUIRED ON
    EXCLUDE_FROM_ALL ON
)

add_executable(mimalloc-tester mimalloc-tester-exe.c noalloc-stdio.c)
# target_link_libraries(mimalloc-tester PRIVATE mimalloc-tester-c)
target_link_libraries(mimalloc-tester PRIVATE mimalloc-tester-c-reexported)
# add_dependencies(mimalloc-tester mimalloc-tester-c)
# target_link_options(mimalloc-tester PRIVATE "-Wl,-upward_library,$<TARGET_FILE:mimalloc-tester-c>,-rpath,$<PATH:GET_PARENT_PATH,$<TARGET_FILE:mimalloc-tester-c>>")
set_target_properties(mimalloc-tester PROPERTIES
    C_STANDARD 17
    C_EXTENSIONS ON
    C_STANDARD_REQUIRED ON
    EXCLUDE_FROM_ALL ON
)
