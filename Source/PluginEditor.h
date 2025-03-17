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
#include "CustomMidiKeyboard.h"

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
	juce::Label gain_label;
	juce::Slider gain_slider;

	// Voices components
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> voices_sliderAttachment;
	juce::Label voices_label;
	juce::Slider voices_slider;

	// Oscillator 1 components
	juce::Label osc1_label;

	std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> osc1waveform_comboBoxAttachment;
	juce::ComboBox osc1waveform_comboBox;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc1level_sliderAttachment;
	juce::Label osc1level_label;
	juce::Slider osc1level_slider;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc1octave_sliderAttachment;
	juce::Label osc1octave_label;
	juce::Slider osc1octave_slider;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc1semitone_sliderAttachment;
	juce::Label osc1semitone_label;
	juce::Slider osc1semitone_slider;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc1fine_sliderAttachment;
	juce::Label osc1fine_label;
	juce::Slider osc1fine_slider;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc1attack_sliderAttachment;
	juce::Label osc1attack_label;
	juce::Slider osc1attack_slider;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc1decay_sliderAttachment;
	juce::Label osc1decay_label;
	juce::Slider osc1decay_slider;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc1sustain_sliderAttachment;
	juce::Label osc1sustain_label;
	juce::Slider osc1sustain_slider;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc1release_sliderAttachment;
	juce::Label osc1release_label;
	juce::Slider osc1release_slider;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc1voices_sliderAttachment;
	juce::Label osc1voices_label;
	juce::Slider osc1voices_slider;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc1voicesDetune_sliderAttachment;
	juce::Label osc1voicesDetune_label;
	juce::Slider osc1voicesDetune_slider;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc1voicesMix_sliderAttachment;
	juce::Label osc1voicesMix_label;
	juce::Slider osc1voicesMix_slider;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc1voicesPan_sliderAttachment;
	juce::Label osc1voicesPan_label;
	juce::Slider osc1voicesPan_slider;

	// Midi keyboard component
	CustomMidiKeyboard midiKeyboard;
};
