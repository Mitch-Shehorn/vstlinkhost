@echo off
echo ====================================
echo VSTLinkHost Simple Setup Script
echo ====================================
echo.

echo Creating deps directory if it doesn't exist...
if not exist "deps" mkdir deps

echo Checking JUCE framework...
if exist "deps\juce\CMakeLists.txt" (
    echo JUCE framework found.
) else (
    echo JUCE framework not found or invalid.
    echo Please download JUCE from: https://juce.com/get-juce/download
    echo Extract it to: %CD%\deps\juce
    echo The JUCE folder must contain a CMakeLists.txt file.
    start "" "https://juce.com/get-juce/download"
    echo Press any key when done...
    pause >nul
)

echo Checking Ableton Link...
if exist "deps\link\include\ableton" (
    echo Ableton Link found.
) else (
    echo Ableton Link not found or invalid.
    echo Please download Ableton Link from: https://github.com/Ableton/link/archive/refs/heads/master.zip
    echo Extract it to: %CD%\deps\link
    echo The link folder must contain an include/ableton directory.
    start "" "https://github.com/Ableton/link/archive/refs/heads/master.zip"
    echo Press any key when done...
    pause >nul
)

echo Checking VST3 SDK (optional)...
if exist "deps\vst3sdk" (
    echo VST3 SDK found.
) else (
    echo VST3 SDK not found. This is optional but recommended for plugin support.
    echo Download from: https://steinbergmedia.github.io/vst3_dev_portal/pages/Downloads.html
    echo Extract to: %CD%\deps\vst3sdk
    echo.
    echo Do you want to download VST3 SDK now? (y/n)
    set /p SDK_CHOICE=
    if /i "%SDK_CHOICE%"=="y" (
        start "" "https://steinbergmedia.github.io/vst3_dev_portal/pages/Downloads.html"
        echo Press any key when done...
        pause >nul
    )
)

echo.
echo ====================================
echo Current dependency status:
echo ====================================

if exist "deps\juce\CMakeLists.txt" (
    echo [OK] JUCE framework
) else (
    echo [MISSING] JUCE framework - Required
)

if exist "deps\link\include\ableton" (
    echo [OK] Ableton Link
) else (
    echo [MISSING] Ableton Link - Required
)

if exist "deps\vst3sdk" (
    echo [OK] VST3 SDK
) else (
    echo [MISSING] VST3 SDK - Optional
)

echo.
echo When all required dependencies are set up, run:
echo build_windows.bat
echo.