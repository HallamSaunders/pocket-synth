/*
  ==============================================================================

    OscillatorVoice.h
    Created: 16 Mar 2025 7:00:49pm
    Author:  Hallam Saunders

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "OscillatorSound.h"

class OscillatorVoice : public juce::SynthesiserVoice
{
public:
	bool canPlaySound(juce::SynthesiserSound* sound) override
	{
		return dynamic_cast<OscillatorSound*> (sound) != nullptr;
	}

	void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) override
	{
		currentAngle = 0.0;
		level = velocity * 0.15;
		tailOff = 0.0;

		double cyclesPerSecond = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
		double cyclesPerSample = cyclesPerSecond / getSampleRate();

		angleDelta = cyclesPerSample * 2.0 * juce::MathConstants<double>::pi;
	}

	void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
	{
		if (angleDelta == 0.0)
			return;

		while (--numSamples >= 0)
		{
			float currentSample = 0.0f;

			if (waveformType == "Sine")
				currentSample = std::sin(currentAngle);
			else if (waveformType == "Square")
				currentSample = (std::sin(currentAngle) >= 0.0f) ? 1.0f : -1.0f;
			else if (waveformType == "Saw")
				currentSample = (2.0f / juce::MathConstants<float>::pi) * currentAngle - 1.0f;
			else if (waveformType == "Triangle")
				currentSample = 2.0f * std::abs(2.0f * (currentAngle / juce::MathConstants<float>::twoPi) - 1.0f) - 1.0f;
			else if (waveformType == "Noise")
				currentSample = 2.0f * juce::Random::getSystemRandom().nextFloat() - 1.0f;

			currentSample *= level;

			if (tailOff > 0.0)
			{
				currentSample *= tailOff;
				tailOff *= 0.99;

				if (tailOff <= 0.005)
				{
					clearCurrentNote();
					angleDelta = 0.0;
					break;
				}
			}
				
			for (int i = outputBuffer.getNumChannels(); --i >= 0;)
				outputBuffer.addSample(i, startSample, currentSample);

			currentAngle += angleDelta;

			if (currentAngle >= juce::MathConstants<double>::twoPi) // Wrap round to 2pi
				currentAngle -= juce::MathConstants<double>::twoPi;

			++startSample;
		}

		if (angleDelta != 0.0)
		{
			if (tailOff > 0.0)
			{
				while (--numSamples >= 0)
				{
					const float currentSample = (float)(std::sin(currentAngle) * level * tailOff);

					for (int i = outputBuffer.getNumChannels(); --i >= 0;)
						outputBuffer.addSample(i, startSample, currentSample);

					currentAngle += angleDelta;
					++startSample;

					tailOff *= 0.99;

					if (tailOff <= 0.005)
					{
						clearCurrentNote();
						angleDelta = 0.0;
						break;
					}
				}
			}
			else
			{
				while (--numSamples >= 0)
				{
					const float currentSample = (float)(std::sin(currentAngle) * level);

					for (int i = outputBuffer.getNumChannels(); --i >= 0;)
						outputBuffer.addSample(i, startSample, currentSample);

					currentAngle += angleDelta;
					++startSample;
				}
			}
		}
	}

	void stopNote(float /*velocity*/, bool allowTailOff) override
	{
		if (allowTailOff)
		{
			if (tailOff == 0.0)
				tailOff = 1.0;
		}
		else
		{
			clearCurrentNote();
			angleDelta = 0.0;
		}
	}

	void pitchWheelMoved(int /*newValue*/) override {}
	void controllerMoved(int /*controllerNumber*/, int /*newValue*/) override {}

	void setWaveform(float newTypeIndex)
	{
		waveformType = waveformTypes[newTypeIndex];
		juce::Logger::outputDebugString("Waveform set to " + waveformType);
	}

private:
	juce::Array<juce::String> waveformTypes = { "Sine", "Square", "Saw", "Triangle", "Noise" };
	juce::String waveformType = "Sine";
	double currentAngle = 0.0, angleDelta = 0.0, level = 0.0, tailOff = 0.0;
};