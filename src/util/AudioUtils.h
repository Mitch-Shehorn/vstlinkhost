#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

namespace AudioUtils
{
    // Convert linear gain to decibels
    float gainToDecibels(float gain);

    // Convert decibels to linear gain
    float decibelsToGain(float decibels);

    // Format time in seconds to minutes:seconds format
    juce::String formatTime(double timeInSeconds);

    // Get note name from MIDI note number
    juce::String getMidiNoteName(int midiNoteNumber, bool includeOctave = true);

    // Calculate frequency from MIDI note number
    float getMidiNoteFrequency(int midiNoteNumber);
}
