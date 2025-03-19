/*
  ==============================================================================

    GlobalControlsComponent.cpp
    Created: 17 Mar 2025 7:42:00pm
    Author:  Hallam Saunders

  ==============================================================================
*/

#include "GlobalControlsComponent.h"

GlobalControlsComponent::GlobalControlsComponent(PocketsynthAudioProcessor& p) : processor(p), leftMeter(p.leftLevel), rightMeter(p.rightLevel)

{
	setLookAndFeel(&customLookAndFeel);

	for (auto* slider : { &gain_slider, &voices_slider })
	{
		slider->setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		slider->setTextBoxStyle(juce::Slider::NoTextBox, false, spacing.sliderTextboxDim, spacing.sliderTextboxDim);
		slider->setPopupDisplayEnabled(true, true, this);
	}

	addAndMakeVisible(leftMeter);
	addAndMakeVisible(rightMeter);

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
		juce::GridItem(leftMeter)			.withArea(1, 12, 3, 13),
		juce::GridItem(rightMeter)			.withArea(1, 13, 3, 14),
		juce::GridItem(gain_slider)			.withArea(1, 11, 3, 12),
		juce::GridItem(gain_label)			.withArea(3, 11, 3, 12),
		juce::GridItem(voices_slider)		.withArea(1, 10, 3, 11),
		juce::GridItem(voices_label)		.withArea(3, 10, 3, 11)
	};

	grid.performLayout(getLocalBounds());
}