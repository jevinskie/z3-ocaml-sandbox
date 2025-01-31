# add_custom_command()

ExternalProject_Add(gmp_ext
    SOURCE_DIR          ${CMAKE_CURRENT_SOURCE_DIR}/gmp
    CONFIGURE_COMMAND   rsync -a ${CMAKE_CURRENT_SOURCE_DIR}/gmp/ . &&
                        autoreconf -fiv &&
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
    # BUILD_COMMAND       make -j ${NCPUS} MAKEINFO=true
    INSTALL_COMMAND     make -j ${NCPUS} install MAKEINFO=true
    BUILD_COMMAND       make -j ${NCPUS} install MAKEINFO=true
    # INSTALL_COMMAND     true
    PREFIX              gmp-build
    INSTALL_DIR         gmp-prefix
    # BUILD_JOB_SERVER_AWARE TRUE
    # BUILD_IN_SOURCE     OFF
    BUILD_BYPRODUCTS    gmp-prefix/lib/libgmp.a gmp-prefix/lib/libgmpxx.a gmp-prefix/include/gmp.h gmp-prefix/include/gmpxx.h
    # (Adjust for parallel builds, host/target options, etc. as needed)
    # EXCLUDE_FROM_ALL
)

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


add_library(GMP ${LIB_TYPE_CMAKE} IMPORTED)
target_include_directories(GMP INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/gmp-prefix/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(GMP INTERFACE
    $<BUILD_INTERFACE:"${CMAKE_CURRENT_BINARY_DIR}/gmp-prefix/lib/libgmp${LIB_EXT}">
    $<INSTALL_INTERFACE:"lib/libgmp${LIB_EXT}">
)
set_target_properties(GMP PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/gmp-prefix/lib/libgmp${LIB_EXT}"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_BINARY_DIR}/gmp-prefix/include"
#    IMPORTED_LINK_INTERFACE_LANGUAGES "C"  # Or "CXX" for C++
)

add_dependencies(GMP gmp_ext)
add_library(GMP::GMP ALIAS GMP)

set(GMP_FOUND ON)
set(GMP_INCLUDE_DIRS "${CMAKE_CURRENT_BINARY_DIR}/gmp-prefix/include")

get_target_property(X GMP INTERFACE_INCLUDE_DIRECTORIES)
message(STATUS "gmp: ${X}")
