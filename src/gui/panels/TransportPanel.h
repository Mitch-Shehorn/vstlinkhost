#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../../core/sync/LinkManager.h"

class TransportPanel : public juce::Component,
                        public juce::Button::Listener,
                        public juce::Slider::Listener
{
public:
    TransportPanel(LinkManager& linkManager);
    ~TransportPanel() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    // Button::Listener implementation
    void buttonClicked(juce::Button* button) override;

    // Slider::Listener implementation
    void sliderValueChanged(juce::Slider* slider) override;

private:
    LinkManager& linkManager;

    juce::TextButton playButton;
    juce::TextButton stopButton;
    juce::Slider tempoSlider;
    juce::Label tempoLabel;
    juce::ToggleButton linkEnabledButton;
};
