/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "LicenseManager.h"

//==============================================================================
/**
*/
class PocketsynthAudioProcessor  : public juce::AudioProcessor,
                                                    public juce::ValueTree::Listener,
	                                                public LicenseManager::Listener,
	                                                public juce::ChangeBroadcaster
{
public:
    //==============================================================================
    PocketsynthAudioProcessor();
    ~PocketsynthAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

	// License management
    LicenseManager& getLicenseManager() { return licenseManager; }

    // Preset and state management
	juce::File getPresetDirectory() { return presetDirectory; }
	juce::AudioProcessorValueTreeState& getTreeState() { return treeState; }
	juce::UndoManager& getUndoManager() { return undoManager; }
    void undo() { undoManager.undo(); }
    void redo() { undoManager.redo(); };
    void savePreset();
    bool loadPreset(juce::File newPresetFile);
	void loadDefaultPreset();

	// React to license activation event from LicenseManager
    void onLicenseActivated() override;
	void onLicenseDeactivated() override;

    // Initial values
    static constexpr float initialGain = 0.6f;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PocketsynthAudioProcessor)

    // License management
    LicenseManager licenseManager;

	// Preset and state management
	juce::File presetDirectory;
    juce::AudioProcessorValueTreeState treeState;
    juce::UndoManager undoManager;
	juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void valueTreePropertyChanged(juce::ValueTree& tree, const juce::Identifier& property) override;
};
