cmake_minimum_required(VERSION 3.10)
project(ecxx
        VERSION 0.0.1
        LANGUAGES CXX)

add_library(ecxx
            STATIC

            src/ecxx/ecxx.cpp
            src/ecxx/ecxx.h
            src/ecxx/bit_vector.h
            src/ecxx/identity_generator.h
            src/ecxx/ecs_node.h
            src/ecxx/ecs_node.cpp
            dev/main.cpp)

target_include_directories(ecxx
                           PUBLIC
                           ${CMAKE_CURRENT_SOURCE_DIR}/src
                           )

set(ECXX_COMPILE_WARNINGS
    -Wall -Wextra -Wshadow -Wnon-virtual-dtor
    -Wnull-dereference -Wpedantic -Wreturn-type
    -Woverloaded-virtual -Wcast-align
    -Wno-deprecated-declarations
    -Wstrict-aliasing)

set(ECXX_COMPILER_LTO "-flto")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(ECXX_COMPILE_WARNINGS ${ECXX_COMPILE_WARNINGS}
        -Wno-dollar-in-identifier-extension
        -Wno-gnu-anonymous-struct
        -Wno-nested-anon-types
        )
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(ECXX_COMPILER_LTO "")
endif ()

if (CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_options(ecxx
                           PRIVATE
                           -fno-exceptions
                           -fno-rtti
                           -fstrict-aliasing
                           -g
                           ${ECXX_COMPILE_WARNINGS}
                           )
else ()
    target_compile_options(ecxx
                           PRIVATE
                           -fno-exceptions
                           -fno-rtti
                           -fstrict-aliasing
                           -Os
                           ${ECXX_COMPILER_LTO}
                           ${ECXX_COMPILE_WARNINGS}
                           )
endif ()

target_compile_options(ecxx
                       PRIVATE
                       -std=c++17
                       )

set_target_properties(ecxx PROPERTIES
                      C_STANDARD 11
                      CXX_STANDARD 17
                      CXX_STANDARD_REQUIRED YES
                      CXX_EXTENSIONS NO
                      )

if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    target_compile_options(ecxx
                           PUBLIC
                           -Wno-dollar-in-identifier-extension)

elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Android")
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
elseif (IOS)
else ()
    message("Platform: UNKNOWN")
endif ()

# todo: tests option
if(1)
add_subdirectory(test)
add_subdirectory(dev)
endif()