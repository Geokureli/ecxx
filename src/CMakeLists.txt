cmake_minimum_required(VERSION 3.10)

add_library(ecxx
            STATIC

            ecxx/identity_generator.h
            ecxx/ecxx.h

            ecxx/v1/ecxx1.h
            ecxx/v1/bit_vector.h
            ecxx/v1/ecs_node.h
            ecxx/v1/ecs_node.cpp
            ecxx/v1/decl.h
            ecxx/v1/component_db.h
            ecxx/v1/entity.h
            ecxx/v1/worlds_map.h
            ecxx/v1/world.h
            ecxx/v1/worlds_map.cpp
            ecxx/v1/entity_impl.h
            ecxx/v1/component_managers.h
            ecxx/v1/active_entities.h
            ecxx/v1/world_impl.h
            ecxx/v1/managed_family.h
            ecxx/v1/view_filters.h
            ecxx/v1/family_aspect_data.h
            ecxx/v2/entity_pool.h
            ecxx/v2/entity_spec.h
            ecxx/v2/entity_value.h
            ecxx/v2/entity_map.h
            ecxx/v2/bit_count.h
            ecxx/v2/sparse_vector.h
            ecxx/v2/world.h
            ecxx/v2/components_db.h
            ecxx/v2/ecxx2.h
            ecxx/v2/entity_wrapper.h
            ecxx/v2/entity_wrapper_impl.h
            ecxx/v2/view.h
            ecxx/v2/runtime_view.h
            )

target_include_directories(ecxx
                           PUBLIC
                           ${CMAKE_CURRENT_SOURCE_DIR}
                           )

target_compile_definitions(ecxx
                           PUBLIC
                           ECXX_DEBUG)

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