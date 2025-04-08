cmake_minimum_required(VERSION 3.30)
project(DevDemo)

set(CMAKE_CXX_STANDARD 20)

set(FORGE_DIR ${CMAKE_SOURCE_DIR}/../..)

add_subdirectory(${FORGE_DIR} ${CMAKE_BINARY_DIR}/ByteForgeEngine)

add_executable(DevDemo
        src/main.cpp
        src/components/bob_component.cpp
        src/components/bob_component.hpp
        src/components/export_test_component.hpp
        src/components/player_controller.hpp
        src/components/player_controller.cpp
        src/components/lifetime_component.cpp
        src/components/lifetime_component.hpp
        src/components/light_component.cpp
        src/components/light_component.hpp)

target_link_libraries(DevDemo PUBLIC ByteForgeEngine)

target_include_directories(DevDemo PUBLIC ${FORGE_DIR})
target_compile_definitions(DevDemo PUBLIC
        GLM_ENABLE_EXPERIMENTAL
        FORGE_USE_UNIFORM_CACHE
        DEMO_ASSET_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets/"
        DEMO_CONFIG_DIR="${CMAKE_CURRENT_SOURCE_DIR}/config/"
)

#target_precompile_headers(byte_forge_demo PUBLIC ${FORGE_DIR}/forge/core/forge_intrinsic.hpp)