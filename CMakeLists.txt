cmake_minimum_required(VERSION 3.10)
project(OpenGLProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(main
    src/main.cpp
    src/glad.c
)

# Include directories for GLAD and GLFW
target_include_directories(main PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# Link GLFW (static) and required Windows system libs
target_link_libraries(main PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/libglfw3.a"
    opengl32
    gdi32
    user32
    shell32
)

# Make sure the 'include' folder contains 'GLFW' and 'glad' subfolders.
# Place 'libglfw3.a' in your 'lib' folder.