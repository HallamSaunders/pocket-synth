/*
  ==============================================================================

    OscillatorComponent.h
    Created: 17 Mar 2025 3:33:29pm
    Author:  Hallam

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "CustomLookAndFeel.h"
#include "Spacing.h"

class OscillatorComponent : public juce::Component
{
public:
    OscillatorComponent();
	~OscillatorComponent() override;

	void resized() override;

    static constexpr int rowOffset = 4;
private:
    
};