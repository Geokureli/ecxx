cmake_minimum_required(VERSION 3.11)

include(GoogleTest)
enable_testing()

add_executable(benchmarks
               ecxx_benchmark.cpp
               entt_benchmark.cpp
               )

set_target_properties(benchmarks PROPERTIES
                      C_STANDARD 11
                      CXX_STANDARD 17
                      CXX_STANDARD_REQUIRED YES
                      CXX_EXTENSIONS NO
                      )

set(benchmarks_args)

gtest_add_tests(benchmarks "${benchmarks_args}" AUTO)
target_link_libraries(benchmarks ecxx gtest gtest_main pthread)
target_compile_options(benchmarks
                       PRIVATE
                       -fno-exceptions
                       -fno-rtti
                       -Os
                       )

target_compile_definitions(benchmarks
                           PUBLIC
                           NDEBUG)

target_include_directories(benchmarks
                           PRIVATE
                           .)
