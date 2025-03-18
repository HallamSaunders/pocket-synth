/*
  ==============================================================================

    GlobalControlsComponent.cpp
    Created: 17 Mar 2025 7:42:00pm
    Author:  Hallam Saunders

  ==============================================================================
*/

#include "GlobalControlsComponent.h"

GlobalControlsComponent::GlobalControlsComponent(juce::AudioProcessorValueTreeState& apvts) : valueTreeState(apvts)
{
	setLookAndFeel(&customLookAndFeel);

	for (auto* slider : { &gain_slider, &voices_slider })
	{
		slider->setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		slider->setTextBoxStyle(juce::Slider::NoTextBox, false, spacing.sliderTextboxDim, spacing.sliderTextboxDim);
		slider->setPopupDisplayEnabled(true, true, this);
	}

	gain_sliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(valueTreeState, "gain", gain_slider);
	addAndMakeVisible(gain_slider);
	gain_label.setText("Gain", juce::dontSendNotification);
	gain_label.attachToComponent(&gain_slider, false);
	gain_label.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(gain_label);

	voices_sliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(valueTreeState, "voices", voices_slider);
	addAndMakeVisible(voices_slider);
	voices_label.setText("Voices", juce::dontSendNotification);
	voices_label.attachToComponent(&voices_slider, false);
	voices_label.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(voices_label);
}

GlobalControlsComponent::~GlobalControlsComponent()
{
	setLookAndFeel(nullptr);
}

void GlobalControlsComponent::resized()
{
	juce::Grid grid = spacing.getGlobalControlsGridLayout();

	grid.items = {
		juce::GridItem(gain_slider)			.withArea(1, 12, 1, 13),
		juce::GridItem(gain_label)			.withArea(2, 12, 2, 13),
		juce::GridItem(voices_slider)		.withArea(1, 11, 1, 12),
		juce::GridItem(voices_label)		.withArea(2, 11, 2, 12)
	};

	grid.performLayout(getLocalBounds());
}