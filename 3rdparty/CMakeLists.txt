add_library(thread-pool INTERFACE)
target_include_directories(thread-pool INTERFACE thread-pool/include)

set(FMT_DOC OFF)
set(FMT_TEST OFF)
add_subdirectory(fmt)

set(Z3_USE_LIB_GMP ON)
set(Z3_SINGLE_THREADED ON)
set(Z3_POLLING_TIMER OFF) # TODO: test
set(Z3_BUILD_LIBZ3_SHARED OFF)
set(Z3_BUILD_EXECUTABLE OFF)
add_subdirectory(z3)
