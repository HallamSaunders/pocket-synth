/*
  ==============================================================================

    Oscillator.h
    Created: 17 Mar 2025 1:33:25pm
    Author:  Hallam

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class Oscillator
{
public:
	Oscillator()
	{
		setWaveform("Sine");
	}

    void setWaveform(const juce::String& waveformType)
    {
		if (waveformType == "Sine")
			osc.initialise([](float x) { return std::sin(x); }, 128);
		else if (waveformType == "Square")
			osc.initialise([](float x) { return x < 0.0f ? -1.0f : 1.0f; }, 128);
		else if (waveformType == "Saw")
			osc.initialise([](float x) { return (2.0f / juce::MathConstants<float>::pi) * x - 1.0f; }, 128);
		else if (waveformType == "Triangle")
			osc.initialise([](float x) { return 2.0f * std::abs(2.0f * (x / juce::MathConstants<float>::twoPi) - 1.0f) - 1.0f; }, 128);
		else if (waveformType == "Noise")
			osc.initialise([](float x) { return 2.0f * juce::Random::getSystemRandom().nextFloat() - 1.0f; }, 128);
		else
			osc.initialise([](float x) { return std::sin(x); }, 128);
    }

	void setFrequency(float frequency)
	{
		osc.setFrequency(frequency);
	}

	void prepare(const juce::dsp::ProcessSpec& spec)
	{
		osc.prepare(spec);
	}

	float getNextSample()
	{
		return osc.processSample(0.0f);
	}
private:
	juce::dsp::Oscillator<float> osc;
};