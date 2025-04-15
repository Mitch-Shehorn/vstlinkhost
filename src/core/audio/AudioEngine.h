#pragma once

#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <memory>
#include <vector>
#include <functional>

class AudioEngine : public juce::AudioIODeviceCallback
{
public:
    AudioEngine();
    ~AudioEngine();

    // Start/stop audio processing
    bool start();
    void stop();

    // Get the current sample rate
    double getSampleRate() const;

    // Get the current buffer size
    int getBufferSize() const;

    // AudioIODeviceCallback implementation
    void audioDeviceIOCallback(const float** inputChannelData,
                              int numInputChannels,
                              float** outputChannelData,
                              int numOutputChannels,
                              int numSamples);

    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
    void audioDeviceStopped() override;

    // Audio device management
    juce::StringArray getAvailableAudioDevices() const;
    bool setAudioDevice(int deviceIndex, int inputChannels, int outputChannels);

    // Plugin management in the audio graph
    using NodeID = juce::AudioProcessorGraph::NodeID;
    
    // Add a plugin processor to the graph
    NodeID addPluginProcessor(std::unique_ptr<juce::AudioPluginInstance> processor);
    
    // Connect nodes in the graph
    bool connectNodes(NodeID sourceNodeID, int sourceChannelIndex, 
                     NodeID destinationNodeID, int destinationChannelIndex);
                     
    // Remove a plugin from the graph
    bool removePlugin(NodeID nodeID);
    
    // Get access to the processor graph
    juce::AudioProcessorGraph& getProcessorGraph();
    
    // Clear all plugins from the graph
    void clearPlugins();
    
    // Get input/output node IDs
    NodeID getInputNode() const { return audioInputNodeID; }
    NodeID getOutputNode() const { return audioOutputNodeID; }
    
    // Get the audio device manager
    juce::AudioDeviceManager& getDeviceManager() { return *deviceManager; }

private:
    std::unique_ptr<juce::AudioDeviceManager> deviceManager;
    juce::AudioProcessorGraph processorGraph;
    double sampleRate;
    int bufferSize;
    bool isRunning;
    
    // Store node IDs for input and output nodes
    NodeID audioInputNodeID;
    NodeID audioOutputNodeID;
    
    // Callback for tempo changes
    std::function<void(double)> tempoChangeCallback;
};
