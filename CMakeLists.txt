cmake_minimum_required(VERSION 3.20)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

set(PROJ_NAME AlundraToolkit)

project(${PROJ_NAME})

FetchContent_Declare(
    SDL
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-2.32.2
)
set(SDL_SHARED ON)
set(SDL_STATIC OFF)
FetchContent_MakeAvailable(SDL)
message(STATUS "SDL source: ${sdl_SOURCE_DIR}")

FetchContent_Declare(
    GLM
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
)
FetchContent_MakeAvailable(GLM)
message(STATUS "GLM source: ${glm_SOURCE_DIR}")

# imgui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.9b-docking
)
FetchContent_MakeAvailable(imgui)
message(STATUS "imgui source: ${imgui_SOURCE_DIR}")

set(imgui_SRC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
)

set(imgui_BACKENDS
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

add_library(imgui STATIC
    ${imgui_SRC}
    ${imgui_BACKENDS}
)

target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${sdl_SOURCE_DIR}/include
)

#set(GLAD_PATH "extern/GLAD")
set(GLEW_ROOT "${CMAKE_SOURCE_DIR}/extern/glew-2.1.0")

find_package(OpenGL REQUIRED)

file(GLOB_RECURSE SOURCES "src/*.c" "src/*.cpp")
file(COPY ${CMAKE_SOURCE_DIR}/test DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/Shaders DESTINATION ${CMAKE_BINARY_DIR})
add_executable(${PROJ_NAME} ${SOURCES})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${PROJ_NAME} PROPERTY CXX_STANDARD 20)
endif()

#target_include_directories(${PROJ_NAME} PRIVATE ${GLAD_PATH}/include)
#target_sources(${PROJ_NAME} PRIVATE ${GLAD_PATH}/src/glad.c)
target_include_directories(${PROJ_NAME} PRIVATE ${GLEW_ROOT}/include)

target_include_directories(${PROJ_NAME} PRIVATE ${glm_SOURCE_DIR})

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    # 64-bit
    target_link_directories(${PROJ_NAME} PRIVATE ${GLEW_ROOT}/lib/Release/x64)
    message(STATUS "Using ${GLEW_ROOT}/lib/Release/x64 for glew!")
else()
    # 32-bit
    target_link_directories(${PROJ_NAME} PRIVATE ${GLEW_ROOT}/lib/Release/Win32)
    message(STATUS "Using ${GLEW_ROOT}/lib/Release/Win32 for glew!")
endif()

#target_link_libraries(${PROJ_NAME} PRIVATE SDL2::SDL2 SDL2::SDL2main SDL2_mixer::SDL2_mixer OpenGL::GL)
target_link_libraries(${PROJ_NAME} PRIVATE SDL2::SDL2 SDL2::SDL2main glew32 OpenGL::GL imgui)

# DLL export
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    # DEBUG: SDL2d.dll
    add_custom_command(TARGET ${PROJ_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${sdl_BINARY_DIR}/SDL2d.dll"
        $<TARGET_FILE_DIR:${PROJ_NAME}>
        COMMENT "Copying SDL2d.dll for Debug"
    )
endif()
if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    # RELEASE: SDL2.dll
    add_custom_command(TARGET ${PROJ_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${sdl_BINARY_DIR}/SDL2.dll"
        $<TARGET_FILE_DIR:${PROJ_NAME}>
        COMMENT "Copying SDL2.dll for Release"
    )
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    # 64-bit
    add_custom_command(TARGET ${PROJ_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    "${GLEW_ROOT}/bin/Release/x64/glew32.dll"
    $<TARGET_FILE_DIR:${PROJ_NAME}>)
else()
    # 32-bit
    add_custom_command(TARGET ${PROJ_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    "${GLEW_ROOT}/bin/Release/Win32/glew32.dll"
    $<TARGET_FILE_DIR:${PROJ_NAME}>)
endif()

if(MSVC)
  target_compile_options(${PROJ_NAME} PUBLIC "/ZI")
  target_link_options(${PROJ_NAME} PUBLIC "/INCREMENTAL")
  message(STATUS "Edit and continue for MSCV enabled!")
endif()
