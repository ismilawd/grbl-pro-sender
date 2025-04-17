cmake_minimum_required(VERSION 3.28)
project(grbl_pro_sender)

set(CMAKE_CXX_STANDARD 26)

include(FetchContent)

# Fetch GLFW
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        latest
)
FetchContent_MakeAvailable(glfw)

# Fetch Dear ImGui docking branch
FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG        docking
)
FetchContent_MakeAvailable(imgui)

# Set paths
set(IMGUI_DIR ${imgui_SOURCE_DIR})
set(IMGUI_BACKENDS_DIR ${imgui_SOURCE_DIR}/backends)

# Collect source files
set(IMGUI_SRC
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_BACKENDS_DIR}/imgui_impl_glfw.cpp
        ${IMGUI_BACKENDS_DIR}/imgui_impl_opengl3.cpp
)

include_directories(./include)

# Main app
add_executable(grbl_pro_sender
        src/main.cpp
        ${IMGUI_SRC})

# Includes
target_include_directories(grbl_pro_sender PRIVATE
        ${IMGUI_DIR}
        ${IMGUI_BACKENDS_DIR}
        ${glfw_SOURCE_DIR}/include
)

# Link (GLFW and system OpenGL)
find_package(OpenGL REQUIRED)
target_link_libraries(grbl_pro_sender PRIVATE glfw OpenGL::GL)

# Link necessary macOS frameworks
if(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(OpenGL_LIBRARY OpenGL)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    target_link_libraries(grbl_pro_sender PRIVATE
            ${COCOA_LIBRARY}
            ${OpenGL_LIBRARY}
            ${IOKIT_LIBRARY}
            ${COREVIDEO_LIBRARY}
    )
    target_compile_definitions(grbl_pro_sender PUBLIC GL_SILENCE_DEPRECATION)
endif()

