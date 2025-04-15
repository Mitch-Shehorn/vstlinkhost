#pragma once

#include <juce_core/juce_core.h>

class Config
{
public:
    static Config& getInstance();

    // Get a string value from config
    juce::String getString(const juce::String& key, const juce::String& defaultValue = "");

    // Set a string value in config
    void setString(const juce::String& key, const juce::String& value);

    // Get an integer value from config
    int getInt(const juce::String& key, int defaultValue = 0);

    // Set an integer value in config
    void setInt(const juce::String& key, int value);

    // Get a float value from config
    float getFloat(const juce::String& key, float defaultValue = 0.0f);

    // Set a float value in config
    void setFloat(const juce::String& key, float value);

    // Get a boolean value from config
    bool getBool(const juce::String& key, bool defaultValue = false);

    // Set a boolean value in config
    void setBool(const juce::String& key, bool value);

    // Save configuration to file
    bool save();

    // Load configuration from file
    bool load();

    // Get the configuration file path
    juce::String getConfigFilePath() const;

private:
    Config(); // Private constructor for singleton
    ~Config();

    juce::PropertiesFile::Options options;
    std::unique_ptr<juce::PropertiesFile> properties;
};
