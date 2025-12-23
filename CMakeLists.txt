cmake_minimum_required(VERSION 3.20)

project(AlundraToolkit LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ----------------------------
# Dependencies (vcpkg)
# ----------------------------
find_package(SDL2 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(FluidSynth CONFIG REQUIRED)

# ----------------------------
# Executable
# ----------------------------
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")
add_executable(${PROJECT_NAME} ${SOURCES})

# ----------------------------
# Link
# ----------------------------
target_sources(${PROJECT_NAME} PRIVATE
    extern/imgui/backends/imgui_impl_sdl2.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    extern/imgui/backends
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL2::SDL2
    SDL2::SDL2main
    OpenGL::GL
    GLEW::GLEW
    glm::glm
    imgui::imgui
    FluidSynth::libfluidsynth
)

# ----------------------------
# Copy resources
# ----------------------------
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/Shaders"
    "${CMAKE_CURRENT_BINARY_DIR}/Shaders"
)

# Temporary location of DATAS.BIN
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/test"
    "${CMAKE_CURRENT_BINARY_DIR}/test"
)

# ----------------------------
# MSVC hot reload
# ----------------------------
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /ZI)
    target_link_options(${PROJECT_NAME} PRIVATE /INCREMENTAL)
endif()
