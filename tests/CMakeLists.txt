include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(scanner_tests
  hash_utils_test.cpp
  path_queue_test.cpp
)

target_include_directories(scanner_tests PRIVATE ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(scanner_tests
  PRIVATE
    scanner_dll
    GTest::gtest
    GTest::gtest_main
)

if(APPLE)
  set_target_properties(scanner_tests PROPERTIES
    MACOSX_RPATH ON
    BUILD_RPATH "$ORIGIN"
  )
endif()

include(GoogleTest)
gtest_discover_tests(scanner_tests)