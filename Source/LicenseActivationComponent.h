/*
  ==============================================================================

    LicenseActivationComponent.h
    Created: 9 Mar 2025 3:17:20pm
    Author:  Hallam Saunders

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "LicenseManager.h"
#include "Spacing.h"
#include "CustomTextEditor.h"

class LicenseActivationComponent : public juce::Component
{
public:
	LicenseActivationComponent(LicenseManager& manager);
	~LicenseActivationComponent() override;

	void paint(juce::Graphics& g) override;
	void resized() override;

private:
	CustomLookAndFeel customLookAndFeel;
	LicenseManager& licenseManager;
	Spacing spacing;

	juce::TextEditor activationIndicator_label;
    juce::TextEditor username_input;
    juce::TextEditor licenseKey_input;
    juce::TextButton activateLicense_button;
	juce::TextButton deactivateLicense_button;

	void tryActivateLicense(const juce::String& username, const juce::String& licenseKey);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LicenseActivationComponent)
};