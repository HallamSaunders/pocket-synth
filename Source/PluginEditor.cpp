/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PocketsynthAudioProcessorEditor::PocketsynthAudioProcessorEditor (PocketsynthAudioProcessor& p)
	: AudioProcessorEditor(&p), audioProcessor(p),
	midiKeyboard(p.getMidiKeyboardState(), juce::MidiKeyboardComponent::horizontalKeyboard)
{
	//========== SET UP EDITOR ==========
	// Setup editor and fix aspect ratio
    setResizable(true, true);
	setResizeLimits(600, 400, 750, 500); 
	getConstrainer()->setFixedAspectRatio(1.5f);
	setSize(600, 400);
	setLookAndFeel(&customLookAndFeel);

	audioProcessor.addChangeListener(this);

    //========== LICENSING COMPONENTS ==========
	launchLicenseActivationWindow_button.setButtonText("License Settings");
	launchLicenseActivationWindow_button.onClick = [this]
		{
			if (licenseActivationWindow != nullptr) licenseActivationWindow.reset(); // If one already exists, reset it
			licenseActivationWindow = std::make_unique<LicenseActivationWindow>(audioProcessor.getLicenseManager());
		};
	addAndMakeVisible(launchLicenseActivationWindow_button);

	//========== PRESET NAVIGATION COMPONENTS ==========
	presetSelection_comboBox.onChange = [this] { handlePresetSelection(); };
	presetSelection_comboBox.setTextWhenNoChoicesAvailable("Init");
	updatePresetList();
	presetSelection_comboBox.setSelectedId(1, juce::dontSendNotification); // Select the init preset
	addAndMakeVisible(presetSelection_comboBox);
	savePreset_button.setButtonText("Save");
	savePreset_button.onClick = [this] { audioProcessor.savePreset(); };
	addAndMakeVisible(savePreset_button);

	//========== CONTROL COMPONENTS ==========
	undo_button.setButtonText("Undo");
	undo_button.onClick = [this] { audioProcessor.undo(); };
	addAndMakeVisible(undo_button);
	redo_button.setButtonText("Redo");
	redo_button.onClick = [this] { audioProcessor.redo(); };
	addAndMakeVisible(redo_button);

	// Gain control
	gain_sliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		audioProcessor.getTreeState(), "gain", gain_slider);
	gain_slider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	gain_slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
	gain_slider.setRange(0.0, 1.0, 0.01);
	addAndMakeVisible(gain_slider);
	gain_label.setText("Gain", juce::dontSendNotification);
	gain_label.attachToComponent(&gain_slider, false);
	gain_label.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(gain_label);

	// Midi keyboard
	addAndMakeVisible(midiKeyboard);

	resized();
}

PocketsynthAudioProcessorEditor::~PocketsynthAudioProcessorEditor()
{
	setLookAndFeel(nullptr);
	audioProcessor.removeChangeListener(this);
}

void PocketsynthAudioProcessorEditor::updatePresetList()
{
	// Store previous preset name
	juce::String previousPresetName = presetSelection_comboBox.getText();

	// Clear and rebuild combobox with new presets
	presetSelection_comboBox.clear();
	juce::File presetDirectory = audioProcessor.getPresetDirectory(); // Get the preset directory
	juce::Array<juce::File> presetFiles = presetDirectory.findChildFiles(juce::File::TypesOfFileToFind::findFiles, false, "*.bdp"); // Initialise empty array of preset files

	// Add the default preset to the combo box
	presetSelection_comboBox.addItem("Init", 1);

	// Check if plugin is active before scanning preset dir
	if (!audioProcessor.getLicenseManager().isActivated())
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

void PocketsynthAudioProcessorEditor::handlePresetSelection()
{
	// Handle init preset selection differently
	if (presetSelection_comboBox.getSelectedId() == 1 && presetSelection_comboBox.getText() == "Init")
	{
		audioProcessor.loadDefaultPreset();
		previousPresetIndex = 1;
		return;
	}

	juce::String presetName = presetSelection_comboBox.getText();

	if (presetName.isEmpty()) return; // Sometimes, the combobox fires an event with an empty string when being updated

	juce::File presetFile = audioProcessor.getPresetDirectory().getChildFile(presetName + ".bdp");
	
	// Try to open the preset
	if (audioProcessor.loadPreset(presetFile))
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

bool PocketsynthAudioProcessorEditor::keyPressed(const juce::KeyPress& key)
{
	if (key == juce::KeyPress('z', juce::ModifierKeys::commandModifier, 0))
	{
		audioProcessor.undo();
		return true;
	}
	else if (key == juce::KeyPress('y', juce::ModifierKeys::commandModifier, 0))
	{
		audioProcessor.redo();
		return true;
	}
	return false;
}

void PocketsynthAudioProcessorEditor::changeListenerCallback(juce::ChangeBroadcaster* source)
{
	if (source == &audioProcessor)
	{
		juce::Logger::outputDebugString("EDITOR: Change broadcaster callback received.");
		updatePresetList();
	}
}

//==============================================================================
void PocketsynthAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void PocketsynthAudioProcessorEditor::resized()
{
    juce::Grid grid = spacing.getGridLayout();

	grid.items = { // Don't forget that JUCE grid items are 1-indexed!  
		// Licensing components
		juce::GridItem(launchLicenseActivationWindow_button).withArea(1, 1, 1, 4),
		
		// Preset navigation components
		juce::GridItem(presetSelection_comboBox)			.withArea(1, 11, 1, 15),
		juce::GridItem(savePreset_button)					.withArea(1, 15, 1, 17),

		// Layout control components
		juce::GridItem(gain_slider)							.withArea(2, 15, 4, 17),
		juce::GridItem(gain_label)							.withArea(4, 15, 4, 17),

		// Midi keyboard
		juce::GridItem(midiKeyboard)						.withArea(15, 1, 17, 17),
    };

    grid.performLayout(getLocalBounds().reduced(spacing.margin, spacing.margin));
}
