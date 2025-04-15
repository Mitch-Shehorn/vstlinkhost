#include "MainView.h"

MainView::MainView(AudioEngine& audioEngineToUse,
                   MidiManager& midiManagerToUse,
                   PluginManager& pluginManagerToUse,
                   LinkManager& linkManagerToUse,
                   TransportManager& transportManagerToUse)
    : audioEngine(audioEngineToUse),
      midiManager(midiManagerToUse),
      pluginManager(pluginManagerToUse),
      linkManager(linkManagerToUse),
      transportManager(transportManagerToUse),
      mainTabs(juce::TabbedButtonBar::TabsAtTop),
      transportPanel(linkManagerToUse),
      pluginsPanel(pluginManagerToUse, audioEngineToUse)
{
    // Main tabs setup
    addAndMakeVisible(mainTabs);
    
    mainTabs.addTab("Plugins", juce::Colours::lightgrey, &pluginsPanel, false);
    // Add other tabs here as needed (mixer, settings, etc.)
    
    // Transport panel
    addAndMakeVisible(transportPanel);
    
    // Audio settings button
    addAndMakeVisible(audioSettingsButton);
    audioSettingsButton.setButtonText("Audio Settings");
    audioSettingsButton.addListener(this);
}

MainView::~MainView()
{
}

void MainView::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MainView::resized()
{
    auto area = getLocalBounds();
    
    // Transport panel at the top
    transportPanel.setBounds(area.removeFromTop(80));
    
    // Audio settings button at the bottom
    audioSettingsButton.setBounds(area.removeFromBottom(30).reduced(2));
    
    // Main tabs fill the remaining space
    mainTabs.setBounds(area);
}

void MainView::buttonClicked(juce::Button* button)
{
    if (button == &audioSettingsButton)
    {
        showAudioSettings();
    }
}

void MainView::showAudioSettings()
{
    auto* audioDeviceSelectorComponent = new juce::AudioDeviceSelectorComponent(
        audioEngine.getDeviceManager(),  // Use a getter from AudioEngine to expose the device manager
        0, 2,  // Min/max input channels
        0, 2,  // Min/max output channels
        false,  // Show MIDI input options
        false,  // Show MIDI output options
        false,  // Show channel options
        false); // Show advanced options
    
    audioDeviceSelectorComponent->setSize(600, 400);
    
    juce::DialogWindow::LaunchOptions options;
    options.content.setOwned(audioDeviceSelectorComponent);
    options.dialogTitle = "Audio Settings";
    options.dialogBackgroundColour = juce::Colours::lightgrey;
    options.escapeKeyTriggersCloseButton = true;
    options.useNativeTitleBar = true;
    options.resizable = false;
    
    options.launchAsync();
}