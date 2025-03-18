/*
  ==============================================================================

    PresetBarComponent.h
    Created: 17 Mar 2025 6:56:00pm
    Author:  Hallam Saunders

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "CustomLookAndFeel.h"
#include "Spacing.h"
#include "PluginProcessor.h"

class PresetBarComponent : public juce::Component
{
public:
    PresetBarComponent(PocketsynthAudioProcessor& p);
	~PresetBarComponent() override;

	void resized() override;

private:
    PocketsynthAudioProcessor& processor;
	CustomLookAndFeel customLookAndFeel;
	Spacing spacing;

    juce::ComboBox presetSelection_comboBox;
    int previousPresetIndex = 1;
    void updatePresetList();
    void handlePresetSelection();
    juce::TextButton savePreset_button;
};