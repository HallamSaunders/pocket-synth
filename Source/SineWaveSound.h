/*
  ==============================================================================

    SineWaveSound.h
    Created: 16 Mar 2025 12:31:18pm
    Author:  Hallam Saunders

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

struct SineWaveSound : public juce::SynthesiserSound
{
    SineWaveSound() {}
    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};