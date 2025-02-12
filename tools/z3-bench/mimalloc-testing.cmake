if (CMAKE_C_COMPILER_ID STREQUAL "AppleClang")
  set(CMAKE_LINK_LIBRARY_USING_UPWARD_LIBRARY
      "PATH{-Wl,-upward_library,<LIBRARY>}NAME{LINKER:-Wl,-upward-l<LIB_ITEM>}"
  )
  set(CMAKE_LINK_LIBRARY_USING_UPWARD_LIBRARY_SUPPORTED TRUE)
endif()

add_library(mimalloc-tester-c SHARED mimalloc-tester-dylib-c.c noalloc-stdio.c mimalloc-extern.c link-in-libmalloc.c)
# target_link_libraries(mimalloc-tester-c PRIVATE mimalloc-obj mimalloc-interposing)
target_link_libraries(mimalloc-tester-c PRIVATE mimalloc-obj)
set_target_properties(mimalloc-tester-c PROPERTIES
    C_STANDARD 17
    C_EXTENSIONS ON
    C_STANDARD_REQUIRED ON
    EXCLUDE_FROM_ALL ON
)

add_library(mimalloc-interposing-depped SHARED dyld-interposing.c)
target_link_libraries(mimalloc-interposing-depped PRIVATE  "$<LINK_LIBRARY:NEEDED_LIBRARY,mimalloc-tester-c>")
set_target_properties(mimalloc-interposing-depped PROPERTIES
    C_STANDARD 17
    C_EXTENSIONS ON
    C_STANDARD_REQUIRED ON
    EXCLUDE_FROM_ALL ON
)

add_library(mimalloc-tester-c-reexported SHARED dyld-interposing.c)
target_link_libraries(mimalloc-tester-c-reexported PRIVATE  "$<LINK_LIBRARY:REEXPORT_LIBRARY,mimalloc-tester-c>")
set_target_properties(mimalloc-tester-c-reexported PROPERTIES
    C_STANDARD 17
    C_EXTENSIONS ON
    C_STANDARD_REQUIRED ON
    EXCLUDE_FROM_ALL ON
)

add_library(mimalloc-interposing SHARED dyld-interposing.c)
target_link_libraries(mimalloc-interposing PRIVATE  "$<LINK_LIBRARY:UPWARD_LIBRARY,mimalloc-tester-c>")
set_target_properties(mimalloc-interposing PROPERTIES
    C_STANDARD 17
    C_EXTENSIONS ON
    C_STANDARD_REQUIRED ON
    EXCLUDE_FROM_ALL ON
)

add_library(mimalloc-umbrella SHARED mimalloc-umbrella.c)
target_link_libraries(mimalloc-umbrella PRIVATE mimalloc-interposing "$<LINK_LIBRARY:REEXPORT_LIBRARY,mimalloc-tester-c>")
set_target_properties(mimalloc-umbrella PROPERTIES
    C_STANDARD 17
    C_EXTENSIONS ON
    C_STANDARD_REQUIRED ON
    EXCLUDE_FROM_ALL ON
)

add_executable(mimalloc-tester mimalloc-tester-exe.c noalloc-stdio.c)
# target_link_libraries(mimalloc-tester PRIVATE mimalloc-tester-c)
# target_link_libraries(mimalloc-tester PRIVATE mimalloc-tester-c-reexported)
# target_link_libraries(mimalloc-tester PRIVATE mimalloc-umbrella)
# add_dependencies(mimalloc-tester mimalloc-tester-c)
# target_link_options(mimalloc-tester PRIVATE "-Wl,-upward_library,$<TARGET_FILE:mimalloc-tester-c>,-rpath,$<PATH:GET_PARENT_PATH,$<TARGET_FILE:mimalloc-tester-c>>")
set_target_properties(mimalloc-tester PROPERTIES
    C_STANDARD 17
    C_EXTENSIONS ON
    C_STANDARD_REQUIRED ON
    EXCLUDE_FROM_ALL ON
)

