# Simple 2D Snake Game (OpenGL)

## Overview
Classic Snake game on a 20x20 grid using OpenGL and GLFW. Control the snake to eat food, grow, and avoid collisions. Levels increase game speed every 50 points.

---

## Features
- Grid-based snake movement with arrow keys  
- Random food spawning avoiding snake’s body  
- Snake grows on eating food; game resets on collision  
- Level system speeds up snake every 50 points  
- Simple 2D OpenGL rendering  
- Press `R` to reset anytime

---

## Controls
- **Arrow keys**: Change direction  
- **R**: Reset game

---

## Requirements
- C++11 or newer  
- GLFW and GLAD libraries  
- OpenGL 3.3 Core Profile or newer  
- CMake (for building)

---

## Build & Run

### Using CMake and Batch Script (Windows)

1. Make sure CMake is installed and added to your system PATH.  
2. Run `build_cmake.bat` in the project directory. This script configures and builds the project.  
3. Run the generated executable (usually found in the `build` folder).

### Example content for `build_cmake.bat`:
```bat
@echo off
mkdir build
cd build
cmake ..
cmake --build . --config Release
pause


## Explanation of What We Did and How We Did It (The Process)

In this project, we developed a classic Snake game using OpenGL and GLFW to create a smooth 2D graphical experience on a fixed 20x20 grid.

### What we did:
- Set up the OpenGL rendering pipeline with simple vertex and fragment shaders to draw colored rectangles representing the grid, snake, and food.
- Implemented the snake as a list of points, updating its position based on user input and timed intervals.
- Created random food placement logic that ensures food never appears on the snake.
- Added game logic to handle snake growth, collision detection, and resetting the game on death.
- Introduced a level system that increases game speed every 50 points, making the game progressively harder.
- Handled keyboard input to control the snake’s direction and reset the game using GLFW callbacks.
- Used CMake and a batch script to simplify building and running the project on Windows.

### How we did it:
- Initialized GLFW and created an OpenGL 3.3 Core Profile context using GLAD for loading OpenGL functions.
- Wrote GLSL shaders for rendering cells as colored quads.
- Managed game state with structures representing points, snake body, and food items.
- Used a main loop that updates game state at fixed time intervals and renders the scene each frame.
- Kept the code modular with clear separation between rendering, input handling, and game logic for maintainability and clarity.

This method balances simplicity and functionality, providing a solid foundation for a graphical Snake game that is easy to understand and extend.
