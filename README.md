### Current Development Stage: Early Alpha

# VSTLinkHost

A VST plugin host with Ableton Link integration for synchronized music creation.

## Features

- Load and manage VST plugins
- Synchronize with other music applications via Ableton Link
- Route MIDI between plugins
- Process audio in real-time

## Building on Windows

### Quick Start

1. Make sure you have [Visual Studio 2022](https://visualstudio.microsoft.com/vs/community/) with C++ desktop development workload installed
2. Run `setup_dependencies.bat` to set up JUCE and Ableton Link
3. Run `build_windows.bat` to build the application
4. Find the executable in the `release` directory

### Manual Build Instructions

For detailed manual build instructions, see [BUILD_INSTRUCTIONS.md](BUILD_INSTRUCTIONS.md).

## Project Structure

- `src/core`: Core functionality (audio, MIDI, plugin management, synchronization)
- `src/gui`: User interface components
- `src/util`: Utility classes and functions

## Dependencies

- [JUCE](https://juce.com/): Audio application framework
- [Ableton Link](https://github.com/Ableton/link): Technology for synchronizing musical tempo
- [VST3 SDK](https://steinbergmedia.github.io/vst3_dev_portal/pages/Downloads.html) (optional): For additional plugin support

## Troubleshooting

### Missing Dependencies

If you see errors about missing JUCE or Ableton Link:
1. Make sure you've run `setup_dependencies.bat` first
2. Check that the `deps` directory contains:
   - `deps/juce` with JUCE framework files
   - `deps/link` with Ableton Link files
3. If directories exist but files are missing, delete them and run `setup_dependencies.bat` again

### Build Errors

- **CMake errors**: Make sure CMake 3.16+ is installed and in your PATH
- **Visual Studio errors**: Ensure VS2022 with C++ desktop development is installed
- **Compiler errors**: Check that your VS installation includes the necessary C++ components

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
