cmake_minimum_required(VERSION 3.29)
project(ByteForge_Engine)

set(CMAKE_CXX_STANDARD 20)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(SHADER_HOT_RELOAD)
endif()

add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)

#set(CMAKE_CXX_FLAGS "-fconstexpr-depth=4096")

include_directories(
        src/
        dep/unordered_dense/include/
        dep/glfw-3.3-stable/include/
        dep/glad/include/
        dep)

add_subdirectory(dep/glfw-3.3-stable/)
add_subdirectory(dep/imgui)
add_subdirectory(dep/glm)

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
        src/core/type_def.hpp
        src/system/virtual_memory.hpp
        src/system/linux/linux_virtual_memory.cpp
        src/system/linux/linux_virtual_memory.hpp
        src/container/string_id.cpp
        src/container/string_id.hpp
        src/graphics/mesh_primitives.hpp
        src/util/random.hpp
        src/graphics/camera.cpp
        src/graphics/camera.hpp
        src/graphics/camera.cpp
        src/system/input_keys.hpp
        src/framework/input.cpp
        src/framework/input.hpp
        src/gui/imgui_subsystem.cpp
        src/gui/imgui_subsystem.hpp
        src/editor/editor_subsystem.cpp
        src/editor/editor_subsystem.hpp
        src/ecs/transform.hpp
)

target_compile_options(ByteForge_Engine PRIVATE -include ${CMAKE_SOURCE_DIR}/src/core/type_def.hpp)

target_link_libraries(ByteForge_Engine glfw ${OPENGL_LIBRARIES} IMGUI glm)
