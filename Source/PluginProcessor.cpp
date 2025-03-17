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
    treeState(*this, &undoManager, juce::Identifier (licenseManager.getPluginID()), createParameterLayout())
#endif
{
    // Set up preset directory
	presetDirectory = licenseManager.getActivationDirectory().getChildFile("Presets");
	presetDirectory.createDirectory();

    // Add listeners
	licenseManager.addListener(this);

    // Add listeners to all parameters
    for (auto p : getParameters())
    {
        if (auto* paramWithID = dynamic_cast<juce::AudioProcessorParameterWithID*>(p))
        {
			treeState.addParameterListener(paramWithID->paramID, this);
        }
    }

    setupSynth();
}

PocketsynthAudioProcessor::~PocketsynthAudioProcessor()
{
	// Remove listeners
	licenseManager.removeListener(this);

    for (auto p : getParameters())
    {
        if (auto* paramWithID = dynamic_cast<juce::AudioProcessorParameterWithID*>(p))
        {
            treeState.removeParameterListener(paramWithID->paramID, this);
        }
    }
}

void PocketsynthAudioProcessor::setupSynth()
{
	int voices = static_cast<int>(*treeState.getRawParameterValue("voices"));

	//synth.clearVoices();

	for (int i = 0; i < voices; i++)
		synth.addVoice(new OscillatorVoice());

    if (synth.getNumSounds() == 0)
    	synth.addSound(new OscillatorSound());
}

