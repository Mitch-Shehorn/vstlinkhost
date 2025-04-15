#include <iostream>

#if JUCE_WINDOWS
  #include <windows.h>
#endif

// JUCE includes - add the JuceHeader.h first
#include <JuceHeader.h>
#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

// Core includes
#include "core/audio/AudioEngine.h"
#include "core/midi/MidiManager.h"

// Simplified basic application for initial testing
class SimpleAudioApp : public juce::JUCEApplication
{
public:
    SimpleAudioApp() {}

    const juce::String getApplicationName() override { return "VSTLinkHost"; }
    const juce::String getApplicationVersion() override { return "0.1.0"; }
    bool moreThanOneInstanceAllowed() override { return false; }

    void initialise(const juce::String& commandLine) override
    {
        std::cout << "Initializing VSTLinkHost (Basic Version)..." << std::endl;
        
        // Initialize components
        audioEngine = std::make_unique<AudioEngine>();
        midiManager = std::make_unique<MidiManager>();
        
        // Start components
        midiManager->initialize();
        audioEngine->start();
        
        // Create a simple window
        mainWindow = std::make_unique<MainWindow>(getApplicationName());
    }

    void shutdown() override
    {
        mainWindow = nullptr;
        audioEngine->stop();
    }

    void systemRequestedQuit() override
    {
        quit();
    }

    class MainWindow : public juce::DocumentWindow
    {
    public:
        MainWindow(juce::String name)
            : DocumentWindow(name,
                            juce::Colours::darkgrey,
                            DocumentWindow::allButtons)
        {
            // Create a simple component
            setContentOwned(new MainContentComponent(), true);
            
            setUsingNativeTitleBar(true);
            setResizable(true, true);
            setSize(600, 400);
            centreWithSize(getWidth(), getHeight());
            setVisible(true);
        }

        void closeButtonPressed() override
        {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }

        // Simple content component
        class MainContentComponent : public juce::Component
        {
        public:
            MainContentComponent()
            {
                addAndMakeVisible(infoLabel);
                infoLabel.setText("VSTLinkHost - Basic Version\n\n"
                                 "This is a simplified version for initial testing.\n"
                                 "Audio engine initialized.",
                                 juce::dontSendNotification);
                infoLabel.setJustificationType(juce::Justification::centred);
                
                setSize(600, 400);
            }
            
            void paint(juce::Graphics& g) override
            {
                g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
            }
            
            void resized() override
            {
                infoLabel.setBounds(getLocalBounds().reduced(20));
            }
            
        private:
            juce::Label infoLabel;
        };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
    std::unique_ptr<AudioEngine> audioEngine;
    std::unique_ptr<MidiManager> midiManager;
};

// This macro will start the application
START_JUCE_APPLICATION(SimpleAudioApp)
