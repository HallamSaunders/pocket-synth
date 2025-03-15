/*
  ==============================================================================

    LicenseActivationComponent.cpp
    Created: 11 Mar 2025 6:34:35pm
    Author:  Hallam Saunders

  ==============================================================================
*/

#include "LicenseActivationComponent.h"

LicenseActivationComponent::LicenseActivationComponent(LicenseManager& manager) : licenseManager(manager)
{
	setLookAndFeel(&customLookAndFeel);

	if (!licenseManager.isActivated())
	{
		// If the license is not activated, show the activation components
		activationIndicator_label.setText("You do not have an active license on this computer.", juce::NotificationType::dontSendNotification);
		activationIndicator_label.applyColourToAllText(customLookAndFeel.getColourFromID("inactive"));

		username_input.setMultiLine(false);
		username_input.setReturnKeyStartsNewLine(false);
		username_input.setTextToShowWhenEmpty("Enter your username...", customLookAndFeel.getColourFromID("dullText"));
		addAndMakeVisible(username_input);

		licenseKey_input.setMultiLine(false);
		licenseKey_input.setReturnKeyStartsNewLine(false);
		licenseKey_input.setTextToShowWhenEmpty("Enter your license key...", customLookAndFeel.getColourFromID("dullText"));
		addAndMakeVisible(licenseKey_input);

		activateLicense_button.setButtonText("Activate License");
		activateLicense_button.onClick = [this] { tryActivateLicense(username_input.getText(), licenseKey_input.getText()); };
		addAndMakeVisible(activateLicense_button);
	}
	else
	{
		// If the license is activated, show a message and option to deactivate
		activationIndicator_label.setText("You have an active license on this computer.", juce::NotificationType::dontSendNotification);
		activationIndicator_label.applyColourToAllText(customLookAndFeel.getColourFromID("active"));

		deactivateLicense_button.setButtonText("Deactivate License");
		deactivateLicense_button.onClick = [this] { licenseManager.clearActivation(); };
		addAndMakeVisible(deactivateLicense_button);
	}

	// Set up the common elements of the label
	activationIndicator_label.setColour(juce::TextEditor::outlineColourId, juce::Colours::transparentWhite.withAlpha(0.0f));
	activationIndicator_label.setReadOnly(true);
	activationIndicator_label.setMultiLine(true);
	activationIndicator_label.setScrollbarsShown(false);
	activationIndicator_label.setCaretVisible(false);
	activationIndicator_label.setInterceptsMouseClicks(false, false);
	activationIndicator_label.setJustification(juce::Justification::centred);
	addAndMakeVisible(activationIndicator_label);
}

LicenseActivationComponent::~LicenseActivationComponent()
{
	setLookAndFeel(nullptr);
}

void LicenseActivationComponent::paint(juce::Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void LicenseActivationComponent::resized()
{
	auto bounds = getLocalBounds().reduced(spacing.margin);

	juce::Grid grid;

	// Add rows to the grid
	grid.templateColumns = { juce::Grid::Fr(1) };
	grid.templateRows = {
		juce::Grid::Fr(2),
		juce::Grid::Fr(1),
		juce::Grid::Fr(1),
		juce::Grid::Fr(1)
	};

	grid.setGap(juce::Grid::Px(spacing.margin));

	// Add items to the grid
	grid.items = {
		juce::GridItem(activationIndicator_label).withArea(1, 1, 1, 1),
		juce::GridItem(username_input).withArea(2, 1, 2, 1),
		juce::GridItem(licenseKey_input).withArea(3, 1, 3, 1),
		juce::GridItem(activateLicense_button).withArea(4, 1, 4, 1),
		juce::GridItem(deactivateLicense_button).withArea(4, 1, 4, 1),
	};

	grid.performLayout(bounds);
}

void LicenseActivationComponent::tryActivateLicense(const juce::String& username, const juce::String& licenseKey)
{
	if (username.isEmpty() || licenseKey.isEmpty())
	{
		juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::AlertIconType::WarningIcon, "License Activation", "Please enter a username and license key.", "OK");
		return;
	}

	if (username.containsAnyOf("\n\r\t") || licenseKey.containsAnyOf("\n\r\t"))
	{
		juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::AlertIconType::WarningIcon, "License Activation", "Please enter a username and license key.", "OK");
		return;
	}

	// Sanitise the inputs, including removing dashes from license key
	username.trim();
	licenseKey.trim();
	juce::String filteredLicenseKey;
	for (auto c : licenseKey)
	{
		if (std::isalnum(c)) filteredLicenseKey += c; // Remove all non-alphanumeric characters from the license key
	}

	// Attempt to activate the license
	if (licenseManager.activateLicense(username, filteredLicenseKey))
	{
		juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::AlertIconType::InfoIcon, "License Activation", "License activated successfully!", "OK");
	}
	else
	{
		juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::AlertIconType::WarningIcon, "License Activation", "License activation failed. Please try again.", "OK");
	}
}