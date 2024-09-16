cmake_minimum_required(VERSION 3.29)
project(ByteForge_Engine)

set(CMAKE_CXX_STANDARD 20)

include_directories(src/ dep/unordered_dense/include/ dep/glfw-3.3-stable/include/ dep/glad/include/)

add_subdirectory(dep/glfw-3.3-stable/)

add_executable(ByteForge_Engine main.cpp
        dep/glad/include/glad/glad.h
        dep/glad/include/KHR/khrplatform.h
        dep/glad/src/glad.c
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
        src/ecs/macro_warcrimes.hpp
        src/events/signal.hpp
        src/system/window.cpp
        src/system/window.hpp
        src/core/isub_system.hpp
        src/system/window_sub_system.cpp
        src/system/window_sub_system.hpp
        src/graphics/ogl_renderer/ogl_shader.cpp
        src/graphics/ogl_renderer/ogl_shader.hpp
        src/graphics/ogl_renderer/ogl_render_sub_system.cpp
        src/graphics/ogl_renderer/ogl_render_sub_system.hpp
        src/core/engine.hpp
        src/core/engine.cpp
        src/events/type_box.hpp
        src/sync/atomic_scope_lock.hpp
        src/system/io.cpp
        src/system/io.hpp
        src/util/macros.hpp
        src/system/fs_monitor.hpp
        src/system/linux/linux_fs_monitor.cpp
        src/system/linux/linux_fs_monitor.hpp
)

target_link_libraries(ByteForge_Engine glfw)
