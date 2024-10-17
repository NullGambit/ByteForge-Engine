cmake_minimum_required(VERSION 3.29)

add_library(IMGUI STATIC)

set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(IMGUI
                PRIVATE
                    imgui_demo.cpp
                    imgui_draw.cpp
                    imgui_tables.cpp
                    imgui_widgets.cpp
                    misc/cpp/imgui_stdlib.h
                    misc/cpp/imgui_stdlib.cpp
                    imgui.cpp
                    misc/cpp/imgui_stdlib.cpp

                PRIVATE
                    backends/imgui_impl_glfw.cpp
                    backends/imgui_impl_opengl3.cpp
                )

target_include_directories(IMGUI
                            PUBLIC ${IMGUI_DIR}
                            PUBLIC ${IMGUI_DIR}/backends
                            PUBLIC ${SDL3_DIR}/include
                            )
#target_link_libraries(IMGUI glfw vulkan ${OPENGL_LIBRARIES})