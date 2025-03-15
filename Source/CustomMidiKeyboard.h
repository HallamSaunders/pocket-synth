/*
  ==============================================================================

    CustomMidiKeyboard.h
    Created: 15 Mar 2025 4:46:06pm
    Author:  Hallam Saunders

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "CustomLookAndFeel.h"

class CustomMidiKeyboard : public juce::MidiKeyboardComponent
{
public:
	CustomMidiKeyboard(juce::MidiKeyboardState& state, juce::MidiKeyboardComponent::Orientation orientation) : juce::MidiKeyboardComponent(state, orientation) {}

	void drawWhiteNote(int midiNoteNumber, juce::Graphics& g, juce::Rectangle<float> area, bool isDown, bool isOver, juce::Colour lineColour, juce::Colour textColour) override
	{
		g.setColour(isDown ? findColour(juce::MidiKeyboardComponent::keyDownOverlayColourId) : findColour(juce::MidiKeyboardComponent::whiteNoteColourId));
		g.fillRect(area);
		g.setColour(findColour(juce::MidiKeyboardComponent::keySeparatorLineColourId));
		g.drawRect(area);
	}

	void drawBlackNote(int midiNoteNumber, juce::Graphics& g, juce::Rectangle<float> area, bool isDown, bool isOver, juce::Colour noteFillColour) override
	{
		g.setColour(isDown ? findColour(juce::MidiKeyboardComponent::keyDownOverlayColourId) : findColour(juce::MidiKeyboardComponent::blackNoteColourId));
		g.fillRect(area);
		g.setColour(findColour(juce::MidiKeyboardComponent::keySeparatorLineColourId));
		g.drawRect(area);
	}
};