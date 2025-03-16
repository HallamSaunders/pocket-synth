/*
  ==============================================================================

    OscillatorSound.h
    Created: 16 Mar 2025 7:02:23pm
    Author:  Hallam Saunders

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

struct OscillatorSound : public juce::SynthesiserSound
{
    OscillatorSound() {}
    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};