add_library(symbol-stubs SHARED symbol-stubs.c)
# one weird trick 2 link mach-o dylibs against executables
target_link_options(symbol-stubs PRIVATE "-Wl,-install_name,@rpath/mimalloc-tester-trick")
set_target_properties(mimalloc-tester PROPERTIES
    C_STANDARD 17
    C_EXTENSIONS ON
    C_STANDARD_REQUIRED ON
    EXCLUDE_FROM_ALL ON
)

add_library(dyld-interposing SHARED dyld-interposing.c noalloc-stdio.c link-in-libmalloc.c)
target_compile_options(dyld-interposing PRIVATE "-mno-stack-arg-probe" "-fno-stack-check" "-fno-stack-protector")
target_link_libraries(dyld-interposing PRIVATE "$<LINK_LIBRARY:UPWARD_LIBRARY,symbol-stubs>")
# target_link_options(dyld-interposing PRIVATE "-Wl,-needed_library,/usr/lib/system/libsystem_malloc.dylib")
set_target_properties(dyld-interposing PROPERTIES
    C_STANDARD 17
    C_EXTENSIONS ON
    C_STANDARD_REQUIRED ON
    EXCLUDE_FROM_ALL ON
)

# add_executable(mimalloc-tester-trick mimalloc-tester-exe.c mimalloc-tester-dylib-c.c noalloc-stdio.c malloc-wrapped.c mimalloc-extern.c link-in-libmalloc.c fishhook.c mimalloc-fishhook.c)
# target_link_libraries(mimalloc-tester-trick PRIVATE mimalloc-obj dyld-interposing)
# # target_link_libraries(mimalloc-tester-trick PRIVATE mimalloc-obj)
# target_compile_definitions(mimalloc-tester-trick PUBLIC malloc=mi_malloc free=mi_free calloc=mi_calloc)
# # target_link_options(mimalloc-tester-trick PRIVATE "-Wl,-needed_library,/usr/lib/system/libsystem_malloc.dylib")
# set_target_properties(mimalloc-tester-trick PROPERTIES
#     C_STANDARD 17
#     C_EXTENSIONS ON
#     C_STANDARD_REQUIRED ON
#     EXCLUDE_FROM_ALL ON
# )

add_library(mimalloc-tester-trick-lib SHARED mimalloc-tester-dylib-c.c noalloc-stdio.c malloc-wrapped.c mimalloc-extern.c link-in-libmalloc.c fishhook.c mimalloc-fishhook.c)
target_link_libraries(mimalloc-tester-trick-lib PRIVATE mimalloc-obj dyld-interposing)
# target_link_libraries(mimalloc-tester-trick-lib PRIVATE mimalloc-obj)
target_compile_definitions(mimalloc-tester-trick-lib PUBLIC malloc=mi_malloc free=mi_free calloc=mi_calloc)
# target_link_options(mimalloc-tester-trick-lib PRIVATE "-Wl,-needed_library,/usr/lib/system/libsystem_malloc.dylib")
set_target_properties(mimalloc-tester-trick-lib PROPERTIES
    C_STANDARD 17
    C_EXTENSIONS ON
    C_STANDARD_REQUIRED ON
    EXCLUDE_FROM_ALL ON
)

add_executable(mimalloc-tester-trick mimalloc-tester-exe.c noalloc-stdio.c)
target_link_libraries(mimalloc-tester-trick PRIVATE mimalloc-tester-trick-lib)
# target_link_libraries(mimalloc-tester-trick PRIVATE mimalloc-obj)
# target_compile_definitions(mimalloc-tester-trick PUBLIC malloc=mi_malloc free=mi_free calloc=mi_calloc)
# target_link_options(mimalloc-tester-trick PRIVATE "-Wl,-needed_library,/usr/lib/system/libsystem_malloc.dylib")
set_target_properties(mimalloc-tester-trick PROPERTIES
    C_STANDARD 17
    C_EXTENSIONS ON
    C_STANDARD_REQUIRED ON
    EXCLUDE_FROM_ALL ON
)
