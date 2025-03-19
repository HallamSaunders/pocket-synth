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
        // Initialize smoothed values
        osc1FrequencySmoothed.reset(getSampleRate(), 0.01);  // Default sample rate, 10ms smoothing
        osc2FrequencySmoothed.reset(getSampleRate(), 0.01);
    }

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

        // Initialize the cached values with current values
        updateCachedValues();
    }

    bool canPlaySound(juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<OscillatorSound*> (sound) != nullptr;
    }

    void prepareToPlay(double sampleRate, int samplesPerBlock)
    {
        juce::dsp::ProcessSpec spec{ sampleRate, (juce::uint32)samplesPerBlock, 1 };

        // Update smoothers with the actual sample rate
        osc1FrequencySmoothed.reset(sampleRate, 0.01);  // 10ms smoothing
        osc2FrequencySmoothed.reset(sampleRate, 0.01);

        // Prepare oscillators
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

        // Initialize smoothed frequencies with the base frequency
        float osc1_freq = calculateOsc1Frequency(0.0f);
        float osc2_freq = calculateOsc2Frequency(0.0f);
        osc1FrequencySmoothed.setCurrentAndTargetValue(osc1_freq);
        osc2FrequencySmoothed.setCurrentAndTargetValue(osc2_freq);

        // Update all parameters at the start of a note
        updateEnvelopeParameters();
        createOscillators();

        oscLevels[0] = *osc1_level * velocity;
        oscLevels[1] = *osc2_level * velocity;
        osc1_adsr.noteOn();
        osc2_adsr.noteOn();

        // Reset update counters
        //updateCounter = 0;

        // Cache current values for future parameter change detection
        updateCachedValues();
    }

    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
    {
        if (!isVoiceActive()) // Do not process if the voice is not active
            return;

        // Check for parameter updates at a reduced rate to save CPU
        checkAndUpdateParameters();
        /*if (++updateCounter >= updateInterval)
        {
            updateCounter = 0;
            checkAndUpdateParameters();
        }*/

        // Process audio block
        processBlock(outputBuffer, startSample, numSamples);
    }

    void stopNote(float /*velocity*/, bool allowTailOff) override
    {
        osc1_adsr.noteOff();
        osc2_adsr.noteOff();

        if (!allowTailOff || (!osc1_adsr.isActive() && !osc2_adsr.isActive()))
            clearCurrentNote();
    }

    void pitchWheelMoved(int /*newValue*/) override {}
    void controllerMoved(int /*controllerNumber*/, int /*newValue*/) override {}

