/*
  ==============================================================================

    LicenseManager.h
    Created: 4 Mar 2025 9:13:27pm
    Author:  Hallam Saunders

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class LicenseManager
{
public:
    LicenseManager();
    ~LicenseManager();

    bool isActivated();
    bool activateLicense(const juce::String& username, const juce::String& licenseKey);

	juce::File getActivationDirectory() { return activationDirectory; }
    juce::String getPluginID() { return pluginID; }

	// Listener class for license activation
	class Listener
	{
	public:
		virtual ~Listener() = default;
		virtual void onLicenseActivated() = 0;
		virtual void onLicenseDeactivated() = 0;
	};

	// Functions to add and remove listeners
	void addListener(Listener* listener) { listeners.add(listener); }
    void removeListener(Listener* listener) { listeners.removeFirstMatchingValue(listener); }

    void clearActivation();

private:
	// Activation file location
	juce::File activationDirectory = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getChildFile(juce::String(companyID)).getChildFile(juce::String(pluginID));
    juce::File activationFile;

    static constexpr const char* activationBaseURL = "http://localhost:8000/";
    static constexpr const char* activationEndpoint = "api/store/licenses/activate/";
    static constexpr const char* pluginID = "PocketSynth";
	static constexpr const char* companyID = "BitshiftDevices";
	const juce::String activationFullURL = juce::String(activationBaseURL) + activationEndpoint;

    // Save, load, and clear activations
    juce::String hardwareID;
    void saveAndEncryptActivationData(const juce::String& activationData);
    juce::String loadAndDecryptActivationData();

	// Encryption and decryption functions
    juce::String createSalt();
    juce::BlowFish createBlowFishObject();

	// Listeners
	juce::Array<Listener*> listeners;
    void notifyLicenseActivated()
    {
		for (auto* listener : listeners)
			listener->onLicenseActivated();
    }
    void notifyLicenseDeactivated()
    {
		for (auto* listener : listeners)
			listener->onLicenseDeactivated();
    }
};