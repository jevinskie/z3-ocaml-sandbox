#file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/gmp-build/tmp")

ExternalProject_Add(gmp_ext
    SOURCE_DIR          ${CMAKE_CURRENT_SOURCE_DIR}/gmp
    CONFIGURE_COMMAND   cd <SOURCE_DIR> &&
                        ./.bootstrap
    BUILD_COMMAND       <SOURCE_DIR>/configure
                        CC=${CMAKE_C_COMPILER}
                        CFLAGS=${CMAKE_C_FLAGS}
                        CXX=${CMAKE_CXX_COMPILER}
                        CXXFLAGS=${CMAKE_CXX_FLAGS}
                        LDFLAGS=${CMAKE_MODULE_LINKER_FLAGS}
                        AR=${CMAKE_AR}
                        RANLIB=${CMAKE_RANLIB}
                        --prefix=<INSTALL_DIR>
                        ${CONFIGURE_SHARED}
                        ${CONFIGURE_STATIC}
                        --disable-cxx &&
                        make -j ${NCPUS} MAKEINFO=true
    INSTALL_COMMAND     make -j ${NCPUS} install MAKEINFO=true
    PREFIX              gmp-build
    INSTALL_DIR         gmp-prefix
    BINARY_DIR          gmp-build/build
    STAMP_DIR           gmp-build/stamp
    BUILD_JOB_SERVER_AWARE TRUE
    # BUILD_IN_SOURCE     OFF
    INSTALL_BYPRODUCTS    ${CMAKE_CURRENT_BINARY_DIR}/gmp-prefix/include/gmp.h ${CMAKE_CURRENT_BINARY_DIR}/gmp-prefix/lib/libgmp${LIB_EXT}
    # STEP_TARGETS build
    STEP_TARGETS install
    EXCLUDE_FROM_ALL ON
)

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/gmp-prefix/include")

# https://discourse.cmake.org/t/add-subdirectory-vs-externalproject-add-vs-include-path-to-projectconfig-cmake/2959/2
# https://github.com/geospace-code/h5fortran/blob/main/CMakeLists.txt
add_library(GMP::GMP INTERFACE IMPORTED GLOBAL)
add_dependencies(GMP::GMP gmp_ext gmp_ext-install)

set_target_properties(GMP::GMP PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_BINARY_DIR}/gmp-prefix/include"
    IMPORTED_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/gmp-prefix/lib/libgmp${LIB_EXT}"
)

# add_library(GMP ALIAS GMP::GMP)

# get_target_property(X GMP INTERFACE_INCLUDE_DIRECTORIES)
# message(WARNING "gmp include: ${X}")
# get_target_property(X GMP IMPORTED_LOCATION)
# message(WARNING "gmp lib: ${X}")
get_target_property(X GMP::GMP INTERFACE_INCLUDE_DIRECTORIES)
message(WARNING "gmp::gmp include: ${X}")
get_target_property(X GMP::GMP IMPORTED_LOCATION)
message(WARNING "gmp::gmp lib: ${X}")
