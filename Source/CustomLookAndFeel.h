/*
  ==============================================================================

    CustomLookAndFeel.h
    Created: 5 Mar 2025 12:06:07pm
    Author:  Hallam Saunders

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel();
    ~CustomLookAndFeel();

    juce::Colour getColourFromID(juce::String colourID);

	// Override drawing properties for buttons
    void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool, bool isButtonDown) override;
	void drawButtonText(juce::Graphics& g, juce::TextButton& button, bool isMouseOverButton, bool isButtonDown) override;

    // Overriding drawing properties for labels
	void drawLabel(juce::Graphics& g, juce::Label& label) override;

	// Overriding drawing properties for text editors
	void drawTextEditorOutline(juce::Graphics& g, int width, int height, juce::TextEditor& textEditor) override;
	void fillTextEditorBackground(juce::Graphics& g, int width, int height, juce::TextEditor& textEditor) override;

    // Overriding drawing properties for rotary sliders
	void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
		float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override;

	// Overriding drawing properties for combo boxes
	void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox& box) override;
	void drawPopupMenuBackground(juce::Graphics& g, int width, int height) override;
	juce::PopupMenu::Options getOptionsForComboBoxPopupMenu(juce::ComboBox& box, juce::Label& label) override;


    static constexpr float fontSizeScale = 0.8f;

private:
    std::unordered_map<juce::String, juce::Colour> customColours;
};