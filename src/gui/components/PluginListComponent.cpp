#include "PluginListComponent.h"

class LoadPluginButtonComponent : public juce::Component,
                                 public juce::Button::Listener
{
public:
    LoadPluginButtonComponent(PluginListComponent& owner, int rowNumber)
        : ownerComponent(owner), row(rowNumber)
    {
        addAndMakeVisible(loadButton);
        loadButton.setButtonText("Load");
        loadButton.addListener(this);
    }
    
    void resized() override
    {
        loadButton.setBounds(getLocalBounds().reduced(2));
    }
    
    void buttonClicked(juce::Button* button) override
    {
        if (button == &loadButton)
            ownerComponent.loadPlugin(row);
    }
    
private:
    PluginListComponent& ownerComponent;
    int row;
    juce::TextButton loadButton;
};

//==============================================================================

PluginListComponent::PluginListComponent(PluginManager& pluginManagerToUse, AudioEngine& audioEngineToUse)
    : pluginManager(pluginManagerToUse), 
      audioEngine(audioEngineToUse),
      currentScanProgress(0.0f)
{
    // Set up the plugin table
    addAndMakeVisible(pluginTable);
    pluginTable.setModel(this);
    
    // Add columns
    pluginTable.getHeader().addColumn("Name", NameColumn, 200);
    pluginTable.getHeader().addColumn("Category", CategoryColumn, 100);
    pluginTable.getHeader().addColumn("Manufacturer", ManufacturerColumn, 150);
    pluginTable.getHeader().addColumn("Format", FormatColumn, 80);
    pluginTable.getHeader().addColumn("Load", LoadColumn, 80);
    
    // Set column properties
    pluginTable.getHeader().setStretchToFitActive(true);
    
    // Buttons
    addAndMakeVisible(scanButton);
    scanButton.setButtonText("Scan for Plugins");
    scanButton.addListener(this);
    
    addAndMakeVisible(addPluginButton);
    addPluginButton.setButtonText("Add Plugin...");
    addPluginButton.addListener(this);
    
    // Progress label
    addAndMakeVisible(scanProgressLabel);
    scanProgressLabel.setJustificationType(juce::Justification::centred);
    scanProgressLabel.setText("", juce::dontSendNotification);
    
    // Set up scan progress callback
    pluginManager.setScanProgressCallback([this](float progress)
    {
        currentScanProgress = progress;
        
        // Use message thread for UI updates
        juce::MessageManager::callAsync([this]()
        {
            handleScanProgress(currentScanProgress);
        });
    });
    
    // Load any previously saved plugin list
    loadPluginList();
}

PluginListComponent::~PluginListComponent()
{
    // Save plugin list on exit
    savePluginList();
}

void PluginListComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void PluginListComponent::resized()
{
    auto area = getLocalBounds().reduced(2);
    
    // Bottom controls
    auto bottomControls = area.removeFromBottom(40);
    
    // Place buttons
    scanButton.setBounds(bottomControls.removeFromLeft(150).reduced(2));
    addPluginButton.setBounds(bottomControls.removeFromLeft(150).reduced(2));
    scanProgressLabel.setBounds(bottomControls.reduced(2));
    
    // Plugin table takes the rest of the space
    pluginTable.setBounds(area);
}

int PluginListComponent::getNumRows()
{
    return pluginManager.getKnownPluginList().getNumTypes();
}

void PluginListComponent::paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
        g.fillAll(juce::Colours::lightblue);
    else if (rowNumber % 2)
        g.fillAll(juce::Colour(0xffeeeeee));
}

void PluginListComponent::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
    g.setColour(rowIsSelected ? juce::Colours::darkblue : juce::Colours::black);
    
    if (rowNumber < pluginManager.getKnownPluginList().getNumTypes())
    {
        auto& desc = pluginManager.getKnownPluginList().getTypes()[rowNumber];
        
        juce::String text;
        
        switch (columnId)
        {
            case NameColumn:
                text = desc.name;
                break;
                
            case CategoryColumn:
                text = desc.category;
                break;
                
            case ManufacturerColumn:
                text = desc.manufacturerName;
                break;
                
            case FormatColumn:
                text = desc.pluginFormatName;
                break;
                
            default:
                break;
        }
        
        g.drawText(text, 2, 0, width - 4, height, juce::Justification::centredLeft, true);
    }
}

juce::Component* PluginListComponent::refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, juce::Component* existingComponentToUpdate)
{
    if (columnId == LoadColumn)
    {
        if (existingComponentToUpdate == nullptr)
            return new LoadPluginButtonComponent(*this, rowNumber);
        
        return existingComponentToUpdate;
    }
    
    return nullptr;
}

