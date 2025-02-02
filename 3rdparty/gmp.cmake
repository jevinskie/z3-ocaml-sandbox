# add_custom_command()

ExternalProject_Add(gmp_ext
    SOURCE_DIR          ${CMAKE_CURRENT_SOURCE_DIR}/gmp
    CONFIGURE_COMMAND   rsync -a ${CMAKE_CURRENT_SOURCE_DIR}/gmp/ . &&
                        ./.bootstrap &&
                        ./configure
                        CC=${CMAKE_C_COMPILER}
                        CFLAGS=${CMAKE_C_FLAGS}
                        CXX=${CMAKE_CXX_COMPILER}
                        CXXFLAGS=${CMAKE_CXX_FLAGS}
                        LDFLAGS=${CMAKE_MODULE_LINKER_FLAGS}
                        AR=${CMAKE_AR}
                        RANLIB=${CMAKE_RANLIB}
                        --prefix=${CMAKE_CURRENT_BINARY_DIR}/gmp-prefix
                        ${CONFIGURE_SHARED}
                        ${CONFIGURE_STATIC}
                        --disable-cxx
    BUILD_COMMAND       make -j ${NCPUS} install MAKEINFO=true
    INSTALL_COMMAND     ""
    PREFIX              gmp-build
    INSTALL_DIR         gmp-prefix
    # BUILD_JOB_SERVER_AWARE TRUE
    # BUILD_IN_SOURCE     OFF
    BUILD_BYPRODUCTS    gmp-prefix/include/gmp.h gmp-prefix/lib/libgmp.a
    # EXCLUDE_FROM_ALL
)

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/gmp-prefix/include")


# if (BUILD_SHARED_LIBS)
#     add_library(GMP INTERFACE EXCLUDE_FROM_ALL)
# else()
#     add_library(GMP INTERFACE EXCLUDE_FROM_ALL)
# endif()

# set_target_properties(GMP PROPERTIES EXCLUDE_FROM_ALL OFF)
# target_link_libraries(GMP INTERFACE gmp-prefix/lib/libgmp${LIB_EXT})
# target_include_directories(GMP INTERFACE gmp-prefix/include)
# target_include_directories(GMP INTERFACE
# $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/gmp-prefix/include/gmp>
# $<INSTALL_INTERFACE:include/gmp>
# )

# install(TARGETS GMP
    # EXPORT libz3
#     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
#     INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
# )

# install(IMPORTED_RUNTIME_ARTIFACTS GMP
    # EXPORT GMPExport
    # LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    # ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    # RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    # INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
# )


# add_library(GMP::GMP UNKNOWN IMPORTED)
# target_include_directories(GMP INTERFACE
#     $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/gmp-prefix/include>
#     $<INSTALL_INTERFACE:include>
# )
# target_link_libraries(GMP::GMP INTERFACE
#     $<BUILD_INTERFACE:"${CMAKE_CURRENT_BINARY_DIR}/gmp-prefix/lib/libgmp${LIB_EXT}">
#     $<INSTALL_INTERFACE:"lib/libgmp${LIB_EXT}">
# )
# set_target_properties(GMP::GMP PROPERTIES
#     IMPORTED_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/gmp-prefix/lib/libgmp${LIB_EXT}"
#     INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_BINARY_DIR}/gmp-prefix/include"
# #    IMPORTED_LINK_INTERFACE_LANGUAGES "C"  # Or "CXX" for C++
# )

# add_dependencies(GMP::GMP gmp_ext-complete gmp_ext gmp_ext-build ${CMAKE_CURRENT_BINARY_DIR}/gmp-prefix/lib/libgmp.a ${CMAKE_CURRENT_BINARY_DIR}/gmp-prefix/include/gmp.h)
# add_library(GMP ALIAS GMP::GMP)

# set(GMP_FOUND ON)
# set(GMP_C_INCLUDES "${CMAKE_CURRENT_BINARY_DIR}/gmp-prefix/include")
# set(GMP_CXX_INCLUDES "${CMAKE_CURRENT_BINARY_DIR}/gmp-prefix/include")
# set(GMP_INCLUDE_DIRS "${GMP_C_INCLUDES}" "${GMP_CXX_INCLUDES}")
# set(GMP_C_LIBRARIES "${CMAKE_CURRENT_BINARY_DIR}/gmp-prefix/lib/libgmp.a")
# set(GMP_CXX_LIBRARIES "${CMAKE_CURRENT_BINARY_DIR}/gmp-prefix/lib/libgmpxx.a")
# list(REMOVE_DUPLICATES GMP_INCLUDE_DIRS)
# uset_target_properties(GMP::GMP PROPERTIES
#     INTERFACE_INCLUDE_DIRECTORIES "${GMP_C_INCLUDES}"
#         IMPORTED_LOCATION "${GMP_C_LIBRARIES}")

add_library(GMP::GMP UNKNOWN IMPORTED)
add_dependencies(GMP::GMP gmp_ext)

set_target_properties(GMP::GMP PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_BINARY_DIR}/gmp-prefix/include"
    IMPORTED_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/gmp-prefix/lib/libgmp.a"
)

add_library(GMP ALIAS GMP::GMP)

get_target_property(X GMP INTERFACE_INCLUDE_DIRECTORIES)
message(WARNING "gmp include: ${X}")
get_target_property(X GMP IMPORTED_LOCATION)
message(WARNING "gmp lib: ${X}")
get_target_property(X GMP::GMP INTERFACE_INCLUDE_DIRECTORIES)
message(WARNING "gmp::gmp include: ${X}")
get_target_property(X GMP::GMP IMPORTED_LOCATION)
message(WARNING "gmp::gmp lib: ${X}")
