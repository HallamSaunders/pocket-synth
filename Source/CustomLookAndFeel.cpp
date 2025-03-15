/*
  ==============================================================================

    CustomLookAndFeel.cpp
    Created: 5 Mar 2025 12:06:07pm
    Author:  Hallam Saunders

  ==============================================================================
*/

#include "CustomLookAndFeel.h"

CustomLookAndFeel::CustomLookAndFeel()
{
    // Set custom colours in the map (useful if they are not directly tied to a component)
    customColours["background"] = juce::Colour(0, 0, 0);                // Black
    customColours["foreground"] = juce::Colour(255, 255, 255);          // White
    customColours["border"] = juce::Colour(128, 128, 128);              // Grey
	customColours["focusedBorder"] = juce::Colour(255, 255, 255);       // White
	customColours["highlight"] = juce::Colour(255, 255, 255);           // White
    customColours["active"] = juce::Colour(0, 255, 0);                  // Green
	customColours["inactive"] = juce::Colour(255, 0, 0);                // Red
	customColours["text"] = juce::Colour(255, 255, 255);                // White
	customColours["dullText"] = juce::Colour(128, 128, 128);            // Grey
	customColours["componentBackground"] = juce::Colour(0, 0, 0);       // Black
	customColours["componentBackgroundDull"] = juce::Colour(0, 0, 0);   // Black

    // Window colours
    setColour(juce::ResizableWindow::backgroundColourId, getColourFromID("background"));
	setColour(juce::DocumentWindow::backgroundColourId, getColourFromID("background"));
    
    // Button colours
	setColour(juce::TextButton::buttonColourId, getColourFromID("componentBackground"));
	setColour(juce::TextButton::buttonOnColourId, getColourFromID("componentBackgroundDull"));
	setColour(juce::TextButton::textColourOnId, getColourFromID("dullText"));
	setColour(juce::TextButton::textColourOffId, getColourFromID("text"));

	// Label colours
	setColour(juce::Label::textColourId, getColourFromID("text"));

	// Text editor colours
	setColour(juce::TextEditor::backgroundColourId, getColourFromID("componentBackground"));
	setColour(juce::TextEditor::outlineColourId, getColourFromID("border"));
	setColour(juce::TextEditor::focusedOutlineColourId, getColourFromID("focusedBorder"));
	setColour(juce::TextEditor::highlightColourId, getColourFromID("highlight"));
	setColour(juce::TextEditor::highlightedTextColourId, getColourFromID("text"));
	setColour(juce::TextEditor::textColourId, getColourFromID("text"));

	// Rotary slider colours
	setColour(juce::Slider::rotarySliderFillColourId, getColourFromID("componentBackground"));
	setColour(juce::Slider::rotarySliderOutlineColourId, getColourFromID("foreground"));
	setColour(juce::Slider::thumbColourId, getColourFromID("foreground"));

	// Combobox colours
	setColour(juce::ComboBox::backgroundColourId, getColourFromID("componentBackground"));
	setColour(juce::ComboBox::outlineColourId, getColourFromID("border"));
	setColour(juce::ComboBox::buttonColourId, getColourFromID("foreground"));
	setColour(juce::ComboBox::textColourId, getColourFromID("text"));
}

CustomLookAndFeel::~CustomLookAndFeel()
{
    customColours.clear();
}

juce::Colour CustomLookAndFeel::getColourFromID(juce::String colourID)
{
	auto colour = customColours.find(colourID);

	if (colour != customColours.end())
	{
		return colour->second;
	}
	else
	{
		juce::Logger::outputDebugString("CUSTOM LOOK AND FEEL: couldn't find that ID, providing default.");
		return juce::Colours::red; // Default colour if we cannot find the specified one
	}
}

//========== OVERRIDE DRAWING METHODS ==========
// Override drawing properties for buttons
void CustomLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool, bool isButtonDown)
{
    auto buttonArea = button.getLocalBounds();

	g.setColour(getColourFromID("border"));
    g.drawRect(buttonArea);
	g.setColour(button.findColour(juce::TextButton::buttonColourId));
	g.fillRect(buttonArea.reduced(1));
}

