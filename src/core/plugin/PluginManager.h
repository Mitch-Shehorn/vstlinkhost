#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

class PluginManager
{
public:
    PluginManager();
    ~PluginManager();

    // Load a plugin from a file
    bool loadPlugin(const juce::String& filePath);
    
    // Load a plugin from a description
    std::unique_ptr<juce::AudioPluginInstance> createPluginInstance(const juce::PluginDescription& desc, 
                                                                  double sampleRate, 
                                                                  int blockSize,
                                                                  juce::String& errorMessage);

    // Scan for plugins in a directory
    void scanForPlugins(const juce::String& directoryPath);
    
    // Get known plugin types after scanning
    const juce::KnownPluginList& getKnownPluginList() const;
    
    // Get available formats
    juce::AudioPluginFormatManager& getFormatManager();
    
    // Get loaded plugins
    const std::vector<std::unique_ptr<juce::AudioPluginInstance>>& getPlugins() const;
    
    // Save/load plugin list
    bool savePluginList(const juce::File& file);
    bool loadPluginList(const juce::File& file);
    
    // Set a callback to be notified when plugin scanning progress changes
    using ScanProgressCallback = std::function<void(float)>;
    void setScanProgressCallback(ScanProgressCallback callback);

private:
    juce::AudioPluginFormatManager formatManager;
    juce::KnownPluginList knownPluginList;
    std::vector<std::unique_ptr<juce::AudioPluginInstance>> loadedPlugins;
    ScanProgressCallback scanProgressCallback;
    
    // Plugin scan progress callback
    void handlePluginScanProgress(const juce::String& pluginBeingScanned, float progress);
};
