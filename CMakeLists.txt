cmake_minimum_required(VERSION 3.31)
cmake_policy(SET CMP0077 NEW)

project(z3-sandbox C CXX) # need C for e.g. CMAKE_C_COMPILER to work

message(STATUS "CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")

set(SMTFUN_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

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

set(BUILD_AS_SUBPROJECT GMP)

macro(find_package)
    if(NOT ${ARGV0} IN_LIST BUILD_AS_SUBPROJECT)
        _find_package(${ARGV})
    endif()
endmacro()

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

function (_get_all_cmake_targets out_var current_dir)
    get_property(targets DIRECTORY ${current_dir} PROPERTY BUILDSYSTEM_TARGETS)
    get_property(subdirs DIRECTORY ${current_dir} PROPERTY SUBDIRECTORIES)

    foreach(subdir ${subdirs})
        _get_all_cmake_targets(subdir_targets ${subdir})
        list(APPEND targets ${subdir_targets})
    endforeach()

    set(${out_var} ${targets} PARENT_SCOPE)
endfunction()

# Run at end of top-level CMakeLists
_get_all_cmake_targets(all_targets ${CMAKE_CURRENT_LIST_DIR})
message(WARNING "targets: ${all_targets}")

## https://stackoverflow.com/questions/32183975/how-to-print-all-the-properties-of-a-target-in-cmake/56738858#56738858
## https://stackoverflow.com/a/56738858/3743145

## Get all properties that cmake supports
execute_process(COMMAND cmake --help-property-list OUTPUT_VARIABLE CMAKE_PROPERTY_LIST)
## Convert command output into a CMake list
STRING(REGEX REPLACE ";" "\\\\;" CMAKE_PROPERTY_LIST "${CMAKE_PROPERTY_LIST}")
STRING(REGEX REPLACE "\n" ";" CMAKE_PROPERTY_LIST "${CMAKE_PROPERTY_LIST}")

list(REMOVE_DUPLICATES CMAKE_PROPERTY_LIST)
# list(REMOVE_ITEM LOCATION CMAKE_PROPERTY_LIST)
# list(APPEND CMAKE_PROPERTY_LIST IMPORTED_LOCATION)

function(print_target_properties tgt)
    if(NOT TARGET ${tgt})
      message("There is no target named '${tgt}'")
      return()
    endif()

    foreach (prop ${CMAKE_PROPERTY_LIST})
        string(REPLACE "<CONFIG>" "${CMAKE_BUILD_TYPE}" prop ${prop})
        get_target_property(propval ${tgt} ${prop})
        if (propval)
            message ("${tgt} ${prop} = ${propval}")
        endif()
    endforeach(prop)
endfunction(print_target_properties)

message(WARNING "print_target_properties(libz3) begin")
# print_target_properties(fmt)
# print_target_properties(libz3)
# print_target_properties(api)
# print_target_properties(api_dll)
# print_target_properties(z3-bench)
message(WARNING "print_target_properties(libz3) end")
