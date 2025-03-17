/*
  ==============================================================================

    GlobalControlsComponent.h
    Created: 17 Mar 2025 7:42:00pm
    Author:  Hallam Saunders

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "CustomLookAndFeel.h"
#include "Spacing.h"

class GlobalControlsComponent : public juce::Component
{
public:
	GlobalControlsComponent(juce::AudioProcessorValueTreeState& apvts);
	~GlobalControlsComponent() override;

	void resized() override;
private:
	juce::AudioProcessorValueTreeState& valueTreeState;
	CustomLookAndFeel customLookAndFeel;
	Spacing spacing;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gain_sliderAttachment;
	juce::Label gain_label;
	juce::Slider gain_slider;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> voices_sliderAttachment;
	juce::Label voices_label;
	juce::Slider voices_slider;
};