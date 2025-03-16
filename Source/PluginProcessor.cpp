/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PocketsynthAudioProcessor::PocketsynthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
    undoManager(),
    treeState(*this, &undoManager, "PARAMETERS", createParameterLayout())
#endif
{
    // Set up preset directory
	presetDirectory = licenseManager.getActivationDirectory().getChildFile("Presets");
	presetDirectory.createDirectory();

    // Add listeners
	licenseManager.addListener(this);

	// Set up ValueTreeState
    treeState.state = juce::ValueTree(licenseManager.getPluginID() + "State");

    // Set up the synth
    for (auto i = 0; i < 4; ++i)
    {
        synth.addVoice(new SineWaveVoice());
    }
    synth.addSound(new SineWaveSound());
}

PocketsynthAudioProcessor::~PocketsynthAudioProcessor()
{
	licenseManager.removeListener(this);
}

// Set up the ValueTreeState with default values
juce::AudioProcessorValueTreeState::ParameterLayout PocketsynthAudioProcessor::createParameterLayout()
{
	juce::AudioProcessorValueTreeState::ParameterLayout layout;

	layout.add(std::make_unique<juce::AudioParameterFloat>("gain", "Gain", juce::NormalisableRange<float>(0.0f, 1.0f), initialGain));

	return layout;
}

// Listen for changes on the ValueTreeState
void PocketsynthAudioProcessor::valueTreePropertyChanged(juce::ValueTree& tree, const juce::Identifier& property)
{
    juce::Logger::outputDebugString("PROCESSOR: ValueTree property changed: " + property.toString() + ", new value: " + tree.getProperty("gain"));
}

void PocketsynthAudioProcessor::savePreset()
{
    // Check if the plugin is activated
    if (!licenseManager.isActivated())
    {
		juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon, "Error", "Plugin is not activated.");
        return;
    }

    // Create a file chooser using heap-allocated shared_ptr so it stays alive throughout the async operation
    auto chooser = std::make_shared<juce::FileChooser>(
        "Save Preset",
        presetDirectory,
        "*.bdp");

    chooser->launchAsync(juce::FileBrowserComponent::saveMode,
        [this, chooser](const juce::FileChooser& fc) // Capture chooser by value
        {
            juce::File presetFile = fc.getResult(); // Get selected file
            if (presetFile != juce::File{}) // Check if the user selected a file
            {
                juce::ValueTree state = treeState.copyState(); // Get a copy of the state
                std::unique_ptr<juce::XmlElement> xml = state.createXml(); // Convert to XML

                if (xml)
                    xml->writeTo(presetFile); // Save XML to file

				// Update change listeners (editor to update list in combobox) after saving the preset has finished
				juce::MessageManager::callAsync([this] { sendChangeMessage(); });
            }
        }
    );
}

bool PocketsynthAudioProcessor::loadPreset(juce::File newPresetFile)
{
    // Check if the plugin is activated
    if (!licenseManager.isActivated())
    {
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon, "Error", "Plugin is not activated.");
        return false;
    }

	juce::Logger::outputDebugString("PROCESSOR: loading preset file: " + newPresetFile.getFullPathName());

    if (!newPresetFile.existsAsFile())
    {
        juce::Logger::outputDebugString("PROCESSOR: preset file does not exist.");
		juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon, "Error", "Preset file does not exist.");
        return false;
    }

    std::unique_ptr<juce::XmlElement> xml(juce::XmlDocument::parse(newPresetFile));

	if (xml == nullptr || !xml->hasTagName(licenseManager.getPluginID() + "State"))
	{
		juce::Logger::outputDebugString("PROCESSOR: preset file is not valid.");
		juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon, "Error", "Preset file is not valid.");
		return false;
	}

    juce::ValueTree newState = juce::ValueTree::fromXml(*xml);

	juce::Logger::outputDebugString("PROCESSOR: loading preset file: " + newPresetFile.getFullPathName());

	// Check every parameter in the new state and make sure it is within the parameter's range
    for (int i = 0; i < newState.getNumChildren(); i++)
    {
        juce::ValueTree paramNode = newState.getChild(i);
        juce::String paramID = paramNode.getProperty("id").toString();
        float value = paramNode.getProperty("value");

		juce::Logger::outputDebugString("PROCESSOR: checking parameter: " + paramID);

        if (auto* param = treeState.getParameter(paramID))
        {
			if (value < param->getNormalisableRange().start || value > param->getNormalisableRange().end)
			{
				juce::Logger::outputDebugString("PROCESSOR: preset file contains invalid value for parameter: " + paramID);
				juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon, "Error", "Preset contained invalid values.");
				return false;
			}
        }
    }

	treeState.replaceState(newState);
    return true;
}

void PocketsynthAudioProcessor::loadDefaultPreset()
{
	for (int i = 0; i < treeState.state.getNumChildren(); i++)
	{
		juce::ValueTree paramNode = treeState.state.getChild(i);
		juce::String paramID = paramNode.getProperty("id").toString();
		float defaultValue = treeState.getParameter(paramID)->getDefaultValue();

		paramNode.setProperty("value", defaultValue, nullptr);
	}
}

// Listener for LicenseManager
void PocketsynthAudioProcessor::onLicenseActivated()
{
    // Notify listeners (plugin editor) that the license has been activated
	juce::Logger::outputDebugString("PROCESSOR: license activation event received.");
}

void PocketsynthAudioProcessor::onLicenseDeactivated()
{
	// Notify listeners (plugin editor) that the license has been deactivated
	juce::Logger::outputDebugString("PROCESSOR: license deactivation event received.");
}

//==============================================================================
const juce::String PocketsynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PocketsynthAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PocketsynthAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PocketsynthAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PocketsynthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PocketsynthAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PocketsynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PocketsynthAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String PocketsynthAudioProcessor::getProgramName (int index)
{
    return {};
}

void PocketsynthAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void PocketsynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
	synth.setCurrentPlaybackSampleRate(sampleRate);
}

void PocketsynthAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PocketsynthAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void PocketsynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

	midiKeyboardState.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);
	synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }
}

//==============================================================================
bool PocketsynthAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PocketsynthAudioProcessor::createEditor()
{
    return new PocketsynthAudioProcessorEditor (*this);
}

//==============================================================================
void PocketsynthAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    // Restore settings when a user opens a project
	std::unique_ptr<juce::XmlElement> xml(treeState.state.createXml());
    copyXmlToBinary(*xml, destData);		
}

void PocketsynthAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    // Save settings when a user closes a project
	std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));

    if (xml)
    {
        treeState.replaceState(juce::ValueTree::fromXml(*xml));
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PocketsynthAudioProcessor();
}
