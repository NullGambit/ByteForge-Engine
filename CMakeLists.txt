cmake_minimum_required(VERSION 3.30)
project(ByteForgeEngine)

set(CMAKE_CXX_STANDARD 20)

#set(CMAKE_CXX_FLAGS "-fconstexpr-depth=4096")
#set(CMAKE_CXX_FLAGS "-fsanitize=address")

find_package(OpenGL REQUIRED)

add_subdirectory(dep/glfw-3.3-stable/)
add_subdirectory(dep/imgui)
add_subdirectory(dep/glm)
add_subdirectory(dep/rpmalloc-1.4.5)
add_subdirectory(dep/magic_enum)

add_library(ByteForgeEngine STATIC
dep/glad/include/glad/glad.h
        dep/glad/include/KHR/khrplatform.h
        dep/glad/src/glad.c
        forge/ecs/ecs.hpp
        forge/fmt/fmt.hpp
        forge/util/types.hpp
        dep/unordered_dense/include/ankerl/unordered_dense.h
        forge/container/map.hpp
        forge/memory/defs.hpp
        forge/ecs/ecs.cpp
        forge/memory/mem_pool.cpp
        forge/memory/mem_pool.hpp
        forge/ecs/macro_warcrimes.hpp
        forge/events/signal.hpp
        forge/system/window.cpp
        forge/system/window.hpp
        forge/core/isub_system.hpp
        forge/system/window_sub_system.cpp
        forge/system/window_sub_system.hpp
        forge/graphics/ogl_renderer/ogl_shader.cpp
        forge/graphics/ogl_renderer/ogl_shader.hpp
        forge/graphics/ogl_renderer/ogl_renderer.cpp
        forge/graphics/ogl_renderer/ogl_renderer.hpp
        forge/core/engine.hpp
        forge/core/engine.cpp
        forge/events/type_box.hpp
        forge/system/io.cpp
        forge/system/io.hpp
        forge/util/macros.hpp
        forge/system/fs_monitor.hpp
        forge/system/linux/linux_fs_monitor.cpp
        forge/system/linux/linux_fs_monitor.hpp
        forge/core/logging.hpp
        forge/concurrency/command_buffer.hpp
        forge/graphics/image/image.hpp
        dep/stb/stb_image.h
        forge/graphics/ogl_renderer/ogl_texture.cpp
        forge/graphics/ogl_renderer/ogl_texture.hpp
        forge/resources/resource.hpp
        forge/graphics/image/stb_image.hpp
        forge/graphics/image/stb_image.cpp
        forge/graphics/image/image.cpp
        forge/core/forge_intrinsic.hpp
        forge/system/virtual_memory.hpp
        forge/system/linux/linux_virtual_memory.cpp
        forge/system/linux/linux_virtual_memory.hpp
        forge/container/string_id.cpp
        forge/container/string_id.hpp
        forge/graphics/mesh_primitives.hpp
        forge/util/random.hpp
        forge/graphics/camera.cpp
        forge/graphics/camera.hpp
        forge/graphics/camera.cpp
        forge/system/input_keys.hpp
        forge/framework/input.cpp
        forge/framework/input.hpp
        forge/gui/imgui_subsystem.cpp
        forge/gui/imgui_subsystem.hpp
        forge/editor/editor_subsystem.cpp
        forge/editor/editor_subsystem.hpp
        forge/ecs/transform.hpp
        forge/memory/mem_utils.hpp
        forge/system/system_info.hpp
        forge/system/linux/linux_system_info.cpp
        forge/system/linux/linux_system_info.hpp
        forge/ecs/component_field.hpp
        forge/ecs/ecs_views.cpp
        forge/ecs/ecs_views.hpp
        forge/graphics/material.hpp
        forge/graphics/ogl_renderer/render_resource.hpp
        forge/system/native_dialog.hpp
        forge/system/linux/linux_native_dialog.cpp
        forge/system/linux/linux_native_dialog.hpp
        forge/util/defer.hpp
        forge/fmt/fmt.cpp
        forge/graphics/ogl_renderer/ogl_buffers.cpp
        forge/graphics/ogl_renderer/ogl_buffers.hpp
        forge/config/arg_parser.cpp
        forge/config/arg_parser.hpp
        forge/framework/components/camera_component.cpp
        forge/framework/components/camera_component.hpp
        forge/container/virtual_array.hpp
        forge/framework/components/register_engine_components.cpp
        forge/framework/components/register_engine_components.hpp
        forge/core/engine_init_options.hpp
        forge/ecs/defs.hpp
        forge/container/set.hpp
        forge/container/hash.hpp
        forge/framework/components/mesh_primitive_component.cpp
        forge/framework/components/mesh_primitive_component.hpp
        forge/graphics/mesh.hpp
        forge/graphics/mesh_generator.cpp
        forge/graphics/mesh_generator.hpp
        forge/events/timer.cpp
        forge/events/timer.hpp
        forge/container/array.hpp
        forge/container/string.hpp
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(ByteForgeEngine PRIVATE FORGE_SHADER_HOT_RELOAD)
endif()

target_include_directories(ByteForgeEngine PUBLIC
        dep/glfw-3.3-stable/include/
        dep/glad/include/
        dep/rpmalloc-1.4.5
        dep/unordered_dense/include
        dep
        dep/magic_enum/include
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_precompile_headers(ByteForgeEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/forge/core/forge_intrinsic.hpp)
target_compile_definitions(ByteForgeEngine PUBLIC
        GLM_ENABLE_EXPERIMENTAL
        FORGE_USE_UNIFORM_CACHE
        FORGE_ENGINE_ASSET_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets/"
        FORGE_ENGINE_CONFIG_DIR="${CMAKE_CURRENT_SOURCE_DIR}/config/"
)

target_link_libraries(ByteForgeEngine PUBLIC glfw ${OPENGL_LIBRARIES} IMGUI glm rpmalloc)
