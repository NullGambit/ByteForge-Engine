cmake_minimum_required(VERSION 3.29)
project(ByteForge_Engine)

set(CMAKE_CXX_STANDARD 20)

include_directories(dep/unordered_dense/include/)

add_executable(ByteForge_Engine main.cpp
        src/ecs/ecs.hpp
        src/fmt/fmt.hpp
        src/util/types.hpp
        dep/unordered_dense/include/ankerl/unordered_dense.h
        src/container/map.hpp
        src/memory/defs.hpp
        src/ecs/ecs.cpp
        src/core/result.hpp
        src/memory/mem_pool.cpp
        src/memory/mem_pool.hpp
)
