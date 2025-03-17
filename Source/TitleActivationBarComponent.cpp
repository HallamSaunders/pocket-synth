/*
  ==============================================================================

    TitleActivationBarComponent.cpp
    Created: 17 Mar 2025 6:05:43pm
    Author:  Hallam Saunders

  ==============================================================================
*/

#include "TitleActivationBarComponent.h"

TitleActivationBarComponent::TitleActivationBarComponent(LicenseManager& manager) : licenseManager(manager)
{
	setLookAndFeel(&customLookAndFeel);

	// Title components
	company_label.setText("BitshiftDevices", juce::dontSendNotification);
	company_label.setJustificationType(juce::Justification::left);
	addAndMakeVisible(company_label);
	product_label.setText("BitshiftSynth", juce::dontSendNotification);
	product_label.setJustificationType(juce::Justification::left);
	addAndMakeVisible(product_label);

	// Licensing components
	setupActivationLabel();
	launchActivationWindow_button.setButtonText("Activation Settings");
	launchActivationWindow_button.onClick = [this]()
		{
			if (licenseActivationWindow != nullptr) licenseActivationWindow.reset(); // If one already exists, reset it
			licenseActivationWindow = std::make_unique<LicenseActivationWindow>(licenseManager);
		};
	addAndMakeVisible(launchActivationWindow_button);
}

TitleActivationBarComponent::~TitleActivationBarComponent()
{
	setLookAndFeel(nullptr);
}

void TitleActivationBarComponent::setupActivationLabel()
{
	if (licenseManager.isActivated())
	{
		activationStatus_label.setColour(juce::Label::textColourId, customLookAndFeel.getColourFromID("active"));
		activationStatus_label.setText("Activated", juce::dontSendNotification);
	}
	else
	{
		activationStatus_label.setColour(juce::Label::textColourId, customLookAndFeel.getColourFromID("inactive"));
		activationStatus_label.setText("Not Activated", juce::dontSendNotification);
	}
	addAndMakeVisible(activationStatus_label);
}

void TitleActivationBarComponent::resized()
{
	auto grid = spacing.getTitleBarGridLayout();

	grid.items = {
		juce::GridItem(company_label)					.withArea(1, 1, 1, 1),
		juce::GridItem(product_label)					.withArea(1, 2, 1, 2),
		juce::GridItem(activationStatus_label)			.withArea(1, 4, 1, 4),
		juce::GridItem(launchActivationWindow_button)	.withArea(1, 5, 1, 5)
	};

	grid.performLayout(getLocalBounds());
}