private:
    double baseFrequency = 440.0;
    std::vector<std::unique_ptr<Oscillator>> osc1, osc2;
    std::array<float, 2> oscLevels = { 0.0f, 0.0f };
    std::array<juce::String, 5> oscWaveforms = { "Sine", "Square", "Saw", "Triangle", "Noise" };

    // Smoothed values for frequency changes
    juce::SmoothedValue<float> osc1FrequencySmoothed;
    juce::SmoothedValue<float> osc2FrequencySmoothed;

    // Update rate limiting
    //int updateCounter = 0;
    //const int updateInterval = 32; // Check for updates every 32 blocks

    // Parameter change flags
    bool osc1WaveformChanged = false;
    bool osc2WaveformChanged = false;
    bool osc1FrequencyChanged = false;
    bool osc2FrequencyChanged = false;
    bool osc1VoiceCountChanged = false;
    bool osc2VoiceCountChanged = false;
    bool osc1EnvelopeChanged = false;
    bool osc2EnvelopeChanged = false;
    bool osc1LevelChanged = false;
    bool osc2LevelChanged = false;

    // Cached parameter values for change detection
    float cached_osc1_active = 0.0f;
    float cached_osc1_waveform = 0.0f;
    float cached_osc1_octave = 0.0f;
    float cached_osc1_semitone = 0.0f;
    float cached_osc1_fine = 0.0f;
    float cached_osc1_attack = 0.0f;
    float cached_osc1_decay = 0.0f;
    float cached_osc1_sustain = 0.0f;
    float cached_osc1_release = 0.0f;
    float cached_osc1_voices = 0.0f;
    float cached_osc1_voicesDetuneMax = 0.0f;
    float cached_osc1_voicesDetune = 0.0f;
    float cached_osc1_voicesMix = 0.0f;
    float cached_osc1_level = 0.0f;

    float cached_osc2_active = 0.0f;
    float cached_osc2_waveform = 0.0f;
    float cached_osc2_octave = 0.0f;
    float cached_osc2_semitone = 0.0f;
    float cached_osc2_fine = 0.0f;
    float cached_osc2_attack = 0.0f;
    float cached_osc2_decay = 0.0f;
    float cached_osc2_sustain = 0.0f;
    float cached_osc2_release = 0.0f;
    float cached_osc2_voices = 0.0f;
    float cached_osc2_voicesDetuneMax = 0.0f;
    float cached_osc2_voicesDetune = 0.0f;
    float cached_osc2_voicesMix = 0.0f;
    float cached_osc2_level = 0.0f;

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

    // Cache parameter values for change detection
    void updateCachedValues()
    {
        cached_osc1_active = *osc1_active;
        cached_osc1_waveform = *osc1_waveform;
        cached_osc1_octave = *osc1_octave;
        cached_osc1_semitone = *osc1_semitone;
        cached_osc1_fine = *osc1_fine;
        cached_osc1_attack = *osc1_attack;
        cached_osc1_decay = *osc1_decay;
        cached_osc1_sustain = *osc1_sustain;
        cached_osc1_release = *osc1_release;
        cached_osc1_voices = *osc1_voices;
        cached_osc1_voicesDetuneMax = *osc1_voicesDetuneMax;
        cached_osc1_voicesDetune = *osc1_voicesDetune;
        cached_osc1_voicesMix = *osc1_voicesMix;
        cached_osc1_level = *osc1_level;

        cached_osc2_active = *osc2_active;
        cached_osc2_waveform = *osc2_waveform;
        cached_osc2_octave = *osc2_octave;
        cached_osc2_semitone = *osc2_semitone;
        cached_osc2_fine = *osc2_fine;
        cached_osc2_attack = *osc2_attack;
        cached_osc2_decay = *osc2_decay;
        cached_osc2_sustain = *osc2_sustain;
        cached_osc2_release = *osc2_release;
        cached_osc2_voices = *osc2_voices;
        cached_osc2_voicesDetuneMax = *osc2_voicesDetuneMax;
        cached_osc2_voicesDetune = *osc2_voicesDetune;
        cached_osc2_voicesMix = *osc2_voicesMix;
        cached_osc2_level = *osc2_level;
    }

    // Check for parameter changes and set flags accordingly
    void checkAndUpdateParameters()
    {
        // Oscillator 1 parameter change detection
        osc1WaveformChanged = (cached_osc1_waveform != *osc1_waveform);
        osc1FrequencyChanged = (cached_osc1_octave != *osc1_octave ||
            cached_osc1_semitone != *osc1_semitone ||
            cached_osc1_fine != *osc1_fine ||
            cached_osc1_voicesDetune != *osc1_voicesDetune ||
            cached_osc1_voicesDetuneMax != *osc1_voicesDetuneMax);
        osc1VoiceCountChanged = (cached_osc1_voices != *osc1_voices);
        osc1EnvelopeChanged = (cached_osc1_attack != *osc1_attack ||
            cached_osc1_decay != *osc1_decay ||
            cached_osc1_sustain != *osc1_sustain ||
            cached_osc1_release != *osc1_release);
        osc1LevelChanged = (cached_osc1_level != *osc1_level);

        // Oscillator 2 parameter change detection
        osc2WaveformChanged = (cached_osc2_waveform != *osc2_waveform);
        osc2FrequencyChanged = (cached_osc2_octave != *osc2_octave ||
            cached_osc2_semitone != *osc2_semitone ||
            cached_osc2_fine != *osc2_fine ||
            cached_osc2_voicesDetune != *osc2_voicesDetune ||
            cached_osc2_voicesDetuneMax != *osc2_voicesDetuneMax);
        osc2VoiceCountChanged = (cached_osc2_voices != *osc2_voices);
        osc2EnvelopeChanged = (cached_osc2_attack != *osc2_attack ||
            cached_osc2_decay != *osc2_decay ||
            cached_osc2_sustain != *osc2_sustain ||
            cached_osc2_release != *osc2_release);
        osc2LevelChanged = (cached_osc2_level != *osc2_level);

        // Apply updates based on what changed
        if (osc1VoiceCountChanged || osc2VoiceCountChanged) {
            // Complete rebuild of oscillators needed
            createOscillators();
        }
        else {
            // Update oscillator parameters without recreation
            if (osc1WaveformChanged || osc1FrequencyChanged) {
                updateOsc1Parameters();
            }

            if (osc2WaveformChanged || osc2FrequencyChanged) {
                updateOsc2Parameters();
            }
        }

        if (osc1EnvelopeChanged || osc2EnvelopeChanged) {
            updateEnvelopeParameters();
        }

        if (osc1LevelChanged) {
            oscLevels[0] = *osc1_level;
        }

        if (osc2LevelChanged) {
            oscLevels[1] = *osc2_level;
        }

        // Update cached values for next comparison
        updateCachedValues();
    }

    // Calculate oscillator frequencies
    float calculateOsc1Frequency(float detune)
    {
        return baseFrequency * std::pow(2.0f, *osc1_octave + *osc1_semitone / 12.0f + *osc1_fine / 1200.0f + detune / 1200.0f);
    }

    float calculateOsc2Frequency(float detune)
    {
        return baseFrequency * std::pow(2.0f, *osc2_octave + *osc2_semitone / 12.0f + *osc2_fine / 1200.0f + detune / 1200.0f);
    }

    // Update parameters for existing oscillators
    void updateOsc1Parameters()
    {
        float detune_osc1 = *osc1_voicesDetune * *osc1_voicesDetuneMax * 100.0f;

        for (int i = 0; i < osc1.size(); ++i)
        {
            if (osc1WaveformChanged) {
                osc1[i]->setWaveform(oscWaveforms[*osc1_waveform]);
            }

            if (osc1FrequencyChanged) {
                float detune = (i - (osc1.size() - 1) / 2.0f) * detune_osc1;
                float freq = calculateOsc1Frequency(detune);

                // Set the smoothed frequency target
                if (i == 0) {
                    osc1FrequencySmoothed.setTargetValue(freq);
                }
                else {
                    // For other oscillators, set frequency directly
                    osc1[i]->setFrequency(freq, getSampleRate());
                }
            }

            osc1[i]->setActive(*osc1_active);

            // Update volume mixing
            float volume = 1.0f - *osc1_voicesMix;
            if (osc1.size() % 2 == 1 && i == osc1.size() / 2)
                volume = *osc1_voicesMix; // Odd case
            else if (osc1.size() % 2 == 0 && (i == osc1.size() / 2 || i == osc1.size() / 2 - 1))
                volume = *osc1_voicesMix; // Even case

            osc1[i]->setVolume(volume);
        }
    }

    void updateOsc2Parameters()
    {
        float detune_osc2 = *osc2_voicesDetune * *osc2_voicesDetuneMax * 100.0f;

        for (int i = 0; i < osc2.size(); ++i)
        {
            if (osc2WaveformChanged) {
                osc2[i]->setWaveform(oscWaveforms[*osc2_waveform]);
            }

            if (osc2FrequencyChanged) {
                float detune = (i - (osc2.size() - 1) / 2.0f) * detune_osc2;
                float freq = calculateOsc2Frequency(detune);

                // Set the smoothed frequency target
                if (i == 0) {
                    osc2FrequencySmoothed.setTargetValue(freq);
                }
                else {
                    // For other oscillators, set frequency directly
                    osc2[i]->setFrequency(freq, getSampleRate());
                }
            }

            osc2[i]->setActive(*osc2_active);

            // Update volume mixing
            float volume = *osc2_voicesMix;
            if (osc2.size() % 2 == 1 && i == osc2.size() / 2)
                volume = 1.0f - *osc2_voicesMix; // Odd case
            else if (osc2.size() % 2 == 0 && (i == osc2.size() / 2 || i == osc2.size() / 2 - 1))
                volume = 1.0f - *osc2_voicesMix; // Even case

            osc2[i]->setVolume(volume);
        }
    }

    // Update envelope parameters
    void updateEnvelopeParameters()
    {
        if (osc1EnvelopeChanged) {
            osc1_adsrParams.attack = *osc1_attack;
            osc1_adsrParams.decay = *osc1_decay;
            osc1_adsrParams.sustain = *osc1_sustain;
            osc1_adsrParams.release = *osc1_release;
            osc1_adsr.setParameters(osc1_adsrParams);
        }

        if (osc2EnvelopeChanged) {
            osc2_adsrParams.attack = *osc2_attack;
            osc2_adsrParams.decay = *osc2_decay;
            osc2_adsrParams.sustain = *osc2_sustain;
            osc2_adsrParams.release = *osc2_release;
            osc2_adsr.setParameters(osc2_adsrParams);
        }
    }

    // Create oscillators from scratch (called on voice count change or initial setup)
    void createOscillators()
    {
        // Clear existing oscillators
        osc1.clear();
        osc2.clear();

        int numVoices_osc1 = static_cast<int>(*osc1_voices);
        int numVoices_osc2 = static_cast<int>(*osc2_voices);

        // Calculate detune amount
        float detune_osc1 = *osc1_voicesDetune * *osc1_voicesDetuneMax * 100.0f; // Convert to cents
        float detune_osc2 = *osc2_voicesDetune * *osc2_voicesDetuneMax * 100.0f;

        // Create oscillator 1 voices
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

            float freq = calculateOsc1Frequency(detune);
            if (i == 0) {
                // For the first oscillator, set the target for smoothing
                osc1FrequencySmoothed.setTargetValue(freq);
            }
            osc->setFrequency(freq, getSampleRate());

            osc->setActive(*osc1_active);
            osc->setVolume(volume);

            // Prepare the oscillator
            juce::dsp::ProcessSpec spec{ getSampleRate(), 512, 1 }; // Use a reasonable default block size
            osc->prepare(spec);

            osc1.emplace_back(std::move(osc));
        }

        // Create oscillator 2 voices
        for (int i = 0; i < numVoices_osc2; ++i)
        {
            auto osc = std::make_unique<Oscillator>();

            // Calculate detune 
            float detune = (i - (numVoices_osc2 - 1) / 2.0f) * detune_osc2;
            float volume = *osc2_voicesMix;

            // Middle oscillator(s) get volume 1 - osc_voicesMix
            if (numVoices_osc2 % 2 == 1 && i == numVoices_osc2 / 2)
                volume = 1 - *osc2_voicesMix; // Odd case
            else if (numVoices_osc2 % 2 == 0 && (i == numVoices_osc2 / 2 || i == numVoices_osc2 / 2 - 1))
                volume = 1 - *osc2_voicesMix; // Even case

            osc->setWaveform(oscWaveforms[*osc2_waveform]);

            float freq = calculateOsc2Frequency(detune);
            if (i == 0) {
                // For the first oscillator, set the target for smoothing
                osc2FrequencySmoothed.setTargetValue(freq);
            }
            osc->setFrequency(freq, getSampleRate());

            osc->setActive(*osc2_active);
            osc->setVolume(volume);

            // Prepare the oscillator
            juce::dsp::ProcessSpec spec{ getSampleRate(), 512, 1 }; // Use a reasonable default block size
            osc->prepare(spec);

            osc2.emplace_back(std::move(osc));
        }
    }

    // Process a block of audio
    void processBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
    {
        for (int sample = 0; sample < numSamples; ++sample)
        {
            // Get smoothed frequency for lead oscillators
            if (osc1.size() > 0 && osc1FrequencyChanged) {
                float smoothedFreq = osc1FrequencySmoothed.getNextValue();
                osc1[0]->setFrequency(smoothedFreq, getSampleRate());
            }

            if (osc2.size() > 0 && osc2FrequencyChanged) {
                float smoothedFreq = osc2FrequencySmoothed.getNextValue();
                osc2[0]->setFrequency(smoothedFreq, getSampleRate());
            }

            float currentSample = 0.0f;
            int numActiveOscillators = 0;

            // Process oscillator 1 voices
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

            // Process oscillator 2 voices
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

            // Normalize output volume based on active oscillators
            if (numActiveOscillators > 0)
                currentSample *= 1.0f / std::sqrt(static_cast<float>(numActiveOscillators));

            // Write to all output channels
            for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
                outputBuffer.addSample(channel, startSample, currentSample);

            ++startSample;
        }
    }
};