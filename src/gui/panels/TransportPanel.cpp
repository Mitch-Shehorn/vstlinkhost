#include "TransportPanel.h"

TransportPanel::TransportPanel(LinkManager& linkManagerToUse)
    : linkManager(linkManagerToUse)
{
    // Transport controls
    addAndMakeVisible(playButton);
    playButton.setButtonText("Play");
    playButton.addListener(this);
    
    addAndMakeVisible(stopButton);
    stopButton.setButtonText("Stop");
    stopButton.addListener(this);
    
    // Tempo control
    addAndMakeVisible(tempoSlider);
    tempoSlider.setRange(60.0, 200.0, 1.0);
    tempoSlider.setValue(120.0); // Default value
    tempoSlider.setTextValueSuffix(" BPM");
    tempoSlider.addListener(this);
    
    addAndMakeVisible(tempoLabel);
    tempoLabel.setText("Tempo", juce::dontSendNotification);
    tempoLabel.attachToComponent(&tempoSlider, true);
    
    // Link control
    addAndMakeVisible(linkEnabledButton);
    linkEnabledButton.setButtonText("Enable Link");
    linkEnabledButton.setToggleState(linkManager.isEnabled(), juce::dontSendNotification);
    linkEnabledButton.addListener(this);
    
    setSize(600, 100);
}

TransportPanel::~TransportPanel()
{
}

void TransportPanel::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void TransportPanel::resized()
{
    auto area = getLocalBounds().reduced(10);
    
    // Position transport controls
    auto transportControls = area.removeFromTop(30);
    playButton.setBounds(transportControls.removeFromLeft(100));
    transportControls.removeFromLeft(10);
    stopButton.setBounds(transportControls.removeFromLeft(100));
    
    // Position Ableton Link button
    transportControls.removeFromLeft(20);
    linkEnabledButton.setBounds(transportControls.removeFromLeft(150));
    
    // Position tempo controls
    auto tempoArea = area.removeFromTop(30);
    tempoArea.removeFromLeft(60); // Make space for label
    tempoSlider.setBounds(tempoArea);
}

void TransportPanel::buttonClicked(juce::Button* button)
{
    if (button == &playButton)
    {
        // Start playback functionality would be implemented here
        // For now, just update the UI
        playButton.setEnabled(false);
        stopButton.setEnabled(true);
    }
    else if (button == &stopButton)
    {
        // Stop playback functionality would be implemented here
        // For now, just update the UI
        playButton.setEnabled(true);
        stopButton.setEnabled(false);
    }
    else if (button == &linkEnabledButton)
    {
        // Enable/disable Ableton Link
        linkManager.enable(linkEnabledButton.getToggleState());
    }
}

void TransportPanel::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &tempoSlider)
    {
        // Update tempo
        double tempo = slider->getValue();
        linkManager.setTempo(tempo);
    }
}