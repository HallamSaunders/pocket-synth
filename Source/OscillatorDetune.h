/*
  ==============================================================================

    OscillatorDetune.h
    Created: 19 Mar 2025 5:49:12pm
    Author:  Hallam Saunders

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class OscillatorDetune
{
public:
	void setWaveform(const juce::String& waveformType)
	{
		for (auto& osc : oscillators)
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

			//osc.reset();
		}
	}

	void setFrequency(float frequency, double sampleRate)
	{
		baseFrequency = frequency;
		updateDetunedFrequencies();

		for (size_t i = 0; i < numVoices; ++i)
		{
			oscillators[i].setFrequency(frequency, sampleRate);
		}
	}

	void prepare(const juce::dsp::ProcessSpec& spec)
	{
		for (auto& osc : oscillators)
			osc.prepare(spec);
	}

	float getNextSample()
	{
		float output = 0.0f;

		for (auto& osc : oscillators)
			output += osc.processSample(0.0f);

		return output / static_cast<float>(oscillators.size());
	}

	void reset()
	{
		for (auto& osc : oscillators)
			osc.reset();
	}

	void setNumVoices(int num)
	{
		numVoices = num;
		oscillators.resize(numVoices, juce::dsp::Oscillator<float>());
		detuneValues.resize(numVoices, 0.0f);
		updateDetunedFrequencies();
	}

	void setDetuneAmount(float amount)
	{
		detuneAmount = amount;
		updateDetunedFrequencies();
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
	void updateDetunedFrequencies()
	{
		float detuneStep = detuneAmount / static_cast<float>(std::max(1, numVoices - 1));

		for (int i = 0; i < numVoices; ++i)
		{
			float detuneOffset = (i - (numVoices - 1) / 2.0f) * detuneStep;
			detuneValues[i] = baseFrequency * std::pow(2.0f, detuneOffset / 1200.0f); // Converting cents to Hz
		}
	}

	std::vector<juce::dsp::Oscillator<float>> oscillators;
	std::vector<float> detuneValues;
	std::atomic<int> numVoices{ 1 };
	std::atomic<float> detuneAmount{ 0.0f };
	float baseFrequency = 440.0f;
	bool active;
};