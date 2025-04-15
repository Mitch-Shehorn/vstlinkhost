#pragma once

#include <juce_core/juce_core.h>

namespace FileUtils
{
    // Check if a file exists
    bool fileExists(const juce::String& path);

    // Get the file extension
    juce::String getFileExtension(const juce::String& path);

    // Get the file name without extension
    juce::String getFileNameWithoutExtension(const juce::String& path);

    // Create a directory if it doesn't exist
    bool createDirectoryIfNotExists(const juce::String& path);

    // Save a file with content
    bool saveToFile(const juce::String& path, const juce::String& content);

    // Load a file's content as string
    juce::String loadFileAsString(const juce::String& path);
}
