add_library(thread-pool INTERFACE)
target_include_directories(thread-pool INTERFACE thread-pool/include)

add_library(sha2 INTERFACE)
target_include_directories(sha2 INTERFACE sha-2-scoped-include)

set(FMT_DOC OFF)
set(FMT_TEST OFF)
add_subdirectory(fmt)

set(Z3_USE_LIB_GMP ON)
set(Z3_SINGLE_THREADED ON)
set(Z3_POLLING_TIMER OFF) # TODO: test
set(Z3_BUILD_LIBZ3_SHARED OFF)
set(Z3_BUILD_EXECUTABLE OFF)
add_subdirectory(z3)
target_include_directories(libz3 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/z3/src/api")
