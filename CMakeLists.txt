cmake_minimum_required(VERSION 3.16)
project(VSTLinkHost VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add custom CMake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Dependencies
add_subdirectory(deps/juce)

# Link setup
set(LINK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/link")

# Add compatibility definition for older JUCE versions
add_definitions(-DJUCE_COMPATIBILITY_MODE)

# Source files
set(SOURCES
    src/main.cpp
    src/core/audio/AudioEngine.cpp
    src/core/midi/MidiManager.cpp
    # src/core/plugin/PluginManager.cpp
    # src/core/sync/LinkManager.cpp
    # src/core/sync/TransportManager.cpp
    # src/gui/views/MainView.cpp
    # src/gui/panels/PluginsPanel.cpp
    # src/gui/panels/TransportPanel.cpp
    # src/gui/components/PluginListComponent.cpp
)

# Create JUCE application properly
juce_add_gui_app(${PROJECT_NAME}
    VERSION ${PROJECT_VERSION}
    COMPANY_NAME "VSTLinkHost"
    COMPANY_COPYRIGHT "Copyright (c) 2024"
    PRODUCT_NAME "VSTLinkHost"
)

# Configure application
juce_generate_juce_header(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${LINK_DIR}/include
    ${LINK_DIR}/modules/asio-standalone/asio/include
)

# JUCE setup
target_link_libraries(${PROJECT_NAME} PRIVATE
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_core
    juce::juce_data_structures
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
)

# Platform-specific setup
if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE pthread dl)
elseif(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE winmm)
elseif(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        "-framework CoreAudio"
        "-framework CoreMIDI"
        "-framework AudioToolbox"
        "-framework Accelerate"
        "-framework QuartzCore"
        "-framework AudioUnit"
        "-framework CoreServices"
    )
endif()

# Tests
enable_testing()
add_subdirectory(tests)
