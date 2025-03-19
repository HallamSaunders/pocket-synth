/*
  ==============================================================================

    LevelMeter.h
    Created: 19 Mar 2025 11:24:39am
    Author:  Hallam Saunders

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class LevelMeter : public juce::Component, public juce::Timer
{
public:
    LevelMeter(std::atomic<float>& levelRef) : level(levelRef)
    {
		startTimerHz(30);
	}

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        g.setColour(juce::Colours::black);
        g.fillRect(bounds);

        float meterHeight = juce::jmap(level.load(), -60.0f, 0.0f, 0.0f, bounds.getHeight());
        g.setColour(juce::Colours::green);
        g.fillRect(bounds.removeFromBottom(meterHeight));
    }

private:
	std::atomic<float>& level;

	void timerCallback() override
	{
		repaint();
	}
};