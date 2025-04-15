#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../panels/TransportPanel.h"
#include "../panels/PluginsPanel.h"
#include "../../core/audio/AudioEngine.h"
#include "../../core/plugin/PluginManager.h"
#include "../../core/midi/MidiManager.h"
#include "../../core/sync/LinkManager.h"
#include "../../core/sync/TransportManager.h"

class MainView : public juce::Component,
                public juce::Button::Listener
{
public:
    MainView(AudioEngine& audioEngine,
            MidiManager& midiManager,
            PluginManager& pluginManager,
            LinkManager& linkManager,
            TransportManager& transportManager);
            
    ~MainView() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void buttonClicked(juce::Button* button) override;

private:
    // References to core components
    AudioEngine& audioEngine;
    MidiManager& midiManager;
    PluginManager& pluginManager;
    LinkManager& linkManager;
    TransportManager& transportManager;

    // UI components
    juce::TabbedComponent mainTabs;
    TransportPanel transportPanel;
    PluginsPanel pluginsPanel;
    juce::TextButton audioSettingsButton;
    
    // Show audio settings dialog
    void showAudioSettings();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainView)
};
