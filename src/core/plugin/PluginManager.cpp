#include "PluginManager.h"

PluginManager::PluginManager()
{
    // Register available plugin formats
    formatManager.addDefaultFormats();
}

PluginManager::~PluginManager()
{
    // Clear loaded plugins
    loadedPlugins.clear();
}

bool PluginManager::loadPlugin(const juce::String& filePath)
{
    juce::File file(filePath);
    if (!file.exists())
        return false;
    
    // Find best matching format for this file
    auto* format = formatManager.findFormatForFile(file);
    if (format == nullptr)
        return false;
        
    // Scan for plugin description
    juce::PluginDescription description;
    juce::OwnedArray<juce::PluginDescription> descriptions;
    
    juce::String errorMessage;
    bool success = format->findAllTypesForFile(descriptions, file);
    
    if (!success || descriptions.isEmpty())
        return false;
        
    description = *descriptions[0];
    
    // Create plugin instance
    auto result = format->createPluginInstance(description, 44100.0, 512,
                                              [](const juce::String& error)
                                              {
                                                  juce::Logger::writeToLog("Plugin error: " + error);
                                              });
    
    if (result.get() != nullptr)
    {
        loadedPlugins.push_back(std::move(result));
        return true;
    }
    
    return false;
}

std::unique_ptr<juce::AudioPluginInstance> PluginManager::createPluginInstance(
    const juce::PluginDescription& desc, 
    double sampleRate, 
    int blockSize,
    juce::String& errorMessage)
{
    return formatManager.createPluginInstance(desc, sampleRate, blockSize, errorMessage);
}

void PluginManager::scanForPlugins(const juce::String& directoryPath)
{
    juce::File directory(directoryPath);
    if (!directory.isDirectory())
        return;
        
    // Clear existing plugin list
    knownPluginList.clear();
    
    // Scan for VSTs
    for (auto* format : formatManager.getFormats())
    {
        knownPluginList.scanAndAddFile(directory.getFullPathName(),
                                       true, // search recursively 
                                       knownPluginList.getBlacklistedFiles(),
                                       [this](const juce::String& name, float progress)
                                       {
                                           handlePluginScanProgress(name, progress);
                                       });
    }
}

const juce::KnownPluginList& PluginManager::getKnownPluginList() const
{
    return knownPluginList;
}

juce::AudioPluginFormatManager& PluginManager::getFormatManager()
{
    return formatManager;
}

const std::vector<std::unique_ptr<juce::AudioPluginInstance>>& PluginManager::getPlugins() const
{
    return loadedPlugins;
}

bool PluginManager::savePluginList(const juce::File& file)
{
    juce::XmlElement xml("PLUGIN_LIST");
    
    knownPluginList.createXml()->copyAttributesTo(xml);
    
    // Save to file
    return xml.writeTo(file);
}

bool PluginManager::loadPluginList(const juce::File& file)
{
    if (!file.exists())
        return false;
        
    std::unique_ptr<juce::XmlElement> xml = juce::XmlDocument::parse(file);
    
    if (xml == nullptr || xml->getTagName() != "PLUGIN_LIST")
        return false;
        
    knownPluginList.recreateFromXml(*xml);
    return true;
}

void PluginManager::setScanProgressCallback(ScanProgressCallback callback)
{
    scanProgressCallback = std::move(callback);
}

void PluginManager::handlePluginScanProgress(const juce::String& pluginBeingScanned, float progress)
{
    if (scanProgressCallback)
        scanProgressCallback(progress);
}