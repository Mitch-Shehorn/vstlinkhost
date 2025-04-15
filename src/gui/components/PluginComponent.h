#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

class PluginComponent : public juce::Component
{
public:
    PluginComponent(juce::AudioPluginInstance* plugin);
    ~PluginComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    // Show/hide plugin editor
    void showEditor(bool show);

    // Get the plugin instance
    juce::AudioPluginInstance* getPlugin() const;

private:
    juce::AudioPluginInstance* plugin;
    std::unique_ptr<juce::AudioProcessorEditor> editor;
    bool editorVisible;
};
