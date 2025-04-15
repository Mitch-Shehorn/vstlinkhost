#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <functional>
#include "LinkManager.h"

class TransportManager : public juce::Timer
{
public:
    TransportManager(LinkManager& linkManagerToUse);
    ~TransportManager();
    
    // Start/stop playback
    void startPlayback();
    void stopPlayback();
    void togglePlayback();
    
    // Is the transport currently playing?
    bool isPlaying() const;
    
    // Set the tempo in BPM
    void setTempo(double bpm);
    
    // Get the current tempo
    double getTempo() const;
    
    // Set the meter (time signature)
    void setMeter(int numerator, int denominator);
    
    // Get the current meter/time signature
    void getMeter(int& numerator, int& denominator) const;
    
    // Get the current beat position
    double getBeatPosition() const;
    
    // Set the beat position
    void setBeatPosition(double beatPosition);
    
    // Get beat phase (0-1 representing position in current bar)
    double getPhase() const;
    
    // Get the current bar/beat display
    juce::String getPositionDisplay() const;
    
    // Get the bar number (1-based)
    int getBarNumber() const;
    
    // Get the beat number within the current bar (1-based)
    int getBeatNumber() const;
    
    // Register callbacks for various transport events
    using TransportCallback = std::function<void()>;
    using TempoCallback = std::function<void(double)>;
    using PeersCallback = std::function<void(int)>;
    using BeatCallback = std::function<void(double)>;
    
    void setTransportCallback(TransportCallback callback);
    void setTempoCallback(TempoCallback callback);
    void setPeersCallback(PeersCallback callback);
    void setBeatCallback(BeatCallback callback);
    
    // Get number of Link peers
    int getNumPeers() const;
    
    // Timer callback
    void timerCallback() override;
    
private:
    LinkManager& linkManager;
    
    bool playing;
    int meterNumerator;
    int meterDenominator;
    double beatPosition;
    double phase;
    
    // Callbacks
    TransportCallback transportCallback;
    TempoCallback tempoCallback;
    PeersCallback peersCallback;
    BeatCallback beatCallback;
    
    // Update the position from the link manager
    void updatePositionFromLink();
    
    // Link callbacks
    void handleTempoChanged(double tempo);
    void handleStartStopChanged(bool isPlaying);
    void handleNumPeersChanged(std::size_t numPeers);
};