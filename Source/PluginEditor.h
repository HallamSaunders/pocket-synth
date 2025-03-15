/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "CustomLookAndFeel.h"
#include "CustomTextEditor.h"
#include "Spacing.h"
#include "LicenseActivationWindow.h"

//==============================================================================
/**
*/
class PocketsynthAudioProcessorEditor  : public juce::AudioProcessorEditor,
	                                                      public juce::ChangeListener
{
public:
    PocketsynthAudioProcessorEditor (PocketsynthAudioProcessor&);
    ~PocketsynthAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PocketsynthAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PocketsynthAudioProcessorEditor)

	// Methods for management and system events
	bool keyPressed(const juce::KeyPress& key) override;
	void changeListenerCallback(juce::ChangeBroadcaster* source) override;

	// Custom LookAndFeel and Spacing
    CustomLookAndFeel customLookAndFeel;
    Spacing spacing;

    // Licensing components
	juce::TextButton launchLicenseActivationWindow_button;
	std::unique_ptr<LicenseActivationWindow> licenseActivationWindow;

    // Preset navigation components
	juce::ComboBox presetSelection_comboBox;
	int previousPresetIndex = 1;
    void updatePresetList();
	void handlePresetSelection();
    juce::TextButton savePreset_button;

    // Control components
    juce::TextButton undo_button;
	juce::TextButton redo_button;

	//========== PARAMETER COMPONENTS ==========
	// Gain parameter components
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gain_sliderAttachment;
	juce::Slider gain_slider;
	juce::Label gain_label;
};
