/*
  ==============================================================================

    Spacing.h
    Created: 8 Mar 2025 12:18:19am
    Author:  Hallam Saunders

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class Spacing
{
public:
    static juce::Grid getGridLayout();
    static constexpr float margin = 5.0f;
private:
	static constexpr float headerHeight = 20.0f;
};