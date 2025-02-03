# set(USE_CMAKE_GOOGLE_TEST_INTEGRATION OFF)
add_subdirectory(folly)
set_target_properties(folly_test_util PROPERTIES EXCLUDE_FROM_ALL ON)
