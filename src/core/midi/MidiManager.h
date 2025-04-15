#pragma once

#include <juce_audio_devices/juce_audio_devices.h>
#include <vector>
#include <memory>

class MidiManager
{
public:
    MidiManager();
    ~MidiManager();

    // Initialize MIDI devices
    bool initialize();

    // Get available MIDI input devices
    juce::StringArray getMidiInputDevices() const;

    // Get available MIDI output devices
    juce::StringArray getMidiOutputDevices() const;

    // Enable a MIDI input device by index
    bool enableMidiInput(int index, bool enable = true);

    // Set a MIDI output device by index
    bool setMidiOutput(int index);

private:
    std::unique_ptr<juce::MidiInput> midiInput;
    std::unique_ptr<juce::MidiOutput> midiOutput;
};
