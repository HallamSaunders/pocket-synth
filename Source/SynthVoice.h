/*
  ==============================================================================

    SynthVoice.h
    Created: 17 Mar 2025 1:18:51pm
    Author:  Hallam

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "OscillatorVoice.h"

class SynthVoice : public juce::SynthesiserVoice
{
public:
    bool canPlaySound(juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<OscillatorSound*> (sound) != nullptr;
    }

	void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) override
	{
        
	}

private:
    OscillatorVoice oscillator1, oscillator2;
};
