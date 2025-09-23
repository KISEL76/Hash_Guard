cmake_minimum_required(VERSION 3.24)
project(scanner_project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)

include_directories(include)

# DLL / shared library
add_library(scanner_dll SHARED
    src/scanner_dll.cpp
    src/scanner_report.cpp
    src/path_queue.cpp
    src/hash_utils.cpp
    include/hash_utils.h
    include/scanner_dll.h
    include/scanner_report.h
    include/path_queue.h
)
target_link_libraries(scanner_dll PRIVATE OpenSSL::Crypto)

# CLI
add_executable(scanner
    src/main.cpp
    src/scanner.cpp
)
target_link_libraries(scanner PRIVATE scanner_dll OpenSSL::Crypto)

enable_testing()
add_subdirectory(tests)