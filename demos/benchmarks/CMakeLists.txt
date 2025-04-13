cmake_minimum_required(VERSION 3.30)
project(ForgeBenchmarks)

set(CMAKE_CXX_STANDARD 20)

set(FORGE_DIR ${CMAKE_SOURCE_DIR}/../../)

add_subdirectory(${FORGE_DIR} ${CMAKE_BINARY_DIR}/ByteForgeEngine)

add_executable(ForgeBenchmarks
        src/main.cpp
        src/cpu_benchmark.cpp
        src/cpu_benchmark.hpp
)

target_link_libraries(ForgeBenchmarks PUBLIC ByteForgeEngine)

target_compile_definitions(ForgeBenchmarks PUBLIC
        GLM_ENABLE_EXPERIMENTAL
        FORGE_USE_UNIFORM_CACHE
)
target_include_directories(ForgeBenchmarks PUBLIC ${FORGE_DIR})

