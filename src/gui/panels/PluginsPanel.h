#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../components/PluginListComponent.h"
#include "../../core/plugin/PluginManager.h"
#include "../../core/audio/AudioEngine.h"

class PluginsPanel : public juce::Component,
                     public juce::Button::Listener
{
public:
    PluginsPanel(PluginManager& pluginManager, AudioEngine& audioEngine);
    ~PluginsPanel() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void buttonClicked(juce::Button* button) override;
    
private:
    juce::TabbedComponent tabbedComponent;
    PluginListComponent pluginListComponent;
    
    // Active plugins list
    class ActivePluginsComponent : public juce::Component,
                                   public juce::TableListBoxModel,
                                   public juce::Button::Listener
    {
    public:
        ActivePluginsComponent(AudioEngine& audioEngine, PluginManager& pluginManager);
        ~ActivePluginsComponent() override;
        
        void paint(juce::Graphics& g) override;
        void resized() override;
        
        int getNumRows() override;
        void paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
        void paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
        juce::Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, juce::Component* existingComponentToUpdate) override;
        
        void buttonClicked(juce::Button* button) override;
        
        void updateList();
        
    private:
        AudioEngine& audioEngine;
        PluginManager& pluginManager;
        
        juce::TableListBox activePluginsTable;
        juce::TextButton clearButton;
        
        void showPluginWindow(int index);
        void removePlugin(int index);
        
        enum ColumnIds
        {
            NameColumn = 1,
            FormatColumn,
            ControlsColumn
        };
    };
    
    ActivePluginsComponent activePluginsComponent;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginsPanel)
};