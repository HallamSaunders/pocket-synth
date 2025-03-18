/*
  ==============================================================================

    PresetBarComponent.cpp
    Created: 17 Mar 2025 6:56:00pm
    Author:  Hallam Saunders

  ==============================================================================
*/

#include "PresetBarComponent.h"

PresetBarComponent::PresetBarComponent(PocketsynthAudioProcessor& p) : processor(p)
{
	setLookAndFeel(&customLookAndFeel);

	juce::Logger::outputDebugString("EDITOR: PresetBarComponent created");

	presetSelection_comboBox.onChange = [this] { handlePresetSelection(); };
	presetSelection_comboBox.setTextWhenNoChoicesAvailable("Init");
	updatePresetList();
	presetSelection_comboBox.setSelectedId(1, juce::dontSendNotification);
	addAndMakeVisible(presetSelection_comboBox);

	savePreset_button.setButtonText("Save");
	savePreset_button.onClick = [this] { processor.savePreset(); };
	addAndMakeVisible(savePreset_button);
}

PresetBarComponent::~PresetBarComponent()
{
	setLookAndFeel(nullptr);
}

void PresetBarComponent::updatePresetList()
{
	// Store previous preset name
	juce::String previousPresetName = presetSelection_comboBox.getText();

	// Clear and rebuild combobox with new presets
	presetSelection_comboBox.clear();
	juce::File presetDirectory = processor.getPresetDirectory(); // Get the preset directory
	juce::Array<juce::File> presetFiles = presetDirectory.findChildFiles(juce::File::TypesOfFileToFind::findFiles, false, "*.bdp"); // Initialise empty array of preset files

	// Add the default preset to the combo box
	presetSelection_comboBox.addItem("Init", 1);

	// Check if plugin is active before scanning preset dir
	if (!processor.getLicenseManager().isActivated())
	{
		return;
	}

	for (int i = 0; i < presetFiles.size(); i++)
	{
		presetSelection_comboBox.addItem(presetFiles[i].getFileNameWithoutExtension(), i + 2); // Add each preset file to the combo box, note adding 2 to index, since 1 is reserved for the default preset

		if (presetFiles[i].getFileNameWithoutExtension() == previousPresetName)
		{
			presetSelection_comboBox.setSelectedId(i + 2); // Select the previous preset
		}
	}
}

void PresetBarComponent::handlePresetSelection()
{
	// Handle init preset selection differently
	if (presetSelection_comboBox.getSelectedId() == 1 && presetSelection_comboBox.getText() == "Init")
	{
		processor.loadDefaultPreset();
		previousPresetIndex = 1;
		return;
	}

	juce::String presetName = presetSelection_comboBox.getText();

	if (presetName.isEmpty()) return; // Sometimes, the combobox fires an event with an empty string when being updated

	juce::File presetFile = processor.getPresetDirectory().getChildFile(presetName + ".bdp");

	// Try to open the preset
	if (processor.loadPreset(presetFile))
	{
		juce::Logger::outputDebugString("EDITOR: Loaded preset " + presetName);
		previousPresetIndex = presetSelection_comboBox.getSelectedId();
	}
	else
	{
		juce::Logger::outputDebugString("EDITOR: Failed to load preset " + presetName);
		presetSelection_comboBox.setSelectedId(previousPresetIndex);
	}
}

void PresetBarComponent::resized()
{
	juce::Grid grid = spacing.getPresetBarGridLayout();

	grid.items = {
		juce::GridItem(presetSelection_comboBox)	.withArea(1, 4, 1, 4),
		juce::GridItem(savePreset_button)			.withArea(1, 5, 1, 5)
	};

	grid.performLayout(getLocalBounds());
}
