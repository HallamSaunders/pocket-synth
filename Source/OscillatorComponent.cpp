/*
  ==============================================================================

    OscillatorComponent.cpp
    Created: 17 Mar 2025 3:33:29pm
    Author:  Hallam

  ==============================================================================
*/

#include "OscillatorComponent.h"

OscillatorComponent::OscillatorComponent(juce::AudioProcessorValueTreeState& apvts, const juce::String& osc_name, const juce::String& oscId) : valueTreeState(apvts)
{
	setLookAndFeel(&customLookAndFeel);

	// Name label
	name_label.setText(osc_name, juce::dontSendNotification);
	name_label.setJustificationType(juce::Justification::left);
	addAndMakeVisible(name_label);

	// Active toggle button
	active_toggleButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
		valueTreeState, oscId + "_active", active_toggleButton);
	addAndMakeVisible(active_toggleButton);

	// Waveform combobox
	waveform_comboBoxAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
		valueTreeState, oscId + "_waveform", waveform_comboBox);
	waveform_comboBox.addItem("Sine", 1);
	waveform_comboBox.addItem("Square", 2);
	waveform_comboBox.addItem("Saw", 3);
	waveform_comboBox.addItem("Triangle", 4);
	waveform_comboBox.addItem("Noise", 5);
	waveform_comboBox.setSelectedId(1, juce::dontSendNotification);
	waveform_comboBox.onChange = [this, oscId]
		{
			juce::String paramId = oscId + "_waveform";

			juce::Logger::outputDebugString(paramId);

			if (auto* param = dynamic_cast<juce::AudioParameterChoice*>(valueTreeState.getParameter(paramId)))
			{
				// Normalize the selected ID to a range between 0 and 1
				param->setValueNotifyingHost(
					static_cast<float>(waveform_comboBox.getSelectedId() - 1) / (param->choices.size() - 1)
				);
			}
		};
	addAndMakeVisible(waveform_comboBox);

	// Tuning sliders
	for (auto* slider : { &octave_slider, &semitone_slider, &fine_slider })
	{
		slider->setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		slider->setTextBoxStyle(juce::Slider::NoTextBox, false, spacing.sliderTextboxDim, spacing.sliderTextboxDim);
		slider->setPopupDisplayEnabled(true, true, this);
	}

	octave_sliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		valueTreeState, oscId + "_octave", octave_slider);
	addAndMakeVisible(octave_slider);
	octave_label.setText("Octave", juce::dontSendNotification);
	octave_label.attachToComponent(&octave_slider, false);
	octave_label.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(octave_label);

	semitone_sliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		valueTreeState, oscId + "_semitone", semitone_slider);
	addAndMakeVisible(semitone_slider);
	semitone_label.setText("Semitone", juce::dontSendNotification);
	semitone_label.attachToComponent(&semitone_slider, false);
	semitone_label.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(semitone_label);

	fine_sliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		valueTreeState, oscId + "_fine", fine_slider);
	addAndMakeVisible(fine_slider);
	fine_label.setText("Fine", juce::dontSendNotification);
	fine_label.attachToComponent(&fine_slider, false);
	fine_label.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(fine_label);

	// Envelope sliders
	for (auto* slider : { &attack_slider, &decay_slider, &sustain_slider, &release_slider })
	{
		slider->setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		slider->setTextBoxStyle(juce::Slider::NoTextBox, false, spacing.sliderTextboxDim, spacing.sliderTextboxDim);
		slider->setPopupDisplayEnabled(true, true, this);
	}

	attack_sliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		valueTreeState, oscId + "_attack", attack_slider);
	addAndMakeVisible(attack_slider);
	attack_label.setText("Attack", juce::dontSendNotification);
	attack_label.attachToComponent(&attack_slider, false);
	attack_label.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(attack_label);

	decay_sliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		valueTreeState, oscId + "_decay", decay_slider);
	addAndMakeVisible(decay_slider);
	decay_label.setText("Decay", juce::dontSendNotification);
	decay_label.attachToComponent(&decay_slider, false);
	decay_label.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(decay_label);

	sustain_sliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		valueTreeState, oscId + "_sustain", sustain_slider);
	addAndMakeVisible(sustain_slider);
	sustain_label.setText("Sustain", juce::dontSendNotification);
	sustain_label.attachToComponent(&sustain_slider, false);
	sustain_label.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(sustain_label);

	release_sliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		valueTreeState, oscId + "_release", release_slider);
	addAndMakeVisible(release_slider);
	release_label.setText("Release", juce::dontSendNotification);
	release_label.attachToComponent(&release_slider, false);
	release_label.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(release_label);

	// Voices sliders
	for (auto* slider : { &voices_slider, &voicesDetune_slider, &voicesMix_slider, &voicesPan_slider })
	{
		slider->setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		slider->setTextBoxStyle(juce::Slider::NoTextBox, false, spacing.sliderTextboxDim, spacing.sliderTextboxDim);
		slider->setPopupDisplayEnabled(true, true, this);
	}

	voices_sliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		valueTreeState, oscId + "_voices", voices_slider);
	addAndMakeVisible(voices_slider);
	voices_label.setText("Voices", juce::dontSendNotification);
	voices_label.attachToComponent(&voices_slider, false);
	voices_label.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(voices_label);

	voicesDetune_sliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		valueTreeState, oscId + "_voicesDetune", voicesDetune_slider);
	addAndMakeVisible(voicesDetune_slider);
	voicesDetune_label.setText("Detune", juce::dontSendNotification);
	voicesDetune_label.attachToComponent(&voicesDetune_slider, false);
	voicesDetune_label.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(voicesDetune_label);

	voicesMix_sliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		valueTreeState, oscId + "_voicesMix", voicesMix_slider);
	addAndMakeVisible(voicesMix_slider);
	voicesMix_label.setText("Mix", juce::dontSendNotification);
	voicesMix_label.attachToComponent(&voicesMix_slider, false);
	voicesMix_label.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(voicesMix_label);

	voicesPan_sliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		valueTreeState, oscId + "_voicesPan", voicesPan_slider);
	addAndMakeVisible(voicesPan_slider);
	voicesPan_label.setText("Pan", juce::dontSendNotification);
	voicesPan_label.attachToComponent(&voicesPan_slider, false);
	voicesPan_label.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(voicesPan_label);

	// Global sliders
	for (auto* slider : { &level_slider, &pan_slider })
	{
		slider->setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		slider->setTextBoxStyle(juce::Slider::NoTextBox, false, spacing.sliderTextboxDim, spacing.sliderTextboxDim);
		slider->setPopupDisplayEnabled(true, true, this);
	}

	level_sliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		valueTreeState, oscId + "_level", level_slider);
	addAndMakeVisible(level_slider);
	level_label.setText("Level", juce::dontSendNotification);
	level_label.attachToComponent(&level_slider, false);
	level_label.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(level_label);

	pan_sliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		valueTreeState, oscId + "_pan", pan_slider);
	addAndMakeVisible(pan_slider);
	pan_label.setText("Pan", juce::dontSendNotification);
	pan_label.attachToComponent(&pan_slider, false);
	pan_label.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(pan_label);
}

