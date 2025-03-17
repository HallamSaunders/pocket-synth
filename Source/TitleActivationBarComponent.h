/*
  ==============================================================================

    TitleActivationBarComponent.h
    Created: 17 Mar 2025 6:05:43pm
    Author:  Hallam Saunders

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "LicenseActivationWindow.h"
#include "CustomLookAndFeel.h"
#include "Spacing.h"
#include "LicenseManager.h"

class TitleActivationBarComponent : public juce::Component
{
public:
	TitleActivationBarComponent(LicenseManager& manager);
	~TitleActivationBarComponent() override;

	void resized() override;

private:
	LicenseManager& licenseManager;
	CustomLookAndFeel customLookAndFeel;
	Spacing spacing;

	// Title components
	juce::Label company_label;
	juce::Label product_label;

	// Licensing components
	void setupActivationLabel();
	juce::Label activationStatus_label;
	juce::TextButton launchActivationWindow_button;
	std::unique_ptr<LicenseActivationWindow> licenseActivationWindow;

	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> active_toggleButtonAttachment;
	juce::ToggleButton active_toggleButton;
};
