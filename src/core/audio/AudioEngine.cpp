#include "AudioEngine.h"

AudioEngine::AudioEngine()
    : sampleRate(44100.0), bufferSize(512), isRunning(false)
{
    deviceManager = std::make_unique<juce::AudioDeviceManager>();
    deviceManager->initialiseWithDefaultDevices(2, 2);
    processorGraph.setPlayConfigDetails(2, 2, sampleRate, bufferSize);
    
    // Add input/output nodes to the graph
    auto audioInputNode = processorGraph.addNode(std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
        juce::AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode));
    
    auto audioOutputNode = processorGraph.addNode(std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
        juce::AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode));
    
    if (audioInputNode && audioOutputNode)
    {
        audioInputNodeID = audioInputNode->nodeID;
        audioOutputNodeID = audioOutputNode->nodeID;
    }
}

AudioEngine::~AudioEngine()
{
    stop();
}

bool AudioEngine::start()
{
    if (isRunning)
        return true;
    
    auto setup = deviceManager->getAudioDeviceSetup();
    setup.sampleRate = sampleRate;
    setup.bufferSize = bufferSize;
    
    auto error = deviceManager->setAudioDeviceSetup(setup, true);
    if (error.isNotEmpty())
        return false;
    
    deviceManager->addAudioCallback(this);
    isRunning = true;
    return true;
}

void AudioEngine::stop()
{
    if (!isRunning)
        return;
    
    deviceManager->removeAudioCallback(this);
    isRunning = false;
}

double AudioEngine::getSampleRate() const
{
    return sampleRate;
}

int AudioEngine::getBufferSize() const
{
    return bufferSize;
}

juce::StringArray AudioEngine::getAvailableAudioDevices() const
{
    juce::StringArray devices;
    
    auto& audioDeviceTypes = deviceManager->getAvailableDeviceTypes();
    for (auto& deviceType : audioDeviceTypes)
    {
        auto deviceNames = deviceType->getDeviceNames();
        for (auto& deviceName : deviceNames)
        {
            devices.add(deviceName);
        }
    }
    
    return devices;
}

bool AudioEngine::setAudioDevice(int deviceIndex, int inputChannels, int outputChannels)
{
    auto devices = getAvailableAudioDevices();
    if (deviceIndex < 0 || deviceIndex >= devices.size())
        return false;
    
    // Stop audio processing
    bool wasRunning = isRunning;
    if (wasRunning)
        stop();
    
    // Find the device type that contains this device
    bool deviceFound = false;
    juce::String deviceName = devices[deviceIndex];
    
    auto& audioDeviceTypes = deviceManager->getAvailableDeviceTypes();
    for (auto& deviceType : audioDeviceTypes)
    {
        auto deviceNames = deviceType->getDeviceNames();
        if (deviceNames.contains(deviceName))
        {
            deviceManager->setCurrentAudioDeviceType(deviceType->getTypeName(), true);
            
            juce::AudioDeviceManager::AudioDeviceSetup setup;
            setup.inputDeviceName = deviceName;
            setup.outputDeviceName = deviceName;
            setup.sampleRate = sampleRate;
            setup.bufferSize = bufferSize;
            setup.inputChannels.setRange(0, inputChannels, true);
            setup.outputChannels.setRange(0, outputChannels, true);
            
            auto error = deviceManager->setAudioDeviceSetup(setup, true);
            deviceFound = error.isEmpty();
            break;
        }
    }
    
    // Restart if it was running
    if (wasRunning && deviceFound)
        start();
    
    return deviceFound;
}

AudioEngine::NodeID AudioEngine::addPluginProcessor(std::unique_ptr<juce::AudioPluginInstance> processor)
{
    if (!processor)
        return {};
    
    // Configure the processor with current settings
    processor->setPlayConfigDetails(2, 2, sampleRate, bufferSize);
    processor->prepareToPlay(sampleRate, bufferSize);
    
    // Add it to the graph
    auto node = processorGraph.addNode(std::move(processor));
    return node ? node->nodeID : juce::AudioProcessorGraph::NodeID();
}

bool AudioEngine::connectNodes(NodeID sourceNodeID, int sourceChannelIndex, 
                              NodeID destinationNodeID, int destinationChannelIndex)
{
    // Make a connection
    auto result = processorGraph.addConnection({ 
        { sourceNodeID, sourceChannelIndex }, 
        { destinationNodeID, destinationChannelIndex } 
    });
    
    return result;
}

bool AudioEngine::removePlugin(NodeID nodeID)
{
    return processorGraph.removeNode(nodeID);
}

juce::AudioProcessorGraph& AudioEngine::getProcessorGraph()
{
    return processorGraph;
}

void AudioEngine::clearPlugins()
{
    processorGraph.clear();
    
    // Re-create input/output nodes
    auto audioInputNode = processorGraph.addNode(std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
        juce::AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode));
    
    auto audioOutputNode = processorGraph.addNode(std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
        juce::AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode));
    
    if (audioInputNode && audioOutputNode)
    {
        audioInputNodeID = audioInputNode->nodeID;
        audioOutputNodeID = audioOutputNode->nodeID;
    }
}

void AudioEngine::audioDeviceIOCallback(const float** inputChannelData,
                                       int numInputChannels,
                                       float** outputChannelData,
                                       int numOutputChannels,
                                       int numSamples)
{
    // Clear output buffers
    for (int channel = 0; channel < numOutputChannels; ++channel)
        if (outputChannelData[channel])
            juce::FloatVectorOperations::clear(outputChannelData[channel], numSamples);
    
    // Create audio buffer
    juce::AudioBuffer<float> buffer(outputChannelData, numOutputChannels, numSamples);
    
    // Copy input to buffer (if available)
    if (numInputChannels > 0)
    {
        for (int channel = 0; channel < numInputChannels && channel < numOutputChannels; ++channel)
            if (inputChannelData[channel] != nullptr && outputChannelData[channel] != nullptr)
                juce::FloatVectorOperations::copy(outputChannelData[channel], 
                                                 inputChannelData[channel], 
                                                 numSamples);
    }
    
    // Create MIDI buffer
    juce::MidiBuffer midiBuffer;
    
    // Process audio through the graph
    // Older versions of JUCE used a different API for processing
    processorGraph.processBlock(buffer, midiBuffer);
}

void AudioEngine::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
    sampleRate = device->getCurrentSampleRate();
    bufferSize = device->getCurrentBufferSizeSamples();
    
    processorGraph.setPlayConfigDetails(2, 2, sampleRate, bufferSize);
    processorGraph.prepareToPlay(sampleRate, bufferSize);
}

void AudioEngine::audioDeviceStopped()
{
    processorGraph.releaseResources();
}