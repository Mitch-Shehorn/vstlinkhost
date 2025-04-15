@echo off
setlocal enabledelayedexpansion

echo ====================================
echo VSTLinkHost Windows Build Script
echo ====================================
echo.

:: Check if CMake is installed
where cmake >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: CMake not found. Please install CMake from https://cmake.org/download/
    echo        and make sure it's in your PATH.
    goto :error
)

:: Check if Visual Studio is installed (by checking for common installation paths)
set VS_FOUND=0
if exist "C:\Program Files\Microsoft Visual Studio\2022\Community" set VS_FOUND=1
if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional" set VS_FOUND=1
if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise" set VS_FOUND=1
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2022\Community" set VS_FOUND=1
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2022\Professional" set VS_FOUND=1
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2022\Enterprise" set VS_FOUND=1

if %VS_FOUND% EQU 0 (
    echo WARNING: Visual Studio 2022 not found in standard locations.
    echo          Make sure Visual Studio 2022 is installed with C++ desktop development workload.
    echo          We'll try to continue, but the build might fail.
    echo.
)

:: Check for dependencies
if not exist "deps" (
    echo ERROR: deps directory not found.
    echo        Please create a deps directory with the required dependencies:
    echo        - JUCE: deps/juce
    echo        - Ableton Link: deps/link
    goto :error
)

if not exist "deps\juce" (
    echo ERROR: JUCE not found in deps/juce.
    echo        Please download JUCE using:
    echo        git submodule add https://github.com/juce-framework/JUCE.git deps/juce
    echo        or manually download it from https://juce.com/get-juce/download
    goto :error
)

if not exist "deps\link" (
    echo ERROR: Ableton Link not found in deps/link.
    echo        Please download Ableton Link using:
    echo        git submodule add https://github.com/Ableton/link.git deps/link
    echo        or manually download it from https://github.com/Ableton/link
    goto :error
)

:: Make sure the cmake directory exists
if not exist "cmake" (
    mkdir cmake
    echo Created cmake directory.
)

:: Check for VST3SDK finder module
if not exist "cmake\FindVST3SDK.cmake" (
    echo Creating FindVST3SDK.cmake module...
    (
        echo # Find the VST3 SDK
        echo # Once done this will define
        echo # VST3SDK_FOUND - System has VST3SDK
        echo # VST3SDK_INCLUDE_DIRS - The VST3SDK include directories
        echo.
        echo # Look for VST3SDK in standard locations
        echo set^(VST3SDK_SEARCH_PATHS
        echo     "${CMAKE_CURRENT_SOURCE_DIR}/deps/vst3sdk"
        echo     "$ENV{VST3SDK_ROOT}"
        echo     "$ENV{ProgramFiles}/Steinberg/VST3 SDK"
        echo     "$ENV{ProgramFiles^(x86^)}/Steinberg/VST3 SDK"
        echo ^)
        echo.
        echo # Find the VST3 SDK root directory
        echo find_path^(VST3SDK_ROOT
        echo     NAMES "base/source/fobject.cpp"
        echo     PATHS ${VST3SDK_SEARCH_PATHS}
        echo ^)
        echo.
        echo if^(VST3SDK_ROOT^)
        echo     set^(VST3SDK_INCLUDE_DIRS
        echo         "${VST3SDK_ROOT}"
        echo         "${VST3SDK_ROOT}/base"
        echo         "${VST3SDK_ROOT}/pluginterfaces"
        echo         "${VST3SDK_ROOT}/public.sdk"
        echo     ^)
        echo     
        echo     set^(VST3SDK_FOUND TRUE^)
        echo else^(^)
        echo     set^(VST3SDK_FOUND FALSE^)
        echo endif^(^)
        echo.
        echo mark_as_advanced^(VST3SDK_ROOT^)
    ) > cmake\FindVST3SDK.cmake
    echo Created cmake\FindVST3SDK.cmake
)

:: Create build directory if it doesn't exist
if not exist "build" (
    mkdir build
    echo Created build directory.
)

:: Check if src directory and main source files exist
if not exist "src\main.cpp" (
    echo ERROR: Source files not found. Make sure you're running this from the project root.
    goto :error
)

echo.
echo Starting build process...
echo.

cd build

:: Configure the project with CMake
echo Configuring project with CMake...

:: Try to detect the best Visual Studio generator
set VS_GENERATOR="Visual Studio 17 2022"

:: Check for Visual Studio 2022
if exist "C:\Program Files\Microsoft Visual Studio\2022" set VS_GENERATOR="Visual Studio 17 2022"
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2022" set VS_GENERATOR="Visual Studio 17 2022"

:: Check for Visual Studio 2019
if not exist "C:\Program Files\Microsoft Visual Studio\2022" (
    if not exist "C:\Program Files (x86)\Microsoft Visual Studio\2022" (
        if exist "C:\Program Files\Microsoft Visual Studio\2019" set VS_GENERATOR="Visual Studio 16 2019"
        if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019" set VS_GENERATOR="Visual Studio 16 2019"
    )
)

echo Using Visual Studio generator: %VS_GENERATOR%
cmake .. -G %VS_GENERATOR% -A x64
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: CMake configuration failed.
    echo.
    echo If you're seeing errors related to JUCE or Ableton Link, please try:
    echo 1. Run setup_dependencies.bat first to set up dependencies
    echo 2. Make sure the deps/juce and deps/link directories contain the correct files
    echo 3. Try running this script again
    cd ..
    goto :error
)

:: Build the project
echo.
echo Building project...
cmake --build . --config Release
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Build failed.
    cd ..
    goto :error
)

cd ..

:: Create release directory if it doesn't exist
if not exist "release" (
    mkdir release
    echo Created release directory.
)

:: Copy the executable to the release directory
echo.
echo Copying executable to release directory...
copy "build\VSTLinkHost_artefacts\Release\VSTLinkHost.exe" "release\" >nul
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to copy executable.
    goto :error
)

:: Check if we need to copy any DLLs (only if they exist)
if exist "build\VSTLinkHost_artefacts\Release\*.dll" (
    echo Copying DLLs to release directory...
    copy "build\VSTLinkHost_artefacts\Release\*.dll" "release\" >nul
    if %ERRORLEVEL% NEQ 0 (
        echo WARNING: Failed to copy DLLs.
    )
)

echo.
echo ====================================
echo Build completed successfully!
echo ====================================
echo.
echo The executable is available in the release directory:
echo %CD%\release\VSTLinkHost.exe
echo.
echo To run the application, go to the release directory and run VSTLinkHost.exe.
echo.
goto :end

:error
echo.
echo ====================================
echo Build failed.
echo ====================================
echo.

:end
endlocal