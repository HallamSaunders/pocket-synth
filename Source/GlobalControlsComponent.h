/*
  ==============================================================================

    GlobalControlsComponent.h
    Created: 17 Mar 2025 7:42:00pm
    Author:  Hallam Saunders

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "LevelMeter.h"
#include "CustomLookAndFeel.h"
#include "Spacing.h"

class GlobalControlsComponent : public juce::Component,
								public juce::Timer
{
public:
	GlobalControlsComponent(PocketsynthAudioProcessor& p);
	~GlobalControlsComponent() override;

	void resized() override;
private:
	PocketsynthAudioProcessor& processor;
	CustomLookAndFeel customLookAndFeel;
	Spacing spacing;

	void timerCallback() override;

	LevelMeter leftMeter;
	LevelMeter rightMeter;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gain_sliderAttachment;
	juce::Label gain_label;
	juce::Slider gain_slider;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> voices_sliderAttachment;
	juce::Label voices_label;
	juce::Slider voices_slider;
};