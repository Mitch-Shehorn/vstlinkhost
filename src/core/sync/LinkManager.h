#pragma once

#include <ableton/Link.hpp>
#include <functional>
#include <memory>
#include <atomic>
#include <mutex>
#include <thread>
#include <condition_variable>

class LinkManager
{
public:
    LinkManager();
    ~LinkManager();

    // Initialize Link with a default tempo
    void initialize(double defaultTempo = 120.0);

    // Enable/disable Link
    void enable(bool shouldEnable);

    // Check if Link is enabled
    bool isEnabled() const;

    // Get the number of connected peers
    std::size_t getNumPeers() const;

    // Get the current tempo from Link
    double getTempo() const;

    // Set the tempo and propagate to Link
    void setTempo(double bpm);

    // Get the current beat time
    double getBeatTime() const;
    
    // Get beat time at specific audio timestamp
    double getBeatTimeAtTimestamp(std::chrono::microseconds timestamp) const;
    
    // Get phase at specific audio timestamp (0-1 representing position in the current bar)
    double getPhaseAtTimestamp(std::chrono::microseconds timestamp, double quantum = 4.0) const;
    
    // Explicitly set the beat position
    void setBeatPosition(double position);
    
    // Start/stop session time progression
    void setIsPlaying(bool isPlaying);
    bool isPlaying() const;
    
    // Quantum (beats per bar) setting
    void setQuantum(double quantum);
    double getQuantum() const;
    
    // Register callbacks for various Link events
    using TempoCallback = std::function<void(double)>;
    using StartStopCallback = std::function<void(bool)>;
    using NumPeersCallback = std::function<void(std::size_t)>;
    
    void setTempoCallback(TempoCallback callback);
    void setStartStopCallback(StartStopCallback callback);
    void setNumPeersCallback(NumPeersCallback callback);
    
    // Force a callback trigger (useful when UI elements need to refresh)
    void forcePeerCountCallback();
    
private:
    std::unique_ptr<ableton::Link> link;
    std::atomic<bool> isLinkEnabled;
    std::atomic<bool> isLinkPlaying;
    std::atomic<double> quantum;
    
    // Callbacks
    TempoCallback tempoCallback;
    StartStopCallback startStopCallback;
    NumPeersCallback numPeersCallback;
    
    // Monitors for Link state changes
    std::unique_ptr<std::thread> monitorThread;
    std::atomic<bool> threadShouldExit;
    std::mutex monitorMutex;
    std::condition_variable monitorCondition;
    
    // Start the monitoring thread
    void startMonitoring();
    
    // Stop the monitoring thread
    void stopMonitoring();
    
    // Thread function for monitoring Link state
    void monitorLinkState();
};
