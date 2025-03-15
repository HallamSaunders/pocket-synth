/*
  ==============================================================================

    CustomTextEditor.h
    Created: 10 Mar 2025 5:14:10pm
    Author:  Hallam Saunders

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "CustomLookAndFeel.h"
#include "Spacing.h"

class CustomTextEditor : public juce::TextEditor
{
public:
    CustomTextEditor(const juce::String& name = {}) : juce::TextEditor(name)
    {
        // Make the internal text invisible
        setColour(juce::TextEditor::textColourId, juce::Colours::black.withAlpha(0.0f));

		setLookAndFeel(&customLookAndFeel);
    }

	~CustomTextEditor() override
	{
		setLookAndFeel(nullptr);
	}

protected:
    void paint(juce::Graphics& g) override
    {
        // First draw the default background
        getLookAndFeel().fillTextEditorBackground(g, getWidth(), getHeight(), *this);

        // Then draw the outline
        getLookAndFeel().drawTextEditorOutline(g, getWidth(), getHeight(), *this);

        // Finally draw your custom text
        auto textArea = getLocalBounds().reduced(2, 0);
        g.setColour(customLookAndFeel.getColourFromID("text")); // Or any color you want

        float fontSize = getHeight() * customLookAndFeel.fontSizeScale;
        g.setFont(fontSize);

        g.drawText(getText(), textArea, juce::Justification::left, true);
    }

private:
	CustomLookAndFeel customLookAndFeel;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CustomTextEditor)
};