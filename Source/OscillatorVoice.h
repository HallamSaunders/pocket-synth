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
	void setParameters(juce::AudioProcessorValueTreeState& apvts)
	{
		// Oscillator 1 parameters
		osc1_active = apvts.getRawParameterValue("osc1_active");
		osc1_waveform = apvts.getRawParameterValue("osc1_waveform");
		osc1_octave = apvts.getRawParameterValue("osc1_octave");
		osc1_semitone = apvts.getRawParameterValue("osc1_semitone");
		osc1_fine = apvts.getRawParameterValue("osc1_fine");

		osc1_attack = apvts.getRawParameterValue("osc1_attack");
		osc1_decay = apvts.getRawParameterValue("osc1_decay");
		osc1_sustain = apvts.getRawParameterValue("osc1_sustain");
		osc1_release = apvts.getRawParameterValue("osc1_release");

		osc1_voices = apvts.getRawParameterValue("osc1_voices");
		osc1_voicesDetuneMax = apvts.getRawParameterValue("osc1_voicesDetuneMax");
		osc1_voicesDetune = apvts.getRawParameterValue("osc1_voicesDetune");
		osc1_voicesMix = apvts.getRawParameterValue("osc1_voicesMix");
		osc1_voicesPan = apvts.getRawParameterValue("osc1_voicesPan");
		osc1_level = apvts.getRawParameterValue("osc1_level");
		osc1_pan = apvts.getRawParameterValue("osc1_pan");

		// Oscillator 2 parameters
		osc2_active = apvts.getRawParameterValue("osc2_active");
		osc2_waveform = apvts.getRawParameterValue("osc2_waveform");
		osc2_octave = apvts.getRawParameterValue("osc2_octave");
		osc2_semitone = apvts.getRawParameterValue("osc2_semitone");
		osc2_fine = apvts.getRawParameterValue("osc2_fine");

		osc2_attack = apvts.getRawParameterValue("osc2_attack");
		osc2_decay = apvts.getRawParameterValue("osc2_decay");
		osc2_sustain = apvts.getRawParameterValue("osc2_sustain");
		osc2_release = apvts.getRawParameterValue("osc2_release");

		osc2_voices = apvts.getRawParameterValue("osc2_voices");
		osc2_voicesDetuneMax = apvts.getRawParameterValue("osc2_voicesDetuneMax");
		osc2_voicesDetune = apvts.getRawParameterValue("osc2_voicesDetune");
		osc2_voicesMix = apvts.getRawParameterValue("osc2_voicesMix");
		osc2_voicesPan = apvts.getRawParameterValue("osc2_voicesPan");
		osc2_level = apvts.getRawParameterValue("osc2_level");
		osc2_pan = apvts.getRawParameterValue("osc2_pan");
	}

	bool canPlaySound(juce::SynthesiserSound* sound) override
	{
		return dynamic_cast<OscillatorSound*> (sound) != nullptr;
	}

	void prepareToPlay(double sampleRate, int samplesPerBlock)
	{
		juce::dsp::ProcessSpec spec{ sampleRate, (juce::uint32)samplesPerBlock, 1 };
		for (auto& osc : osc1)
		{
			osc->prepare(spec);
		}
		for (auto& osc : osc2)
		{
			osc->prepare(spec);
		}
	}

	void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) override
	{
		// Reset ADSR envelopes and oscillators (prevent phase issues)
		osc1_adsr.reset();
		osc2_adsr.reset();
		osc1.clear();
		osc2.clear();

		baseFrequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);

		updateEnvelopeParameters();
		setupOscillatorUnison();

		oscLevels[0] = *osc1_level * velocity;
		oscLevels[1] = *osc2_level * velocity;
		osc1_adsr.noteOn();
		osc2_adsr.noteOn();
	}

	void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
	{
		if (!isVoiceActive()) // Do not process if the voice is not active
			return;

		if (parametersNeedUpdating.load())
		{
			updateEnvelopeParameters();
			setupOscillatorUnison();
			parametersNeedUpdating = false;
		}

		while (--numSamples >= 0)
		{
			float currentSample = 0.0f;
			int numActiveOscillators = 0;

			for (auto& osc : osc1)
			{
				if (osc->isActive())
				{
					float sample = osc->getNextSample();
					float env = osc1_adsr.getNextSample();
					currentSample += sample * env * oscLevels[0];
					++numActiveOscillators;
				}
			}

			for (auto& osc : osc2)
			{
				if (osc->isActive())
				{
					float sample = osc->getNextSample();
					float env = osc2_adsr.getNextSample();
					currentSample += sample * env * oscLevels[1];
					++numActiveOscillators;
				}
			}

			if (numActiveOscillators > 0)
				currentSample *= 1.0f / std::sqrt(static_cast<float>(numActiveOscillators)); // Normalise the output according to power summation principle

			for (int i = outputBuffer.getNumChannels(); --i >= 0;)
				outputBuffer.addSample(i, startSample, currentSample);

			++startSample;
		}
	}

	void stopNote(float /*velocity*/, bool allowTailOff) override
	{
		osc1_adsr.noteOff();
		osc2_adsr.noteOff();

		if (!allowTailOff || !osc1_adsr.isActive() && !osc2_adsr.isActive())
			clearCurrentNote();
	}

	void pitchWheelMoved(int /*newValue*/) override {}
	void controllerMoved(int /*controllerNumber*/, int /*newValue*/) override {}

	std::atomic<bool> parametersNeedUpdating{ false };

