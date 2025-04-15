# VSTLinkHost - Build Instructions for Windows

This document provides step-by-step instructions to build the VSTLinkHost application on Windows.

## Prerequisites

Before building, you need to install:

1. **CMake** (version 3.16 or higher)
   - Download from: https://cmake.org/download/
   - Make sure to add CMake to your system PATH during installation

2. **Visual Studio 2022** (Community edition is sufficient)
   - Download from: https://visualstudio.microsoft.com/vs/community/
   - During installation, make sure to include the "Desktop development with C++" workload

3. **Git** (optional, but recommended)
   - Download from: https://git-scm.com/download/win

## Setting Up Dependencies

The project requires two main external libraries:

### 1. JUCE Framework

Option A: Using Git (recommended)
```
cd vstlinkhost
git submodule add https://github.com/juce-framework/JUCE.git deps/juce
```

Option B: Manual download
- Download JUCE from: https://juce.com/get-juce/download
- Extract the contents to `vstlinkhost/deps/juce`

### 2. Ableton Link

Option A: Using Git (recommended)
```
cd vstlinkhost
git submodule add https://github.com/Ableton/link.git deps/link
```

Option B: Manual download
- Download Link from: https://github.com/Ableton/link (clone or download as ZIP)
- Extract the contents to `vstlinkhost/deps/link`

### 3. VST3 SDK (Optional, for VST plugin support)

- Download VST3 SDK from: https://steinbergmedia.github.io/vst3_dev_portal/pages/Downloads.html
- Extract the contents to `vstlinkhost/deps/vst3sdk`

## Building the Project

Once you have set up all dependencies, you can build the project using one of these methods:

### Method 1: Using the provided build script (Recommended)

1. Open Command Prompt or Windows PowerShell
2. Navigate to the project directory: `cd path\to\vstlinkhost`
3. Run the build script: `build_windows.bat`
4. The script will:
   - Check for dependencies
   - Create necessary directories
   - Configure the project with CMake
   - Build the project
   - Copy the executable and DLLs to the `release` directory

If the build is successful, you'll find the executable in the `release` directory.

### Method 2: Manual build using CMake

1. Open Command Prompt or Windows PowerShell
2. Navigate to the project directory: `cd path\to\vstlinkhost`
3. Create and enter a build directory:
   ```
   mkdir build
   cd build
   ```
4. Configure with CMake:
   ```
   cmake .. -G "Visual Studio 17 2022" -A x64
   ```
5. Build the project:
   ```
   cmake --build . --config Release
   ```
6. The executable will be in the `build\Release` directory

## Running the Application

1. Navigate to the `release` directory (if using the build script) or `build\Release` directory (if built manually)
2. Double-click on `VSTLinkHost.exe` to run the application

Note: The application needs to be run with administrator privileges if you want to scan system directories for VST plugins.

## Troubleshooting

### CMake not found
- Make sure CMake is installed and added to your system PATH
- Try restarting Command Prompt or PowerShell after installing CMake

### Build errors with JUCE or Link
- Ensure the directories `deps/juce` and `deps/link` contain the correct library files
- Check that the submodule initialization completed successfully

### VST support issues
- Make sure VST3 SDK is correctly installed in `deps/vst3sdk`
- Check that the FindVST3SDK.cmake file is present in the `cmake` directory

### Missing DLLs when running
- Make sure all required DLLs are in the same directory as the executable
- You may need to install the Visual C++ Redistributable packages from Microsoft

## Creating an Installer (Optional)

If you want to create an installer for distribution:

1. Download and install Inno Setup from: https://jrsoftware.org/isinfo.php
2. Create a script file similar to:
   ```
   [Setup]
   AppName=VSTLinkHost
   AppVersion=0.1.0
   DefaultDirName={pf}\VSTLinkHost
   DefaultGroupName=VSTLinkHost
   OutputBaseFilename=VSTLinkHost-Setup
   Compression=lzma
   SolidCompression=yes

   [Files]
   Source: "release\VSTLinkHost.exe"; DestDir: "{app}"; Flags: ignoreversion
   Source: "release\*.dll"; DestDir: "{app}"; Flags: ignoreversion

   [Icons]
   Name: "{group}\VSTLinkHost"; Filename: "{app}\VSTLinkHost.exe"
   Name: "{group}\Uninstall VSTLinkHost"; Filename: "{uninstallexe}"
   ```
3. Compile the script with Inno Setup to create your installer