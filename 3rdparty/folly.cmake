# set(USE_CMAKE_GOOGLE_TEST_INTEGRATION OFF)
add_subdirectory(folly)
set_target_properties(folly_test_util follybenchmark logging_example logging_example_lib PROPERTIES EXCLUDE_FROM_ALL ON)
# add_library(folly INTERFACE)
# target_include_directories(folly INTERFACE folly-scoped-include)
get_target_property(tmp_folly_deps_intf_inc_dirs folly_deps INTERFACE_INCLUDE_DIRECTORIES)
list(FILTER tmp_folly_deps_intf_inc_dirs EXCLUDE REGEX "GLOG_INCLUDE_DIR-NOTFOUND")
set_target_properties(folly_deps PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${tmp_folly_deps_intf_inc_dirs}")


get_target_property(tmp_folly_deps_intf_libs folly_deps INTERFACE_LINK_LIBRARIES)
list(TRANSFORM tmp_folly_deps_intf_libs REPLACE "\.dylib" ".a")
list(FILTER tmp_folly_deps_intf_libs EXCLUDE REGEX "GLOG_LIBRARY-NOTFOUND")
set_target_properties(folly_deps PROPERTIES INTERFACE_LINK_LIBRARIES "${tmp_folly_deps_intf_libs}")
