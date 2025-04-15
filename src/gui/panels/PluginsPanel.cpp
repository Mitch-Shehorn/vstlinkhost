#include "PluginsPanel.h"

//==============================================================================
// Button component for plugin controls in the active plugins list

class PluginControlsComponent : public juce::Component,
                               public juce::Button::Listener
{
public:
    PluginControlsComponent(PluginsPanel::ActivePluginsComponent& owner, int rowNumber)
        : ownerComponent(owner), row(rowNumber)
    {
        addAndMakeVisible(showButton);
        showButton.setButtonText("Open");
        showButton.addListener(this);
        
        addAndMakeVisible(removeButton);
        removeButton.setButtonText("×");
        removeButton.addListener(this);
    }
    
    void resized() override
    {
        auto area = getLocalBounds().reduced(2);
        removeButton.setBounds(area.removeFromRight(30));
        area.removeFromRight(2); // spacing
        showButton.setBounds(area);
    }
    
    void buttonClicked(juce::Button* button) override
    {
        if (button == &showButton)
        {
            // Tell the owner to show the plugin UI
            ownerComponent.showPluginWindow(row);
        }
        else if (button == &removeButton)
        {
            // Tell the owner to remove this plugin
            ownerComponent.removePlugin(row);
        }
    }
    
private:
    PluginsPanel::ActivePluginsComponent& ownerComponent;
    int row;
    juce::TextButton showButton;
    juce::TextButton removeButton;
};

//==============================================================================
// ActivePluginsComponent implementation

PluginsPanel::ActivePluginsComponent::ActivePluginsComponent(AudioEngine& audioEngineToUse, PluginManager& pluginManagerToUse)
    : audioEngine(audioEngineToUse), pluginManager(pluginManagerToUse)
{
    addAndMakeVisible(activePluginsTable);
    activePluginsTable.setModel(this);
    
    // Set up columns
    activePluginsTable.getHeader().addColumn("Name", NameColumn, 200);
    activePluginsTable.getHeader().addColumn("Format", FormatColumn, 80);
    activePluginsTable.getHeader().addColumn("Controls", ControlsColumn, 120);
    
    // Set column properties
    activePluginsTable.getHeader().setStretchToFitActive(true);
    
    // Clear button
    addAndMakeVisible(clearButton);
    clearButton.setButtonText("Clear All Plugins");
    clearButton.addListener(this);
}

PluginsPanel::ActivePluginsComponent::~ActivePluginsComponent()
{
}

void PluginsPanel::ActivePluginsComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void PluginsPanel::ActivePluginsComponent::resized()
{
    auto area = getLocalBounds().reduced(2);
    
    // Bottom controls
    auto bottomControls = area.removeFromBottom(40);
    clearButton.setBounds(bottomControls.removeFromLeft(150).reduced(2));
    
    // Active plugins table
    activePluginsTable.setBounds(area);
}

int PluginsPanel::ActivePluginsComponent::getNumRows()
{
    return audioEngine.getProcessorGraph().getNumNodes() - 2; // Subtract input/output nodes
}

void PluginsPanel::ActivePluginsComponent::paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
        g.fillAll(juce::Colours::lightblue);
    else if (rowNumber % 2)
        g.fillAll(juce::Colour(0xffeeeeee));
}

void PluginsPanel::ActivePluginsComponent::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
    g.setColour(rowIsSelected ? juce::Colours::darkblue : juce::Colours::black);
    
    auto& graph = audioEngine.getProcessorGraph();
    auto nodes = graph.getNodes();
    
    // Skip input/output nodes (first two nodes)
    int pluginIndex = rowNumber + 2;
    
    if (pluginIndex < nodes.size())
    {
        auto node = nodes[pluginIndex];
        auto* processor = node->getProcessor();
        
        if (processor != nullptr)
        {
            juce::String text;
            
            switch (columnId)
            {
                case NameColumn:
                    text = processor->getName();
                    break;
                    
                case FormatColumn:
                    if (auto* instance = dynamic_cast<juce::AudioPluginInstance*>(processor))
                        text = instance->getPluginDescription().pluginFormatName;
                    break;
                    
                default:
                    break;
            }
            
            g.drawText(text, 2, 0, width - 4, height, juce::Justification::centredLeft, true);
        }
    }
}

juce::Component* PluginsPanel::ActivePluginsComponent::refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, juce::Component* existingComponentToUpdate)
{
    if (columnId == ControlsColumn)
    {
        if (existingComponentToUpdate == nullptr)
            return new PluginControlsComponent(*this, rowNumber);
        
        return existingComponentToUpdate;
    }
    
    return nullptr;
}

void PluginsPanel::ActivePluginsComponent::buttonClicked(juce::Button* button)
{
    if (button == &clearButton)
    {
        // Ask for confirmation
        if (juce::AlertWindow::showOkCancelBox(juce::AlertWindow::QuestionIcon,
                                             "Clear Plugins",
                                             "Are you sure you want to remove all plugins?"))
        {
            audioEngine.clearPlugins();
            updateList();
        }
    }
}

void PluginsPanel::ActivePluginsComponent::updateList()
{
    activePluginsTable.updateContent();
    activePluginsTable.repaint();
}

void PluginsPanel::ActivePluginsComponent::showPluginWindow(int index)
{
    auto& graph = audioEngine.getProcessorGraph();
    auto nodes = graph.getNodes();
    
    // Skip input/output nodes (first two nodes)
    int pluginIndex = index + 2;
    
    if (pluginIndex < nodes.size())
    {
        auto node = nodes[pluginIndex];
        auto* processor = node->getProcessor();
        
        if (processor != nullptr)
        {
            if (auto* ui = processor->createEditorIfNeeded())
            {
                juce::DialogWindow::LaunchOptions options;
                
                options.content.setOwned(ui);
                options.dialogTitle = processor->getName();
                options.escapeKeyTriggersCloseButton = true;
                options.useNativeTitleBar = true;
                options.resizable = true;
                
                options.launchAsync();
            }
            else
            {
                juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon,
                                                     "Plugin UI",
                                                     "This plugin does not have a custom UI.");
            }
        }
    }
}

void PluginsPanel::ActivePluginsComponent::removePlugin(int index)
{
    auto& graph = audioEngine.getProcessorGraph();
    auto nodes = graph.getNodes();
    
    // Skip input/output nodes
    int pluginIndex = index + 2;
    
    if (pluginIndex < nodes.size())
    {
        auto node = nodes[pluginIndex];
        audioEngine.removePlugin(node->nodeID);
        updateList();
    }
}

//==============================================================================
// PluginsPanel implementation

PluginsPanel::PluginsPanel(PluginManager& pluginManagerToUse, AudioEngine& audioEngineToUse)
    : tabbedComponent(juce::TabbedButtonBar::TabsAtTop),
      pluginListComponent(pluginManagerToUse, audioEngineToUse),
      activePluginsComponent(audioEngineToUse, pluginManagerToUse)
{
    addAndMakeVisible(tabbedComponent);
    
    tabbedComponent.addTab("Available Plugins", juce::Colours::lightgrey, &pluginListComponent, false);
    tabbedComponent.addTab("Active Plugins", juce::Colours::lightgrey, &activePluginsComponent, false);
}

PluginsPanel::~PluginsPanel()
{
}

void PluginsPanel::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void PluginsPanel::resized()
{
    tabbedComponent.setBounds(getLocalBounds());
}

void PluginsPanel::buttonClicked(juce::Button* button)
{
    // Handle any panel-level buttons here
}