private:
	double baseFrequency = 440.0;
	std::vector<std::unique_ptr<Oscillator>> osc1, osc2;
	std::array<float, 2> oscLevels = { 0.0f, 0.0f };
	std::array<juce::String, 5> oscWaveforms = { "Sine", "Square", "Saw", "Triangle", "Noise" };

	// Oscillator 1 parameters
	juce::ADSR osc1_adsr;
	juce::ADSR::Parameters osc1_adsrParams;
	std::atomic<float>* osc1_active = nullptr;
	std::atomic<float>* osc1_waveform = nullptr;
	std::atomic<float>* osc1_octave = nullptr;
	std::atomic<float>* osc1_semitone = nullptr;
	std::atomic<float>* osc1_fine = nullptr;
	std::atomic<float>* osc1_attack = nullptr;
	std::atomic<float>* osc1_decay = nullptr;
	std::atomic<float>* osc1_sustain = nullptr;
	std::atomic<float>* osc1_release = nullptr;
	std::atomic<float>* osc1_voices = nullptr;
	std::atomic<float>* osc1_voicesDetuneMax = nullptr;
	std::atomic<float>* osc1_voicesDetune = nullptr;
	std::atomic<float>* osc1_voicesMix = nullptr;
	std::atomic<float>* osc1_voicesPan = nullptr;
	std::atomic<float>* osc1_level = nullptr;
	std::atomic<float>* osc1_pan = nullptr;

	// Oscillator 2 parameters
	juce::ADSR osc2_adsr;
	juce::ADSR::Parameters osc2_adsrParams;
	std::atomic<float>* osc2_active = nullptr;
	std::atomic<float>* osc2_waveform = nullptr;
	std::atomic<float>* osc2_octave = nullptr;
	std::atomic<float>* osc2_semitone = nullptr;
	std::atomic<float>* osc2_fine = nullptr;
	std::atomic<float>* osc2_attack = nullptr;
	std::atomic<float>* osc2_decay = nullptr;
	std::atomic<float>* osc2_sustain = nullptr;
	std::atomic<float>* osc2_release = nullptr;
	std::atomic<float>* osc2_voices = nullptr;
	std::atomic<float>* osc2_voicesDetuneMax = nullptr;
	std::atomic<float>* osc2_voicesDetune = nullptr;
	std::atomic<float>* osc2_voicesMix = nullptr;
	std::atomic<float>* osc2_voicesPan = nullptr;
	std::atomic<float>* osc2_level = nullptr;
	std::atomic<float>* osc2_pan = nullptr;

	void updateEnvelopeParameters()
	{
		osc1_adsrParams.attack = *osc1_attack;
		osc1_adsrParams.decay = *osc1_decay;
		osc1_adsrParams.sustain = *osc1_sustain;
		osc1_adsrParams.release = *osc1_release;
		osc1_adsr.setParameters(osc1_adsrParams);

		osc2_adsrParams.attack = *osc2_attack;
		osc2_adsrParams.decay = *osc2_decay;
		osc2_adsrParams.sustain = *osc2_sustain;
		osc2_adsrParams.release = *osc2_release;
		osc2_adsr.setParameters(osc2_adsrParams);
	}

	void setupOscillatorUnison()
	{
		int numVoices_osc1 = static_cast<int>(*osc1_voices);
		int numVoices_osc2 = static_cast<int>(*osc2_voices);
		
		// Calculate detune amount
		float detune_osc1 = *osc1_voicesDetune * *osc1_voicesDetuneMax * 100.0f; // Convert to cents
		float detune_osc2 = *osc2_voicesDetune * *osc2_voicesDetuneMax * 100.0f;

		for (int i = 0; i < numVoices_osc1; ++i)
		{
			auto osc = std::make_unique<Oscillator>();

			// Calculate detune 
			float detune = (i - (numVoices_osc1 - 1) / 2.0f) * detune_osc1;
			float volume = 1.0f - *osc1_voicesMix;

			// Middle oscillator(s) get volume osc_voicesMix, others get 1 - that
			if (numVoices_osc1 % 2 == 1 && i == numVoices_osc1 / 2)
				volume = *osc1_voicesMix; // Odd case
			else if (numVoices_osc1 % 2 == 0 && (i == numVoices_osc1 / 2 || i == numVoices_osc1 / 2 - 1))
				volume = *osc1_voicesMix; // Even case

			osc->setWaveform(oscWaveforms[*osc1_waveform]);
			osc->setFrequency(baseFrequency * std::pow(2.0f, *osc1_octave + *osc1_semitone / 12.0f + *osc1_fine / 1200.0f + detune / 1200.0f), getSampleRate());
			osc->setActive(*osc1_active);
			osc->setVolume(volume);
			osc1.emplace_back(std::move(osc));
		}

		for (int i = 0; i < numVoices_osc2; ++i)
		{
			auto osc = std::make_unique<Oscillator>();

			// Calculate detune 
			float detune = (i - (numVoices_osc2 - 1) / 2.0f) * detune_osc1;
			float volume = *osc2_voicesMix;

			// Middle oscillator(s) get volume 1 - osc_voicesMix
			if (numVoices_osc2 % 2 == 1 && i == numVoices_osc2 / 2)
				volume = 1 - *osc2_voicesMix; // Odd case
			else if (numVoices_osc2 % 2 == 0 && (i == numVoices_osc2 / 2 || i == numVoices_osc2 / 2 - 1))
				volume = 1 - *osc2_voicesMix; // Even case

			osc->setWaveform(oscWaveforms[*osc2_waveform]);
			osc->setFrequency(baseFrequency * std::pow(2.0f, *osc2_octave + *osc2_semitone / 12.0f + *osc2_fine / 1200.0f + detune / 1200.0f), getSampleRate());
			osc->setActive(*osc2_active);
			osc->setVolume(volume);
			osc1.emplace_back(std::move(osc));
		}
	}
};