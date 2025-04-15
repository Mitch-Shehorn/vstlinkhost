@echo off
setlocal enabledelayedexpansion

echo ====================================
echo VSTLinkHost Dependency Setup Script
echo ====================================
echo.

:: Create deps directory if it doesn't exist
if not exist "deps" (
    mkdir deps
    echo Created deps directory.
)

:: Check if Git is installed
where git >nul 2>nul
if %ERRORLEVEL% EQU 0 (
    echo Git found, will use it to download dependencies.
    set GIT_AVAILABLE=1
) else (
    echo Git not found, will use manual download methods.
    set GIT_AVAILABLE=0
)

echo.
echo Setting up dependencies...
echo.

:: Setup JUCE Framework
echo Setting up JUCE framework...
if not exist "deps\juce" (
    mkdir deps\juce 2>nul
    
    if %GIT_AVAILABLE% EQU 1 (
        git clone https://github.com/juce-framework/JUCE.git deps\juce
        if exist "deps\juce\CMakeLists.txt" (
            echo JUCE framework set up successfully via Git.
        ) else (
            echo Failed to download JUCE, please set it up manually.
            rmdir /s /q deps\juce 2>nul
            mkdir deps\juce 2>nul
            
            echo Downloading JUCE manually...
            start "" "https://juce.com/get-juce/download"
            echo Please download JUCE, extract it to: %CD%\deps\juce
            echo The JUCE folder should contain a CMakeLists.txt file.
            echo.
            echo Press any key when done...
            pause >nul
        )
    ) else (
        echo Downloading JUCE manually...
        start "" "https://juce.com/get-juce/download"
        echo Please download JUCE, extract it to: %CD%\deps\juce
        echo The JUCE folder should contain a CMakeLists.txt file.
        echo.
        echo Press any key when done...
        pause >nul
    )
) else (
    echo JUCE directory already exists.
    if not exist "deps\juce\CMakeLists.txt" (
        echo WARNING: JUCE appears to be missing its CMakeLists.txt file.
        echo This might indicate an incomplete or incorrect JUCE installation.
        echo.
        echo Would you like to clear the directory and set up JUCE again? (y/n)
        set /p JUCE_REINSTALL="Enter choice (y/n): "
        
        if /i "!JUCE_REINSTALL!"=="y" (
            echo Removing existing JUCE directory...
            rmdir /s /q deps\juce
            mkdir deps\juce
            
            echo Downloading JUCE manually...
            start "" "https://juce.com/get-juce/download"
            echo Please download JUCE, extract it to: %CD%\deps\juce
            echo The JUCE folder should contain a CMakeLists.txt file.
            echo.
            echo Press any key when done...
            pause >nul
        )
    ) else (
        echo JUCE framework exists and looks valid, skipping.
    )
)

:: Setup Ableton Link
echo Setting up Ableton Link...
if not exist "deps\link" (
    mkdir deps\link 2>nul
    
    if %GIT_AVAILABLE% EQU 1 (
        git clone https://github.com/Ableton/link.git deps\link
        if exist "deps\link\include\ableton" (
            echo Ableton Link set up successfully via Git.
        ) else (
            echo Failed to download Ableton Link, please set it up manually.
            rmdir /s /q deps\link 2>nul
            mkdir deps\link 2>nul
            
            echo Downloading Ableton Link manually...
            start "" "https://github.com/Ableton/link/archive/refs/heads/master.zip"
            echo Please download Ableton Link, extract it to: %CD%\deps\link
            echo The folder should contain an "include" folder with "ableton" inside.
            echo.
            echo Press any key when done...
            pause >nul
        )
    ) else (
        echo Downloading Ableton Link manually...
        start "" "https://github.com/Ableton/link/archive/refs/heads/master.zip"
        echo Please download Ableton Link, extract it to: %CD%\deps\link
        echo The folder should contain an "include" folder with "ableton" inside.
        echo.
        echo Press any key when done...
        pause >nul
    )
) else (
    echo Ableton Link directory already exists.
    if not exist "deps\link\include\ableton" (
        echo WARNING: Ableton Link appears to be missing the include/ableton directory.
        echo This might indicate an incomplete or incorrect installation.
        echo.
        echo Would you like to clear the directory and set up Ableton Link again? (y/n)
        set /p LINK_REINSTALL="Enter choice (y/n): "
        
        if /i "!LINK_REINSTALL!"=="y" (
            echo Removing existing Ableton Link directory...
            rmdir /s /q deps\link
            mkdir deps\link
            
            echo Downloading Ableton Link manually...
            start "" "https://github.com/Ableton/link/archive/refs/heads/master.zip"
            echo Please download Ableton Link, extract it to: %CD%\deps\link
            echo The folder should contain an "include" folder with "ableton" inside.
            echo.
            echo Press any key when done...
            pause >nul
        )
    ) else (
        echo Ableton Link exists and looks valid, skipping.
    )
)

:: Setup VST3 SDK (Optional)
echo Setting up VST3 SDK...
if not exist "deps\vst3sdk" (
    echo.
    echo VST3 SDK is required for plugin support. Do you want to:
    echo 1. Download it manually (recommended, will open browser)
    echo 2. Skip for now (you can add it later)
    echo.
    set /p VSTSDK_CHOICE="Enter choice (1/2): "
    
    if "!VSTSDK_CHOICE!"=="1" (
        mkdir deps\vst3sdk 2>nul
        
        echo Opening VST3 SDK download page...
        start "" "https://steinbergmedia.github.io/vst3_dev_portal/pages/Downloads.html"
        echo After downloading, please extract the SDK contents to: %CD%\deps\vst3sdk
        echo Press any key when done...
        pause >nul
    ) else (
        echo Skipping VST3 SDK setup. You can add it later by extracting it to: %CD%\deps\vst3sdk
    )
) else (
    echo VST3 SDK directory already exists, skipping.
)

:: Final report
echo.
echo Checking dependency status:
echo.

if exist "deps\juce\CMakeLists.txt" (
    echo [OK] JUCE framework: FOUND
) else (
    echo [ERROR] JUCE framework: MISSING or INVALID
    echo         Make sure deps\juce contains a CMakeLists.txt file.
)

if exist "deps\link\include\ableton" (
    echo [OK] Ableton Link: FOUND
) else (
    echo [ERROR] Ableton Link: MISSING or INVALID
    echo         Make sure deps\link\include\ableton directory exists.
)

if exist "deps\vst3sdk" (
    echo [OK] VST3 SDK: FOUND
) else (
    echo [WARNING] VST3 SDK: NOT FOUND (optional)
)

echo.
echo ====================================
echo Dependency setup completed!
echo ====================================
echo.
echo To build the application, run:
echo build_windows.bat
echo.

endlocal