// Set up the ValueTreeState with default values
juce::AudioProcessorValueTreeState::ParameterLayout PocketsynthAudioProcessor::createParameterLayout()
{
	juce::AudioProcessorValueTreeState::ParameterLayout layout;

	// Global parameters
	layout.add(std::make_unique<juce::AudioParameterFloat>("gain", "Gain", juce::NormalisableRange<float>(0.0f, 1.0f), initialGain));
	layout.add(std::make_unique<juce::AudioParameterInt>("voices", "Voices", 1, 16, 4));

    // Oscillator 1 parameters
	layout.add(std::make_unique<juce::AudioParameterBool>("osc1_active", "Osc 1 Active", true));
	layout.add(std::make_unique<juce::AudioParameterChoice>("osc1_waveform", "Osc 1 Waveform", juce::StringArray{ "Sine", "Square", "Saw", "Triangle", "Noise"}, 0));
	layout.add(std::make_unique<juce::AudioParameterInt>("osc1_octave", "Osc 1 Octave", -3, 3, 0));
	layout.add(std::make_unique<juce::AudioParameterInt>("osc1_semitone", "Osc 1 Semitone", -11, 11, 0));
	layout.add(std::make_unique<juce::AudioParameterInt>("osc1_fine", "Osc 1 Fine", -100, 100, 0));
	layout.add(std::make_unique<juce::AudioParameterFloat>("osc1_attack", "Osc 1 Attack", juce::NormalisableRange<float>(0.0f, 1.0f), 0.1f));
	layout.add(std::make_unique<juce::AudioParameterFloat>("osc1_decay", "Osc 1 Decay", juce::NormalisableRange<float>(0.0f, 1.0f), 0.1f));
	layout.add(std::make_unique<juce::AudioParameterFloat>("osc1_sustain", "Osc 1 Sustain", juce::NormalisableRange<float>(0.0f, 1.0f), 0.1f));
	layout.add(std::make_unique<juce::AudioParameterFloat>("osc1_release", "Osc 1 Release", juce::NormalisableRange<float>(0.0f, 1.0f), 0.1f));
	layout.add(std::make_unique<juce::AudioParameterInt>("osc1_voices", "Osc 1 Voices", 1, 16, 1));
	layout.add(std::make_unique<juce::AudioParameterInt>("osc1_voicesDetune", "Osc 1 Voices Detune", -100, 100, 0));
	layout.add(std::make_unique<juce::AudioParameterFloat>("osc1_voicesMix", "Osc 1 Voices Mix", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
	layout.add(std::make_unique<juce::AudioParameterFloat>("osc1_voicesPan", "Osc 1 Voices Pan", juce::NormalisableRange<float>(-1.0f, 1.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("osc1_level", "Osc 1 Level", juce::NormalisableRange<float>(0.0f, 1.0f), 0.6f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("osc1_pan", "Osc 1 Pan", juce::NormalisableRange<float>(-1.0f, 1.0f), 0.0f));

    // Oscillator 2 parameters
    layout.add(std::make_unique<juce::AudioParameterBool>("osc2_active", "Osc 2 Active", true));
	layout.add(std::make_unique<juce::AudioParameterChoice>("osc2_waveform", "Osc 2 Waveform", juce::StringArray{ "Sine", "Square", "Saw", "Triangle", "Noise" }, 0));
	layout.add(std::make_unique<juce::AudioParameterInt>("osc2_octave", "Osc 2 Octave", -3, 3, 0));
	layout.add(std::make_unique<juce::AudioParameterInt>("osc2_semitone", "Osc 2 Semitone", -11, 11, 0));
	layout.add(std::make_unique<juce::AudioParameterInt>("osc2_fine", "Osc 2 Fine", -100, 100, 0));
	layout.add(std::make_unique<juce::AudioParameterFloat>("osc2_attack", "Osc 2 Attack", juce::NormalisableRange<float>(0.0f, 1.0f), 0.1f));
	layout.add(std::make_unique<juce::AudioParameterFloat>("osc2_decay", "Osc 2 Decay", juce::NormalisableRange<float>(0.0f, 1.0f), 0.1f));
	layout.add(std::make_unique<juce::AudioParameterFloat>("osc2_sustain", "Osc 2 Sustain", juce::NormalisableRange<float>(0.0f, 1.0f), 0.1f));
	layout.add(std::make_unique<juce::AudioParameterFloat>("osc2_release", "Osc 2 Release", juce::NormalisableRange<float>(0.0f, 1.0f), 0.1f));
	layout.add(std::make_unique<juce::AudioParameterInt>("osc2_voices", "Osc 2 Voices", 1, 16, 1));
	layout.add(std::make_unique<juce::AudioParameterInt>("osc2_voicesDetune", "Osc 2 Voices Detune", -100, 100, 0));
	layout.add(std::make_unique<juce::AudioParameterFloat>("osc2_voicesMix", "Osc 2 Voices Mix", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
	layout.add(std::make_unique<juce::AudioParameterFloat>("osc2_voicesPan", "Osc 2 Voices Pan", juce::NormalisableRange<float>(-1.0f, 1.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("osc2_level", "Osc 2 Level", juce::NormalisableRange<float>(0.0f, 1.0f), 0.6f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("osc2_pan", "Osc 2 Pan", juce::NormalisableRange<float>(-1.0f, 1.0f), 0.0f));

	return layout;
}

// Listen for changes on the ValueTreeState
void PocketsynthAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
	juce::Logger::outputDebugString("Changed parameter: " + parameterID + " new value: " + (juce::String)newValue);

	if (parameterID == "gain")
	{
		// Set the gain of the synth
	}

    if (parameterID == "voices")
    {
		setupSynth();
    }

	if (parameterID == "osc1waveform")
    {
		// Set the waveform of oscillator 1
		for (int i = 0; i < synth.getNumVoices(); i++)
		{
			if (auto* voice = dynamic_cast<OscillatorVoice*>(synth.getVoice(i)))
			{
				//voice->setWaveform(newValue);
			}
		}
    }
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

    // Prepare each voice
    for (int i = 0; i < synth.getNumVoices(); i++)
    {
		if (auto* voice = dynamic_cast<OscillatorVoice*>(synth.getVoice(i)))
		{
			voice->prepareToPlay(sampleRate, samplesPerBlock);
		}
    }
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
    float gainModifier = static_cast<float>(*treeState.getRawParameterValue("gain"));

    for (int channel = 0; channel < totalNumOutputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

		for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
		{
			channelData[sample] *= gainModifier;
		}
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
