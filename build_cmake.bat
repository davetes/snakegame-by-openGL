@echo off
echo Building with CMake...

:: Create build directory if it doesn't exist
if not exist build mkdir build

:: Change to build directory
cd build

:: Generate project files with CMake
cmake .. -G "MinGW Makefiles"

:: Build the project
cmake --build .

:: Check if build was successful
if %ERRORLEVEL% EQU 0 (
    echo Build successful!
) else (
    echo Build failed with error code %ERRORLEVEL%
)

:: Return to the main directory
cd .. 