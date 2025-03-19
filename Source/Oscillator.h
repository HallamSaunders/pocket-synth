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

		osc.reset();
	}

	void setFrequency(float frequency, double sampleRate)
	{
		osc.setFrequency(frequency, sampleRate);
	}

	void prepare(const juce::dsp::ProcessSpec& spec)
	{
		osc.prepare(spec);
	}

	float getNextSample()
	{
		return osc.processSample(0.0f) * volume;
	}

	void reset()
	{
		osc.reset();
	}

	void setVolume(float newVolume)
	{
		volume = newVolume;
	}

	void setActive(bool shouldBeActive)
	{
		active = shouldBeActive;
	}

	bool isActive() const
	{
		return active;
	}
private:
	juce::dsp::Oscillator<float> osc;
	float volume = 1.0f;
	bool active;
};