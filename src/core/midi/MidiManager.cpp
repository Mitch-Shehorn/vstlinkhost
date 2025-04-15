#include "MidiManager.h"

MidiManager::MidiManager()
{
}

MidiManager::~MidiManager()
{
    // Close any open MIDI connections
    midiInput.reset();
    midiOutput.reset();
}

bool MidiManager::initialize()
{
    return true; // Basic initialization successful
}

juce::StringArray MidiManager::getMidiInputDevices() const
{
    return juce::MidiInput::getDevices();
}

juce::StringArray MidiManager::getMidiOutputDevices() const
{
    return juce::MidiOutput::getDevices();
}

bool MidiManager::enableMidiInput(int index, bool enable)
{
    if (index < 0 || index >= juce::MidiInput::getDevices().size())
        return false;
    
    if (enable)
    {
        // Close any existing connection
        midiInput.reset();
        
        // Create a new MIDI input connection
        auto deviceName = juce::MidiInput::getDevices()[index];
        midiInput = juce::MidiInput::openDevice(index, nullptr);
        
        if (midiInput == nullptr)
            return false;
        
        midiInput->start();
    }
    else
    {
        // Close the MIDI input
        if (midiInput != nullptr)
        {
            midiInput->stop();
            midiInput.reset();
        }
    }
    
    return true;
}

bool MidiManager::setMidiOutput(int index)
{
    if (index < 0 || index >= juce::MidiOutput::getDevices().size())
        return false;
    
    // Close any existing connection
    midiOutput.reset();
    
    // Create a new MIDI output connection
    auto deviceName = juce::MidiOutput::getDevices()[index];
    midiOutput = juce::MidiOutput::openDevice(index);
    
    return midiOutput != nullptr;
}