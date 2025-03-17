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
#include "Oscillator.h"

class OscillatorVoice : public juce::SynthesiserVoice
{
public:
	OscillatorVoice()
	{
		for (auto& osc : oscillators)
			osc.setWaveform("Sine");
	}

	bool canPlaySound(juce::SynthesiserSound* sound) override
	{
		return dynamic_cast<OscillatorSound*> (sound) != nullptr;
	}

	void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) override
	{
		level = velocity * 0.15;

		double baseFrequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);

		for (auto& osc : oscillators)
		{
			osc.setFrequency(baseFrequency, getSampleRate());
			osc.setActive(true);
		}
	}

	void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
	{
		if (!isVoiceActive()) // Do not process if the voice is not active
			return;

		if (oscillators[0].getNextSample() == 0.0f && oscillators[1].getNextSample() == 0.0f)
			return;

		while (--numSamples >= 0)
		{
			float currentSample = 0.0f;

			for (auto& osc : oscillators)
				currentSample += osc.getNextSample();

			for (int i = outputBuffer.getNumChannels(); --i >= 0;)
				outputBuffer.addSample(i, startSample, currentSample);

			++startSample;
		}
	}

	void stopNote(float /*velocity*/, bool allowTailOff) override
	{
		clearCurrentNote();
	}

	void pitchWheelMoved(int /*newValue*/) override {}
	void controllerMoved(int /*controllerNumber*/, int /*newValue*/) override {}

	void setWaveform(int oscIndex, const juce::String& type)
	{
		if (oscIndex >= 0 && oscIndex < oscillators.size())
			oscillators[oscIndex].setWaveform(type);
	}

	void prepareToPlay(double sampleRate, int samplesPerBlock)
	{
		juce::dsp::ProcessSpec spec{ sampleRate, (juce::uint32)samplesPerBlock, 1 };
		for (auto& osc : oscillators)
			osc.prepare(spec);
	}

private:
	std::array<Oscillator, 2> oscillators;
	double level = 0.0;
};