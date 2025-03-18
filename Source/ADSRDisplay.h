/*
  ==============================================================================

    ADSRDisplay.h
    Created: 18 Mar 2025 4:56:42pm
    Author:  Hallam Saunders

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class ADSRDisplay : public juce::Component, private juce::Timer
{
public:
    ADSRDisplay(juce::AudioProcessorValueTreeState& apvts, const juce::String& oscId)
	{
		attack = apvts.getRawParameterValue(oscId + "_attack");
		decay = apvts.getRawParameterValue(oscId + "_decay");
		sustain = apvts.getRawParameterValue(oscId + "_sustain");
		release = apvts.getRawParameterValue(oscId + "_release");

		startTimerHz(30);
    }

	~ADSRDisplay() override
	{
		stopTimer();
	}

	void paint(juce::Graphics& g) override
	{
		juce::Rectangle<int> area = getLocalBounds();
		g.setColour(getCustomColour("foreground"));
		g.drawRect(area, 1);

		// Map ADSR values to screen space
		float width = static_cast<float>(area.getWidth());
		float height = static_cast<float>(area.getHeight());

		float attackTime = juce::jmap(attack->load(), 0.001f, 5.0f, 0.0f, width * 0.3f);
		float decayTime = juce::jmap(decay->load(), 0.001f, 5.0f, 0.0f, width * 0.2f);
		float sustainLevel = juce::jmap(sustain->load(), 0.0f, 1.0f, height, height * 0.2f);
		float releaseTime = juce::jmap(release->load(), 0.001f, 5.0f, 0.0f, width * 0.3f);

		float startX = static_cast<float>(area.getX());
		float startY = static_cast<float>(area.getBottom());

        // Attack
        juce::Line<float> attackLine(
            startX, startY,
            startX + attackTime, height * 0.1f
        );
		g.drawArrow(attackLine, 2.0f, 10.0f, 10.0f);

        // Decay
        juce::Line<float> decayLine(
            startX + attackTime, height * 0.1f,
            startX + attackTime + decayTime, sustainLevel
        );
		g.drawArrow(decayLine, 2.0f, 10.0f, 10.0f);

        // Sustain (hold the level)
        juce::Line<float> sustainLine(
            startX + attackTime + decayTime, sustainLevel,
            startX + width * 0.7f, sustainLevel
        );
		g.drawArrow(sustainLine, 2.0f, 10.0f, 10.0f);

        // Release
        juce::Line<float> releaseLine(
            startX + width * 0.7f, sustainLevel,
            startX + width * 0.7f + releaseTime, startY
        );
		g.drawArrow(releaseLine, 2.0f, 10.0f, 10.0f);
	}

	void resized() override
	{

	}
private:
	std::atomic<float>* attack = nullptr;
	std::atomic<float>* decay = nullptr;
	std::atomic<float>* sustain = nullptr;
	std::atomic<float>* release = nullptr;

    juce::Colour getCustomColour(juce::String colourID) const
    {
        if (auto* customLF = dynamic_cast<CustomLookAndFeel*>(&getLookAndFeel()))
        {
            return customLF->getColourFromID(colourID);
        }
        return juce::Colours::black; // Fallback
    }

	void timerCallback() override
	{
		repaint();
	}
};