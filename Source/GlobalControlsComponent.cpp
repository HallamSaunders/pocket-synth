/*
  ==============================================================================

    GlobalControlsComponent.cpp
    Created: 17 Mar 2025 7:42:00pm
    Author:  Hallam Saunders

  ==============================================================================
*/

#include "GlobalControlsComponent.h"

GlobalControlsComponent::GlobalControlsComponent(PocketsynthAudioProcessor& p) : processor(p), meterLeft(p.leftLevel), meterRight(p.rightLevel)

{
	setLookAndFeel(&customLookAndFeel);

	for (auto* slider : { &gain_slider, &voices_slider })
	{
		slider->setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		slider->setTextBoxStyle(juce::Slider::NoTextBox, false, spacing.sliderTextboxDim, spacing.sliderTextboxDim);
		slider->setPopupDisplayEnabled(true, true, this);
	}

	addAndMakeVisible(meterLeft);
	addAndMakeVisible(meterRight);
	left_label.setText("Left", juce::dontSendNotification);
	left_label.attachToComponent(&meterLeft, false);
	left_label.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(left_label);
	right_label.setText("Right", juce::dontSendNotification);
	right_label.attachToComponent(&meterRight, false);
	right_label.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(right_label);

	gain_sliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.getTreeState(), "gain", gain_slider);
	addAndMakeVisible(gain_slider);
	gain_label.setText("Gain", juce::dontSendNotification);
	gain_label.attachToComponent(&gain_slider, false);
	gain_label.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(gain_label);

	voices_sliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.getTreeState(), "voices", voices_slider);
	addAndMakeVisible(voices_slider);
	voices_label.setText("Voices", juce::dontSendNotification);
	voices_label.attachToComponent(&voices_slider, false);
	voices_label.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(voices_label);

	startTimerHz(30);
}

GlobalControlsComponent::~GlobalControlsComponent()
{
	setLookAndFeel(nullptr);
}

void GlobalControlsComponent::timerCallback()
{
	repaint();
}

void GlobalControlsComponent::resized()
{
	juce::Grid grid = spacing.getGlobalControlsGridLayout();

	grid.items = {
		juce::GridItem(meterLeft)			.withArea(1, 23, 3, 24),
		juce::GridItem(meterRight)			.withArea(1, 24, 3, 25),
		juce::GridItem(left_label)			.withArea(3, 23, 3, 24),
		juce::GridItem(right_label)			.withArea(3, 24, 3, 25),
		juce::GridItem(gain_slider)			.withArea(1, 21, 3, 23),
		juce::GridItem(gain_label)			.withArea(3, 21, 3, 23),
		juce::GridItem(voices_slider)		.withArea(1, 19, 3, 21),
		juce::GridItem(voices_label)		.withArea(3, 19, 3, 21)
	};

	grid.performLayout(getLocalBounds());
}