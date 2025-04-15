#include "TransportManager.h"

TransportManager::TransportManager(LinkManager& linkManagerToUse)
    : linkManager(linkManagerToUse), 
      playing(false), 
      meterNumerator(4), 
      meterDenominator(4), 
      beatPosition(0.0),
      phase(0.0)
{
    // Set Link's quantum to match our meter
    linkManager.setQuantum(meterNumerator);
    
    // Register callbacks for Link events
    linkManager.setTempoCallback([this](double tempo) { handleTempoChanged(tempo); });
    linkManager.setStartStopCallback([this](bool isPlaying) { handleStartStopChanged(isPlaying); });
    linkManager.setNumPeersCallback([this](std::size_t numPeers) { handleNumPeersChanged(numPeers); });
    
    // Start the timer that updates the position
    startTimer(16); // ~60fps update rate
}

TransportManager::~TransportManager()
{
    stopTimer();
}

void TransportManager::startPlayback()
{
    if (!playing)
    {
        playing = true;
        linkManager.setIsPlaying(true);
        
        // Not needed as the callback will be triggered via Link
        // if (transportCallback)
        //     transportCallback();
    }
}

void TransportManager::stopPlayback()
{
    if (playing)
    {
        playing = false;
        linkManager.setIsPlaying(false);
        
        // Not needed as the callback will be triggered via Link
        // if (transportCallback)
        //     transportCallback();
    }
}

void TransportManager::togglePlayback()
{
    if (playing)
        stopPlayback();
    else
        startPlayback();
}

bool TransportManager::isPlaying() const
{
    return linkManager.isPlaying();
}

void TransportManager::setTempo(double bpm)
{
    linkManager.setTempo(bpm);
}

double TransportManager::getTempo() const
{
    return linkManager.getTempo();
}

void TransportManager::setMeter(int numerator, int denominator)
{
    if (numerator > 0 && denominator > 0)
    {
        meterNumerator = numerator;
        meterDenominator = denominator;
        
        // Update Link's quantum to match our meter
        linkManager.setQuantum(meterNumerator);
    }
}

void TransportManager::getMeter(int& numerator, int& denominator) const
{
    numerator = meterNumerator;
    denominator = meterDenominator;
}

double TransportManager::getBeatPosition() const
{
    return beatPosition;
}

void TransportManager::setBeatPosition(double newBeatPosition)
{
    beatPosition = newBeatPosition;
    linkManager.setBeatPosition(newBeatPosition);
    
    // Update phase
    phase = newBeatPosition - std::floor(newBeatPosition / meterNumerator) * meterNumerator;
    
    // Notify any listeners
    if (beatCallback)
        beatCallback(newBeatPosition);
    
    if (transportCallback)
        transportCallback();
}

double TransportManager::getPhase() const
{
    return phase / meterNumerator;
}

juce::String TransportManager::getPositionDisplay() const
{
    // Calculate bars and beats from the beat position
    int bar = getBarNumber();
    int beat = getBeatNumber();
    
    // Format as bar.beat.tick (with 4 ticks per beat)
    int tick = static_cast<int>((beatPosition - std::floor(beatPosition)) * 4);
    
    return juce::String(bar) + "." + juce::String(beat) + "." + juce::String(tick);
}

int TransportManager::getBarNumber() const
{
    return static_cast<int>(beatPosition) / meterNumerator + 1; // 1-based bar number
}

int TransportManager::getBeatNumber() const
{
    return static_cast<int>(beatPosition) % meterNumerator + 1; // 1-based beat number
}

void TransportManager::setTransportCallback(TransportCallback callback)
{
    transportCallback = std::move(callback);
}

void TransportManager::setTempoCallback(TempoCallback callback)
{
    tempoCallback = std::move(callback);
}

void TransportManager::setPeersCallback(PeersCallback callback)
{
    peersCallback = std::move(callback);
}

void TransportManager::setBeatCallback(BeatCallback callback)
{
    beatCallback = std::move(callback);
}

int TransportManager::getNumPeers() const
{
    return static_cast<int>(linkManager.getNumPeers());
}

void TransportManager::timerCallback()
{
    updatePositionFromLink();
}

void TransportManager::updatePositionFromLink()
{
    // Always update position, even when not playing (for seeking)
    auto newBeatPosition = linkManager.getBeatTime();
    
    // Only update if the position has changed
    if (std::abs(newBeatPosition - beatPosition) > 0.001)
    {
        beatPosition = newBeatPosition;
        
        // Update phase
        phase = beatPosition - std::floor(beatPosition / meterNumerator) * meterNumerator;
        
        // Notify beat callback
        if (beatCallback)
            beatCallback(beatPosition);
    }
}

void TransportManager::handleTempoChanged(double tempo)
{
    // Update internal state (not needed since we directly query Link)
    
    // Notify tempo callback
    if (tempoCallback)
        tempoCallback(tempo);
}

void TransportManager::handleStartStopChanged(bool isPlaying)
{
    // Update internal state
    playing = isPlaying;
    
    // Notify transport callback
    if (transportCallback)
        transportCallback();
}

void TransportManager::handleNumPeersChanged(std::size_t numPeers)
{
    // Notify peers callback
    if (peersCallback)
        peersCallback(static_cast<int>(numPeers));
}