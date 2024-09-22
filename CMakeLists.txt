cmake_minimum_required(VERSION 3.29)
project(ByteForge_Engine)

set(CMAKE_CXX_STANDARD 20)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(SHADER_HOT_RELOAD)
endif()

include_directories(
        src/
        dep/unordered_dense/include/
        dep/glfw-3.3-stable/include/
        dep/glad/include/
        dep)

add_subdirectory(dep/glfw-3.3-stable/ dep/glm)

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
        src/graphics/ogl_renderer/ogl_renderer.cpp
        src/graphics/ogl_renderer/ogl_renderer.hpp
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
        src/core/isub_system.cpp
        src/core/logging.hpp
        src/concurrency/command_buffer.hpp
        src/graphics/image/image.hpp
        dep/stb/stb_image.h
        src/graphics/ogl_renderer/ogl_texture.cpp
        src/graphics/ogl_renderer/ogl_texture.hpp
        src/resources/resource.hpp
        src/graphics/image/stb_image.hpp
        src/graphics/image/stb_image.cpp
        src/graphics/image/image.cpp
)

target_link_libraries(ByteForge_Engine glfw ${OPENGL_LIBRARIES})
