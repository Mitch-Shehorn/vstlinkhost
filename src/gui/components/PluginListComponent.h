#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <memory>
#include "../../core/plugin/PluginManager.h"
#include "../../core/audio/AudioEngine.h"

class PluginListComponent : public juce::Component,
                            public juce::TableListBoxModel,
                            public juce::Button::Listener,
                            public juce::FileDragAndDropTarget
{
public:
    PluginListComponent(PluginManager& pluginManagerToUse, AudioEngine& audioEngineToUse);
    ~PluginListComponent() override;
    
    // Component overrides
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // TableListBoxModel overrides
    int getNumRows() override;
    void paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
    void paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
    juce::Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, juce::Component* existingComponentToUpdate) override;
    
    // Button::Listener override
    void buttonClicked(juce::Button* button) override;
    
    // FileDragAndDropTarget overrides
    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;
    
    // Scan for plugins in the default locations
    void scanForPlugins();
    
    // Scan for plugins in a specific directory
    void scanForPluginsInDirectory(const juce::File& directory);
    
    // Load a plugin by index
    bool loadPlugin(int index);
    
    // Load a plugin from file
    bool loadPluginFromFile(const juce::File& file);
    
    // Update the table to reflect current plugin list
    void updateTable();
    
private:
    PluginManager& pluginManager;
    AudioEngine& audioEngine;
    
    juce::TableListBox pluginTable;
    juce::TextButton scanButton;
    juce::TextButton addPluginButton;
    
    juce::Label scanProgressLabel;
    std::atomic<float> currentScanProgress;
    
    // Handle scan progress
    void handleScanProgress(float progress);
    
    // Show plugin settings dialog
    void showPluginSettings();
    
    // Show plugin options menu
    void showPluginMenu(int rowNumber, int x, int y);
    
    // Remove a plugin by index
    void removePlugin(int index);
    
    // Scan for plugins in standard locations
    void scanStandardLocations();
    
    // Additional utility methods
    juce::File getDefaultPluginSettingsFile();
    void savePluginList();
    void loadPluginList();
    
    enum ColumnIds
    {
        NameColumn = 1,
        CategoryColumn,
        ManufacturerColumn,
        FormatColumn,
        LoadColumn
    };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginListComponent)
};