void PluginListComponent::buttonClicked(juce::Button* button)
{
    if (button == &scanButton)
    {
        scanForPlugins();
    }
    else if (button == &addPluginButton)
    {
        juce::FileChooser chooser("Select a plugin file", 
                                 juce::File::getSpecialLocation(juce::File::userHomeDirectory), 
                                 pluginManager.getFormatManager().getWildcardForAllFormats());
        
        if (chooser.browseForFileToOpen())
        {
            auto file = chooser.getResult();
            loadPluginFromFile(file);
        }
    }
}

bool PluginListComponent::isInterestedInFileDrag(const juce::StringArray& files)
{
    for (auto& file : files)
    {
        if (pluginManager.getFormatManager().doesPluginStillExist(
            juce::PluginDescription::createFromOSFileReference(juce::File(file), {})))
            return true;
    }
    
    return false;
}

void PluginListComponent::filesDropped(const juce::StringArray& files, int x, int y)
{
    for (auto& file : files)
    {
        loadPluginFromFile(juce::File(file));
    }
}

void PluginListComponent::scanForPlugins()
{
    // Reset progress
    currentScanProgress = 0.0f;
    scanProgressLabel.setText("Scanning for plugins...", juce::dontSendNotification);
    
    // Disable buttons during scan
    scanButton.setEnabled(false);
    addPluginButton.setEnabled(false);
    
    // Start scan in background thread
    std::thread([this]
    {
        scanStandardLocations();
        
        // Re-enable buttons and update table on the message thread
        juce::MessageManager::callAsync([this]
        {
            scanButton.setEnabled(true);
            addPluginButton.setEnabled(true);
            scanProgressLabel.setText("Scan complete", juce::dontSendNotification);
            updateTable();
            savePluginList();
        });
    }).detach();
}

void PluginListComponent::scanForPluginsInDirectory(const juce::File& directory)
{
    if (!directory.isDirectory())
        return;
    
    // Reset progress
    currentScanProgress = 0.0f;
    
    // Update UI on message thread
    juce::MessageManager::callAsync([this, directoryPath = directory.getFullPathName()]
    {
        scanProgressLabel.setText("Scanning " + directoryPath + "...", juce::dontSendNotification);
        scanButton.setEnabled(false);
        addPluginButton.setEnabled(false);
    });
    
    // Start scan in background thread
    std::thread([this, directory]
    {
        pluginManager.scanForPlugins(directory.getFullPathName());
        
        // Re-enable buttons and update table on the message thread
        juce::MessageManager::callAsync([this]
        {
            scanButton.setEnabled(true);
            addPluginButton.setEnabled(true);
            scanProgressLabel.setText("Scan complete", juce::dontSendNotification);
            updateTable();
            savePluginList();
        });
    }).detach();
}

bool PluginListComponent::loadPlugin(int index)
{
    if (index < 0 || index >= pluginManager.getKnownPluginList().getNumTypes())
        return false;
    
    auto& desc = pluginManager.getKnownPluginList().getTypes()[index];
    
    // Show a waiting cursor
    juce::MouseCursor::showWaitCursor();
    
    // Create plugin instance
    juce::String errorMessage;
    auto pluginInstance = pluginManager.createPluginInstance(
        desc, audioEngine.getSampleRate(), audioEngine.getBufferSize(), errorMessage);
    
    if (pluginInstance != nullptr)
    {
        // Add the plugin to the audio engine's processor graph
        auto nodeID = audioEngine.addPluginProcessor(std::move(pluginInstance));
        
        // Connect to input and output nodes if successful
        if (nodeID.isValid())
        {
            // Connect to input (assuming stereo)
            audioEngine.connectNodes(audioEngine.getInputNode(), 0, nodeID, 0);
            audioEngine.connectNodes(audioEngine.getInputNode(), 1, nodeID, 1);
            
            // Connect to output (assuming stereo)
            audioEngine.connectNodes(nodeID, 0, audioEngine.getOutputNode(), 0);
            audioEngine.connectNodes(nodeID, 1, audioEngine.getOutputNode(), 1);
            
            juce::MouseCursor::hideWaitCursor();
            return true;
        }
    }
    
    juce::MouseCursor::hideWaitCursor();
    
    if (errorMessage.isNotEmpty())
    {
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
                                             "Plugin Error",
                                             "Failed to load plugin: " + errorMessage);
    }
    
    return false;
}

bool PluginListComponent::loadPluginFromFile(const juce::File& file)
{
    if (!file.exists())
        return false;
    
    // Show a waiting cursor
    juce::MouseCursor::showWaitCursor();
    
    auto result = pluginManager.loadPlugin(file.getFullPathName());
    
    juce::MouseCursor::hideWaitCursor();
    
    if (!result)
    {
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
                                             "Plugin Error",
                                             "Failed to load plugin: " + file.getFullPathName());
    }
    
    return result;
}

