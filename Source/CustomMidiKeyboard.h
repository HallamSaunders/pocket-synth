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

	/*void drawUpDownButton(juce::Graphics& g, int w, int h, bool isUpButton, bool isMouseOver, bool isButtonDown) override
	{
        g.fillAll(juce::Colours::transparentBlack); // Hide default arrows

        // Optional: Draw custom arrows
        g.setColour(juce::Colours::white);
        juce::Path arrow;
        float arrowSize = h * 0.3f;
        float centerX = w * 0.5f;
        float centerY = h * 0.5f;

        if (isUpButton) // Draw up arrow
        {
            arrow.addTriangle(centerX, centerY - arrowSize, centerX - arrowSize, centerY + arrowSize, centerX + arrowSize, centerY + arrowSize);
        }
        else // Draw down arrow
        {
            arrow.addTriangle(centerX, centerY + arrowSize, centerX - arrowSize, centerY - arrowSize, centerX + arrowSize, centerY - arrowSize);
        }

        g.fillPath(arrow);
	}*/
};