cmake_minimum_required(VERSION 3.31)
cmake_policy(SET CMP0077 NEW)

project(z3-sandbox C CXX) # need C for e.g. CMAKE_C_COMPILER to work

message(STATUS "CMAKE_MODULE_PATH orig: ${CMAKE_MODULE_PATH}")
list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
message(STATUS "CMAKE_MODULE_PATH new: ${CMAKE_MODULE_PATH}")

message(STATUS "CMAKE_SYSTEM_PREFIX_PATH orig: ${CMAKE_SYSTEM_PREFIX_PATH}")
list(REMOVE_ITEM CMAKE_SYSTEM_PREFIX_PATH /opt/homebrew /usr/local)
list(APPEND CMAKE_SYSTEM_PREFIX_PATH /opt/homebrew/opt/libevent /opt/homebrew/opt/double-conversion /opt/homebrew/opt/fast_float)
message(STATUS "CMAKE_SYSTEM_PREFIX_PATH new: ${CMAKE_SYSTEM_PREFIX_PATH}")

include(ExternalProject)

option(FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." ON)

if (${FORCE_COLORED_OUTPUT})
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
       set(COLOR_DIAGS -fdiagnostics-color=always)
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
       set(COLOR_DIAGS -fcolor-diagnostics)
    else()
        set(COLOR_DIAGS)
    endif ()
endif ()

cmake_host_system_information(RESULT NCPUS
                              QUERY NUMBER_OF_LOGICAL_CORES)

set(BUILD_SHARED_LIBS OFF)

if(BUILD_SHARED_LIBS)
    set(CONFIGURE_SHARED --enable-shared)
    set(CONFIGURE_STATIC --disable-static)
    set(LIB_EXT ${CMAKE_SHARED_LIBRARY_SUFFIX})
    set(LIB_TYPE_CMAKE SHARED)
else()
    set(CONFIGURE_SHARED --disable-shared)
    set(CONFIGURE_STATIC --enable-static)
    set(LIB_EXT ${CMAKE_STATIC_LIBRARY_SUFFIX})
    set(LIB_TYPE_CMAKE STATIC)
endif()

find_package(Boost CONFIG REQUIRED)
find_package(Threads REQUIRED)


set(BUILD_AS_SUBPROJECT GMP OpenSSL ZLIB BZip2)
# set(BUILD_AS_SUBPROJECT GMP LIBSODIUM Libsodium LIBAIO LibAIO LIBIBERTY Libiberty LIBURING LibUring Snappy LibDwarf DWARF LIBDWARF LIBUNWIND LibUnwind ZSTD Zstd LZ4 LibLZMA BZip2 OpenSSL ZLIB libevent gflags Gflags Glog DoubleConversion event LibEvent)
# set(ALLOWED_FIND_PKGS Threads Threads::Threads boost_headers boost_headers::headers Boost Boost::headers)

# function(orig_find_package)
#     _find_package(${ARGV})
# endfunction()


macro(find_package)
    # if((NOT ${ARGV0} IN_LIST BUILD_AS_SUBPROJECT) OR (${ARGV0} IN_LIST ALLOWED_FIND_PKGS))
    if(NOT ${ARGV0} IN_LIST BUILD_AS_SUBPROJECT)
        _find_package(${ARGV})
    endif()
endmacro()

# NOP out subproject use of cmake_minimum_required
macro(cmake_minimum_required)
endmacro()

set(FAST_CISH_FLAGS -O3)
set(FAST_CFLAGS ${FAST_CISH_FLAGS} ${COLOR_DIAGS} -fvectorize -flto=full -fvisibility=hidden -fvisibility-inlines-hidden -fvisibility-inlines-hidden-static-local-var -fdata-sections -ffunction-sections -march=native)
set(FAST_CXXFLAGS ${FAST_CFLAGS} -stdlib=libc++ -fno-rtti)
set(FAST_LDFLAGS ${FAST_CXXFLAGS})

# add_compile_options(-O0 -glldb)
# add_link_options(-O0 -glldb)

add_subdirectory(3rdparty)
add_subdirectory(tools)