void PluginListComponent::updateTable()
{
    pluginTable.updateContent();
    pluginTable.repaint();
}

void PluginListComponent::handleScanProgress(float progress)
{
    int percent = static_cast<int>(progress * 100.0f);
    scanProgressLabel.setText("Scanning: " + juce::String(percent) + "%", juce::dontSendNotification);
}

void PluginListComponent::showPluginSettings()
{
    juce::DialogWindow::LaunchOptions options;
    
    auto* content = new juce::PropertyPanel();
    
    // TODO: Add properties for plugin search paths, etc.
    
    options.content.setOwned(content);
    options.dialogTitle = "Plugin Settings";
    options.dialogBackgroundColour = juce::Colours::lightgrey;
    options.escapeKeyTriggersCloseButton = true;
    options.useNativeTitleBar = true;
    options.resizable = true;
    
    options.launchAsync();
}

void PluginListComponent::showPluginMenu(int rowNumber, int x, int y)
{
    juce::PopupMenu menu;
    
    menu.addItem(1, "Load Plugin");
    menu.addItem(2, "Show Plugin Info");
    
    menu.showMenuAsync(juce::PopupMenu::Options()
                       .withTargetComponent(pluginTable)
                       .withTargetScreenArea(juce::Rectangle<int>(x, y, 1, 1)),
                       [this, rowNumber](int result)
    {
        if (result == 1)
        {
            loadPlugin(rowNumber);
        }
        else if (result == 2)
        {
            // Show plugin info
            if (rowNumber < pluginManager.getKnownPluginList().getNumTypes())
            {
                auto& desc = pluginManager.getKnownPluginList().getTypes()[rowNumber];
                
                juce::String info;
                info << "Name: " << desc.name << "\n"
                     << "Format: " << desc.pluginFormatName << "\n"
                     << "Category: " << desc.category << "\n"
                     << "Manufacturer: " << desc.manufacturerName << "\n"
                     << "Version: " << desc.version << "\n"
                     << "File: " << desc.fileOrIdentifier;
                
                juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon,
                                                     "Plugin Information",
                                                     info);
            }
        }
    });
}

void PluginListComponent::removePlugin(int index)
{
    // TODO: Implement plugin removal from the audio engine
}

void PluginListComponent::scanStandardLocations()
{
    // Common plugin locations
    juce::StringArray directories;
    
    // Windows VST locations
    directories.add("C:\\Program Files\\VSTPlugins");
    directories.add("C:\\Program Files\\Common Files\\VST3");
    directories.add("C:\\Program Files\\Common Files\\VST2");
    directories.add("C:\\Program Files\\Steinberg\\VSTPlugins");
    
    // macOS VST locations
    directories.add("/Library/Audio/Plug-Ins/VST");
    directories.add("/Library/Audio/Plug-Ins/VST3");
    directories.add("/Library/Audio/Plug-Ins/Components");  // AU plugins
    
    // Linux VST locations
    directories.add("/usr/lib/vst");
    directories.add("/usr/lib/vst3");
    directories.add("/usr/local/lib/vst");
    directories.add("/usr/local/lib/vst3");
    
    // User home VST locations
    auto homeDir = juce::File::getSpecialLocation(juce::File::userHomeDirectory);
    
    // Windows user VST locations
    directories.add(homeDir.getChildFile("VSTPlugins").getFullPathName());
    
    // macOS user VST locations
    directories.add(homeDir.getChildFile("Library/Audio/Plug-Ins/VST").getFullPathName());
    directories.add(homeDir.getChildFile("Library/Audio/Plug-Ins/VST3").getFullPathName());
    directories.add(homeDir.getChildFile("Library/Audio/Plug-Ins/Components").getFullPathName());
    
    // Linux user VST locations
    directories.add(homeDir.getChildFile(".vst").getFullPathName());
    directories.add(homeDir.getChildFile(".vst3").getFullPathName());
    
    // Scan each directory
    for (auto& dir : directories)
    {
        juce::File dirFile(dir);
        if (dirFile.isDirectory())
        {
            pluginManager.scanForPlugins(dir);
        }
    }
}

juce::File PluginListComponent::getDefaultPluginSettingsFile()
{
    auto appDataDir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                        .getChildFile("VSTLinkHost");
    
    if (!appDataDir.exists())
        appDataDir.createDirectory();
    
    return appDataDir.getChildFile("PluginList.xml");
}

void PluginListComponent::savePluginList()
{
    auto file = getDefaultPluginSettingsFile();
    pluginManager.savePluginList(file);
}

void PluginListComponent::loadPluginList()
{
    auto file = getDefaultPluginSettingsFile();
    if (file.exists())
    {
        pluginManager.loadPluginList(file);
        updateTable();
    }
}