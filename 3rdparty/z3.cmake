set(Z3_USE_LIB_GMP ON)
set(Z3_SINGLE_THREADED ON)
set(Z3_POLLING_TIMER OFF) # TODO: test
set(Z3_BUILD_LIBZ3_SHARED ${BUILD_SHARED_LIBS})
set(Z3_BUILD_EXECUTABLE OFF)
# set(Z3_COMPONENT_EXTRA_INCLUDE_DIRS "${Z3_COMPONENT_EXTRA_INCLUDE_DIRS};${GMP_INC_DIRS}")
add_subdirectory(z3)
# target_include_directories(libz3 PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/z3/src/api")
# set_target_properties(libz3 PROPERTIES EXCLUDE_FROM_ALL ON)
target_link_libraries(libz3 PUBLIC GMP)
get_target_property(libz3_sources libz3 SOURCES)

foreach(obj_target IN LISTS libz3_sources)
    if("${obj_target}" MATCHES "^\\$<TARGET_OBJECTS:(.*)>$")
        set(target_name "${CMAKE_MATCH_1}")
        target_link_libraries(${target_name} PUBLIC GMP)
    endif()
endforeach()
# add_dependencies(libz3 gmp-prefix/include/gmp.h)
# add_dependencies(libz3 GMP::GMP)
# target_include_directories(libz3 PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/z3/src/api")
# target_include_directories(libz3 INTERFACE
#     $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/gmp-prefix/include>
#     $<INSTALL_INTERFACE:include>
# )
# get_target_property(GMP_INC_DIRS GMP INTERFACE_INCLUDE_DIRECTORIES)
# add_custom_target(genexdebug COMMAND ${CMAKE_COMMAND} -E echo "$<TARGET_PROPERTY:GMP,INTERFACE_INCLUDE_DIRECTORIES>")
# add_custom_target(genexdebug2 COMMAND ${CMAKE_COMMAND} -E echo "$<TARGET_PROPERTY:GMP,INCLUDE_DIRECTORIES>")

# message(WARNING "HELLO: ${fooo}")
# target_include_directories(util PUBLIC $<TARGET_PROPERTY:GMP,INTERFACE_INCLUDE_DIRECTORIES>)
# target_include_directories(libz3 PUBLIC $<TARGET_PROPERTY:GMP,INTERFACE_INCLUDE_DIRECTORIES>)
# target_include_directories(util INTERFACE $<TARGET_PROPERTY:GMP,INTERFACE_INCLUDE_DIRECTORIES>)
# target_include_directories(libz3 INTERFACE $<TARGET_PROPERTY:GMP,INTERFACE_INCLUDE_DIRECTORIES>)
# target_include_directories(util PRIVATE /tmp/)
# install(TARGETS GMP
#     EXPORT Z3_EXPORTED_TARGETS
#     ARCHIVE
#     LIBRARY
#     PUBLIC_HEADER
#     PRIVATE_HEADER
# )
