cmake_minimum_required(VERSION 3.10)
project(ecxx
        VERSION 0.0.1
        LANGUAGES CXX)

add_subdirectory(src)

if (NOT DEFINED ECXX_MAKE_TESTS)
    set(ECXX_MAKE_TESTS 1)
endif ()

if (NOT DEFINED ECXX_MAKE_DEVELOP)
    set(ECXX_MAKE_DEVELOP 1)
endif ()

if (ECXX_MAKE_TESTS)
    add_subdirectory(test)
endif ()

if (ECXX_MAKE_DEVELOP)
    add_subdirectory(dev)
endif ()


if (NOT DEFINED ECXX_MAKE_BENCHMARKS)
    set(ECXX_MAKE_BENCHMARKS 1)
endif ()

if (ECXX_MAKE_BENCHMARKS)
    add_subdirectory(benchmarks)
endif ()
