cmake_minimum_required(VERSION 3.11)
project(ecxx_tests
        VERSION 0.0.1
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(BUILD_GMOCK OFF)
set(INSTALL_GTEST OFF)
add_subdirectory(googletest)

include(GoogleTest)
enable_testing()

add_executable(ecxx_tests
               bit_vector_test.cpp
               identity_generator_test_1.cpp
               identity_generator_test_2.cpp
               identity_generator_shared.h components.h component_test.cpp)

set_target_properties(ecxx_tests PROPERTIES
                      C_STANDARD 11
                      CXX_STANDARD 17
                      CXX_STANDARD_REQUIRED YES
                      CXX_EXTENSIONS NO
                      )

set(ecxx_tests_args)

gtest_add_tests(ecxx_tests "${ecxx_tests_args}" AUTO)
target_link_libraries(ecxx_tests ecxx gtest gtest_main pthread)
target_compile_options(ecxx_tests
                       PRIVATE
                       -fno-exceptions
                       )

#target_include_directories(ek_unit_tests
#                           PRIVATE
#                           ../src
#                           )
