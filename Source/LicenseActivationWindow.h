/*
  ==============================================================================

    LicenseActivationWindow.h
    Created: 9 Mar 2025 3:17:30pm
    Author:  Hallam Saunders

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "LicenseActivationComponent.h"
#include "LicenseManager.h"

class LicenseActivationWindow : public juce::DocumentWindow,
								public LicenseManager::Listener
{ 
public:
	LicenseActivationWindow(LicenseManager& manager)
		: DocumentWindow("License Settings", juce::Desktop::getInstance().getDefaultLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId), DocumentWindow::closeButton),
		licenseManager(manager)
	{
		licenseManager.addListener(this);
		setContentOwned(std::make_unique<LicenseActivationComponent>(licenseManager).release(), true);
		centreWithSize(300, 180);
		setResizable(false, false);
		setVisible(true);
	}

	~LicenseActivationWindow() override
	{
		licenseManager.removeListener(this);
		setLookAndFeel(nullptr);
	}

	void closeButtonPressed() override
	{
		setVisible(false);
	}

	void onLicenseActivated() override
	{
		setVisible(false); // Do not delete the window, just hide it, since it is managed by a unique_ptr in editor which handles its deletion when it leaves scope
	}

	void onLicenseDeactivated() override
	{
		// ... may carry some functionality in future. For now it's just here to satisfy the pure virtual nature of the function
	}

private:
	LicenseManager& licenseManager;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LicenseActivationWindow)
};