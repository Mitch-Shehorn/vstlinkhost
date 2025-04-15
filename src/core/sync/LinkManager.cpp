#include "LinkManager.h"
#include <chrono>
#include <iostream>

LinkManager::LinkManager()
    : isLinkEnabled(false), 
      isLinkPlaying(false), 
      quantum(4.0), 
      threadShouldExit(false)
{
}

LinkManager::~LinkManager()
{
    stopMonitoring();
    
    if (link)
        link->enable(false);
}

void LinkManager::initialize(double defaultTempo)
{
    // Create the Link instance with initial tempo
    link = std::make_unique<ableton::Link>(defaultTempo);
    
    // Set initial states
    isLinkEnabled = false;
    isLinkPlaying = false;
    quantum = 4.0;
    
    // Start monitoring for link state changes
    startMonitoring();
}

void LinkManager::enable(bool shouldEnable)
{
    if (!link)
        return;
        
    link->enable(shouldEnable);
    isLinkEnabled = shouldEnable;
    
    if (numPeersCallback)
        numPeersCallback(getNumPeers());
}

bool LinkManager::isEnabled() const
{
    return link && link->isEnabled();
}

std::size_t LinkManager::getNumPeers() const
{
    if (!link || !link->isEnabled())
        return 0;
        
    return link->numPeers();
}

double LinkManager::getTempo() const
{
    if (!link)
        return 120.0;
    
    auto timeline = link->captureAudioTimeline();
    return timeline.tempo();
}

void LinkManager::setTempo(double bpm)
{
    if (!link)
        return;
    
    auto timeline = link->captureAudioTimeline();
    timeline.setTempo(bpm, link->clock().micros());
    link->commitAudioTimeline(timeline);
    
    // Notify tempo callback
    if (tempoCallback)
        tempoCallback(bpm);
}

double LinkManager::getBeatTime() const
{
    if (!link)
        return 0.0;
    
    auto timeline = link->captureAudioTimeline();
    return timeline.beatAtTime(link->clock().micros(), quantum);
}

double LinkManager::getBeatTimeAtTimestamp(std::chrono::microseconds timestamp) const
{
    if (!link)
        return 0.0;
    
    auto timeline = link->captureAudioTimeline();
    return timeline.beatAtTime(timestamp, quantum);
}

double LinkManager::getPhaseAtTimestamp(std::chrono::microseconds timestamp, double quantumValue) const
{
    if (!link)
        return 0.0;
    
    auto timeline = link->captureAudioTimeline();
    auto beat = timeline.beatAtTime(timestamp, quantumValue);
    return beat - std::floor(beat / quantumValue) * quantumValue;
}

void LinkManager::setBeatPosition(double position)
{
    if (!link)
        return;
    
    auto timeline = link->captureAudioTimeline();
    auto time = link->clock().micros();
    timeline.requestBeatAtTime(position, time, quantum);
    link->commitAudioTimeline(timeline);
}

void LinkManager::setIsPlaying(bool shouldPlay)
{
    if (!link)
        return;
    
    if (isLinkPlaying != shouldPlay)
    {
        isLinkPlaying = shouldPlay;
        
        auto timeline = link->captureAudioTimeline();
        auto time = link->clock().micros();
        
        if (shouldPlay)
        {
            // Start playing from current beat position
            auto currentBeat = timeline.beatAtTime(time, quantum);
            timeline.setIsPlaying(true, time);
            timeline.requestBeatAtTime(currentBeat, time, quantum);
        }
        else
        {
            // Stop playing at current beat position
            timeline.setIsPlaying(false, time);
        }
        
        link->commitAudioTimeline(timeline);
        
        // Notify callback
        if (startStopCallback)
            startStopCallback(shouldPlay);
    }
}

bool LinkManager::isPlaying() const
{
    if (!link)
        return false;
    
    auto timeline = link->captureAudioTimeline();
    return timeline.isPlaying();
}

void LinkManager::setQuantum(double quantumValue)
{
    if (quantumValue > 0.0)
        quantum = quantumValue;
}

double LinkManager::getQuantum() const
{
    return quantum;
}

void LinkManager::setTempoCallback(TempoCallback callback)
{
    tempoCallback = std::move(callback);
}

void LinkManager::setStartStopCallback(StartStopCallback callback)
{
    startStopCallback = std::move(callback);
}

void LinkManager::setNumPeersCallback(NumPeersCallback callback)
{
    numPeersCallback = std::move(callback);
}

void LinkManager::forcePeerCountCallback()
{
    if (numPeersCallback)
        numPeersCallback(getNumPeers());
}

void LinkManager::startMonitoring()
{
    // Only start if we don't already have a running thread
    if (!monitorThread)
    {
        threadShouldExit = false;
        monitorThread = std::make_unique<std::thread>(&LinkManager::monitorLinkState, this);
    }
}

void LinkManager::stopMonitoring()
{
    if (monitorThread && monitorThread->joinable())
    {
        // Signal thread to exit
        threadShouldExit = true;
        
        // Wake up the thread if it's sleeping
        {
            std::lock_guard<std::mutex> lock(monitorMutex);
            monitorCondition.notify_one();
        }
        
        // Wait for the thread to finish
        monitorThread->join();
        monitorThread.reset();
    }
}

void LinkManager::monitorLinkState()
{
    if (!link)
        return;
    
    // Previous state values for change detection
    std::size_t prevPeers = 0;
    double prevTempo = getTempo();
    bool prevPlaying = isPlaying();
    
    while (!threadShouldExit)
    {
        // Check for changes
        
        // 1. Number of peers
        auto currentPeers = getNumPeers();
        if (currentPeers != prevPeers)
        {
            prevPeers = currentPeers;
            if (numPeersCallback)
                numPeersCallback(currentPeers);
        }
        
        // 2. Tempo changes
        auto currentTempo = getTempo();
        if (std::abs(currentTempo - prevTempo) > 0.01)
        {
            prevTempo = currentTempo;
            if (tempoCallback)
                tempoCallback(currentTempo);
        }
        
        // 3. Play state changes
        auto currentPlaying = isPlaying();
        if (currentPlaying != prevPlaying)
        {
            prevPlaying = currentPlaying;
            if (startStopCallback)
                startStopCallback(currentPlaying);
        }
        
        // Sleep for a short time
        std::unique_lock<std::mutex> lock(monitorMutex);
        monitorCondition.wait_for(lock, std::chrono::milliseconds(100), 
                                 [this] { return threadShouldExit.load(); });
    }
}