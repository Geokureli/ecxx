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
               common/components.h

               v1/bit_vector_test.cpp
               common/identity_generator_test_1.cpp
               common/identity_generator_test_2.cpp
               common/identity_generator_shared.h
               v1/component_test.cpp
               v1/query_test.cpp
               v1/family_aspect_test.cpp
               v1/view_test.cpp

               v2/entity_pool_test.cpp
               v2/entity_map_test.cpp
               v2/bit_count_test.cpp
               v2/sparse_vector_test.cpp
               v2/entity_value_test.cpp
               v2/world_test.cpp
               v2/components_test.cpp
               v2/view_test.cpp)

set_target_properties(ecxx_tests PROPERTIES
                      C_STANDARD 11
                      CXX_STANDARD 17
                      CXX_STANDARD_REQUIRED YES
                      CXX_EXTENSIONS NO
                      )

set(ecxx_tests_args)

gtest_add_tests(ecxx_tests "${ecxx_tests_args}" AUTO)
target_link_libraries(ecxx_tests ecxx gtest gtest_main pthread)
#target_compile_options(ecxx_tests
 #                      PRIVATE
  #                     -fno-exceptions
   #                    )

#target_include_directories(ek_unit_tests
#                           PRIVATE
#                           ../src
#                           )