void CustomLookAndFeel::drawButtonText(juce::Graphics& g, juce::TextButton& button, bool isMouseOverButton, bool isButtonDown)
{
    auto buttonArea = button.getLocalBounds();

	// Calculate and set font size as a fraction of the smallest dimension
	float fontSize = juce::jmin(buttonArea.getWidth(), buttonArea.getHeight()) * fontSizeScale;
    g.setFont(fontSize);

	if (isButtonDown)
	{
		g.setColour(button.findColour(juce::TextButton::textColourOnId));
	}
	else
	{
		g.setColour(button.findColour(juce::TextButton::textColourOffId));
	}

    g.drawText(button.getButtonText(), buttonArea, juce::Justification::centred);
}

// Override drawing properties for labels
void CustomLookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label)
{
	auto labelArea = label.getLocalBounds();

	// Calculate and set font size as a fraction of the smallest dimension
	float fontSize = juce::jmin(labelArea.getWidth(), labelArea.getHeight()) * fontSizeScale;
	g.setFont(fontSize);
    g.setColour(label.findColour(juce::Label::textColourId));
	g.drawText(label.getText(), labelArea, juce::Justification::centred);
}

// Overriding drawing properties for text editors
void CustomLookAndFeel::drawTextEditorOutline(juce::Graphics& g, int width, int height, juce::TextEditor& textEditor)
{
	auto textEditorArea = textEditor.getLocalBounds();

	g.setColour(textEditor.findColour(juce::TextEditor::outlineColourId));
	g.drawRect(textEditorArea);
}

void CustomLookAndFeel::fillTextEditorBackground(juce::Graphics& g, int width, int height, juce::TextEditor& textEditor)
{
	auto textEditorArea = textEditor.getLocalBounds();
	g.setColour(textEditor.findColour(juce::TextEditor::backgroundColourId));
	g.fillRect(textEditorArea);
}

// Overriding drawing properties for rotary sliders
void CustomLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
	float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider)
{
	// Calculate radius and centre of the rotary slider
	auto radius = (float)juce::jmin(width / 2, height / 2) - 4.0f;
	auto centreX = (float)x + (float)width * 0.5f;
	auto centreY = (float)y + (float)height * 0.5f;
	auto rx = centreX - radius;
	auto ry = centreY - radius;
	auto rw = radius * 2.0f;
	auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

	// Draw the rotary slider outline
	g.setColour(slider.findColour(juce::Slider::rotarySliderOutlineColourId));
	g.drawEllipse(rx, ry, rw, rw, 1.0f);

	// Draw the pointer
	juce::Path p;
	auto pointerLength = radius * 0.5f;
	auto pointerThickness = 2.0f;
	p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
	p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
	g.setColour(slider.findColour(juce::Slider::thumbColourId));
	g.fillPath(p);
}

// Overriding drawing properties for combo boxes
void CustomLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox& box)
{
	auto boxArea = box.getLocalBounds();

	// Draw the combo box outline
	g.setColour(box.findColour(juce::ComboBox::outlineColourId));
	g.drawRect(boxArea);

	// Draw the combo box background
	g.setColour(box.findColour(juce::ComboBox::backgroundColourId));
	g.fillRect(boxArea.reduced(1));

	// Draw combobox button (left half and right half converge on one central spot)
	g.setColour(box.findColour(juce::ComboBox::buttonColourId));
	auto heightMultiplier = 0.35f;
	auto widthMultiplier = 0.2f;
	auto buttonLeftStartX = buttonX + widthMultiplier * buttonW;
	auto buttonRightStartX = buttonX + buttonW - (widthMultiplier * buttonW);
	auto buttonStartY = buttonY + heightMultiplier * buttonH;
	auto buttonFinishX = buttonX + buttonW / 2;
	auto buttonFinishY = buttonY + buttonH - (heightMultiplier * buttonH);
	g.drawLine(buttonLeftStartX, buttonStartY, buttonFinishX, buttonFinishY, 1.0f);
	g.drawLine(buttonRightStartX, buttonStartY, buttonFinishX, buttonFinishY, 1.0f);
}

void CustomLookAndFeel::drawPopupMenuBackground(juce::Graphics& g, int width, int height)
{
	g.setColour(getColourFromID("componentBackground"));
	g.fillRect(0, 0, width, height);

	g.setColour(getColourFromID("border"));
	g.drawRect(0, 0, width, height);
}

juce::PopupMenu::Options CustomLookAndFeel::getOptionsForComboBoxPopupMenu(juce::ComboBox& box, juce::Label& label)
{
	return juce::PopupMenu::Options()
		.withTargetComponent(&box)
		.withMinimumNumColumns(box.getNumItems() / 10);
}