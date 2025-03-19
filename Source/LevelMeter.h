/*
  ==============================================================================

    LevelMeter.h
    Created: 19 Mar 2025 11:24:39am
    Author:  Hallam Saunders

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "CustomLookAndFeel.h"
#include "Spacing.h"

class LevelMeter : public juce::Component, public juce::Timer
{
public:
	LevelMeter(std::atomic<float>& lev) : level(lev)
    {
		setLookAndFeel(&customLookAndFeel);
		startTimerHz(30);
	}

	~LevelMeter() override
	{
		setLookAndFeel(nullptr);
	}

	void paint(juce::Graphics& g) override
	{
		auto bounds = getLocalBounds().toFloat();

		float meterHeight = juce::jmap(level.load(), -60.0f, 0.0f, 0.0f, bounds.getHeight());
		juce::Rectangle<float> meter(bounds.getX(), bounds.getY() + (bounds.getHeight() - meterHeight), bounds.getWidth(), meterHeight);

		g.setColour(customLookAndFeel.getColourFromID("foreground"));
		g.fillRect(meter);
	}

private:
	CustomLookAndFeel customLookAndFeel;
	Spacing spacing;

    std::atomic<float>& level;

	void timerCallback() override
	{
		repaint();
	}
};