OscillatorComponent::~OscillatorComponent()
{
	setLookAndFeel(nullptr);
}

void OscillatorComponent::resized()
{
	juce::Grid grid = spacing.getOscillatorGridLayout();

	grid.items = {
		// Title bar
		juce::GridItem(name_label)				.withArea(1, 1, 1, 5),
		juce::GridItem(active_toggleButton)		.withArea(1, 5, 1, 7),

		// Waveform combobox
		juce::GridItem(waveform_comboBox)		.withArea(2, 1, 2, 5),

		// Tuning sliders and labels
		juce::GridItem(octave_slider)			.withArea(2, 5, 4, 7),
		juce::GridItem(octave_label)			.withArea(4, 5, 4, 7),
		juce::GridItem(semitone_slider)			.withArea(5, 5, 7, 7),
		juce::GridItem(semitone_label)			.withArea(7, 5, 7, 7),
		juce::GridItem(fine_slider)				.withArea(8, 5, 10, 7),
		juce::GridItem(fine_label)				.withArea(10, 5, 10, 7),

		// Envelope sliders and labels
		juce::GridItem(attack_slider)			.withArea(8, 7, 10, 9),
		juce::GridItem(attack_label)			.withArea(10, 7, 10, 9),
		juce::GridItem(decay_slider)			.withArea(8, 9, 10, 11),
		juce::GridItem(decay_label)				.withArea(10, 9, 10, 11),
		juce::GridItem(sustain_slider)			.withArea(8, 11, 10, 13),
		juce::GridItem(sustain_label)			.withArea(10, 11, 10, 13),
		juce::GridItem(release_slider)			.withArea(8, 13, 10, 15),
		juce::GridItem(release_label)			.withArea(10, 13, 10, 15),

		// Voices sliders and labels
		juce::GridItem(voices_slider)			.withArea(8, 15, 10, 17),
		juce::GridItem(voices_label)			.withArea(10, 15, 10, 17),
		juce::GridItem(voicesDetune_slider)		.withArea(8, 17, 10, 19),
		juce::GridItem(voicesDetune_label)		.withArea(10, 17, 10, 19),
		juce::GridItem(voicesMix_slider)		.withArea(8, 19, 10, 21),
		juce::GridItem(voicesMix_label)			.withArea(10, 19, 10, 21),
		juce::GridItem(voicesPan_slider)		.withArea(8, 21, 10, 23),
		juce::GridItem(voicesPan_label)			.withArea(10, 21, 10, 23),

		// Global sliders and labels
		juce::GridItem(level_slider)			.withArea(2, 23, 4, 25),
		juce::GridItem(level_label)				.withArea(4, 23, 4, 25),
		juce::GridItem(pan_slider)				.withArea(5, 23, 7, 25),
		juce::GridItem(pan_label)				.withArea(7, 23, 7, 25)
	};

	grid.performLayout(getLocalBounds());
}