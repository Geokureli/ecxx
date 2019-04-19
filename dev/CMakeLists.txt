cmake_minimum_required(VERSION 3.10)
project(ecxx_dev
        VERSION 0.0.1
        LANGUAGES CXX)

add_executable(ecxx_dev
               main.cpp
               )

target_include_directories(ecxx_dev
                           PUBLIC
                           ${CMAKE_CURRENT_SOURCE_DIR}
                           )

target_link_libraries(ecxx_dev ecxx)

set_target_properties(ecxx_dev PROPERTIES
                      C_STANDARD 11
                      CXX_STANDARD 17
                      CXX_STANDARD_REQUIRED YES
                      CXX_EXTENSIONS NO
                      )