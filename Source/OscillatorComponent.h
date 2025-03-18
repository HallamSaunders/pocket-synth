/*
  ==============================================================================

    OscillatorComponent.h
    Created: 17 Mar 2025 3:33:29pm
    Author:  Hallam

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "CustomLookAndFeel.h"
#include "Spacing.h"
#include "ADSRDisplay.h"

class OscillatorComponent : public juce::Component
{
public:
    OscillatorComponent(juce::AudioProcessorValueTreeState& apvts, const juce::String& osc_name, const juce::String& oscId);
	~OscillatorComponent() override;

	void resized() override;
private:
	juce::AudioProcessorValueTreeState& valueTreeState;
	CustomLookAndFeel customLookAndFeel;
	Spacing spacing;

	// Display components
	juce::Label name_label;
	ADSRDisplay adsrDisplay;

	// Interactive components
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> active_toggleButtonAttachment;
	juce::ToggleButton active_toggleButton;

	std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> waveform_comboBoxAttachment;
	juce::ComboBox waveform_comboBox;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> octave_sliderAttachment;
	juce::Label octave_label;
	juce::Slider octave_slider;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> semitone_sliderAttachment;
	juce::Label semitone_label;
	juce::Slider semitone_slider;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> fine_sliderAttachment;
	juce::Label fine_label;
	juce::Slider fine_slider;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attack_sliderAttachment;
	juce::Label attack_label;
	juce::Slider attack_slider;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decay_sliderAttachment;
	juce::Label decay_label;
	juce::Slider decay_slider;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sustain_sliderAttachment;
	juce::Label sustain_label;
	juce::Slider sustain_slider;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> release_sliderAttachment;
	juce::Label release_label;
	juce::Slider release_slider;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> voices_sliderAttachment;
	juce::Label voices_label;
	juce::Slider voices_slider;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> voicesDetune_sliderAttachment;
	juce::Label voicesDetune_label;
	juce::Slider voicesDetune_slider;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> voicesMix_sliderAttachment;
	juce::Label voicesMix_label;
	juce::Slider voicesMix_slider;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> voicesPan_sliderAttachment;
	juce::Label voicesPan_label;
	juce::Slider voicesPan_slider;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> level_sliderAttachment;
	juce::Label level_label;
	juce::Slider level_slider;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> pan_sliderAttachment;
	juce::Label pan_label;
	juce::Slider pan_slider;
};