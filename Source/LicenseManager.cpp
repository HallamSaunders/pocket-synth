/*
  ==============================================================================

    LicenseManager.cpp
    Created: 4 Mar 2025 9:13:21pm
    Author:  Hallam Saunders

  ==============================================================================
*/

#include "LicenseManager.h"

// Constructor and destructor
LicenseManager::LicenseManager()
{
    // Initialise activation file location
	activationDirectory.createDirectory(); // Create the directory if it doesn't exist

    // Initialise file location for where the activation is (or will be if it doesn't exist)
	activationFile = activationDirectory.getChildFile(juce::String(pluginID) + "Activation.dat");

	// Initialise hardware ID as combination of device name and first calculated device identifier
    hardwareID = juce::SystemStats::getComputerName() + juce::SystemStats::getDeviceIdentifiers()[0];
}

LicenseManager::~LicenseManager()
{

}

bool LicenseManager::isActivated()
{
	// Decrypt and load activation data from file
    const juce::String activationData = loadAndDecryptActivationData();

    if (activationData.isEmpty()) return false;

    // Activation data from the file will be in JSON format, parse it
    juce::var parsedData = juce::JSON::parse(activationData);

    if (parsedData.isObject())
    {
		// Create new JSON object to store parsed activation data
        juce::DynamicObject* parsedDataJsonObject = parsedData.getDynamicObject();

        // Check if status is success
        if (parsedDataJsonObject->hasProperty("status") && parsedDataJsonObject->getProperty("status").toString() != "success")
        {
			juce::Logger::outputDebugString("LICENSE MANAGER: license status from activation file is not success.");
            return false;
        }

        // Check if the hardware ID matches computed hardware ID
        if (parsedDataJsonObject->hasProperty("hardware_id") && parsedDataJsonObject->getProperty("hardware_id").toString() != (hardwareID + createSalt()))
        {
			juce::Logger::outputDebugString("LICENSE MANAGER: hardware ID from activation file does not match computed value.");
            return false;
        }

		juce::Logger::outputDebugString("LICENSE MANAGER: all fields from activation file match, license is activated!");
		return true;
	}

	return false;
}

bool LicenseManager::activateLicense(const juce::String& username, const juce::String& licenseKey)
{
	juce::Logger::outputDebugString("LICENSE MANAGER: attempting to activate license with key " + licenseKey);

    // Construct JSON request body from the input variables
    juce::URL url(activationFullURL);
    juce::var requestData = new juce::DynamicObject();
    requestData.getDynamicObject()->setProperty("username", username);
    requestData.getDynamicObject()->setProperty("product_id", pluginID);
    requestData.getDynamicObject()->setProperty("license_key", licenseKey);
    juce::String requestBody = juce::JSON::toString(requestData);
    std::unique_ptr<juce::InputStream> stream(url
        .withPOSTData(requestBody)
        .createInputStream(juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inPostData)
        .withExtraHeaders("Content-Type: application/json")));

	juce::Logger::outputDebugString("LICENSE MANAGER: constructed request body: " + requestBody);

    if (stream == nullptr) {
		juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::AlertIconType::WarningIcon, "Network Error", "Could not connect to the license server. Please check your internet connection and try again.");
        return false;
    }

    juce::String response = stream->readEntireStreamAsString();
    juce::var jsonResponse = juce::JSON::parse(response);

    if (jsonResponse.isObject() && jsonResponse.hasProperty("status") && jsonResponse["status"].toString() == "success")
    {
		juce::Logger::outputDebugString("LICENSE MANAGER: license activated successfully!");
        saveAndEncryptActivationData(response);
		notifyLicenseActivated();
        return true;
    }

	juce::Logger::outputDebugString("LICENSE MANAGER: license activation failed!");
    return false; // Some error with the request
}

void LicenseManager::saveAndEncryptActivationData(const juce::String& activationData)
{
    if (activationFile.create().wasOk())
    {
        // Parse the activation data from JSON
		const juce::var parsedData = juce::JSON::parse(activationData);

		// Create new JSON object to store the modified activation data
		juce::var fileData = new juce::DynamicObject();

        if (parsedData.isObject())
        {
			// Iterate through each of the key-value pairs in the parsed JSON object
            juce::DynamicObject* parsedDataJsonObject = parsedData.getDynamicObject();
            for (auto& key : parsedDataJsonObject->getProperties())
            {
				// Set these key-value pairs in the new JSON object
                const juce::String keyName = key.name.toString();
                const juce::var value = key.value.toString();
                fileData.getDynamicObject()->setProperty(keyName, value.toString());
            }

			// Append the hardware ID to the new JSON object
            fileData.getDynamicObject()->setProperty("hardware_id", hardwareID);

			juce::Logger::outputDebugString("LICENSE MANAGER: activation data saved: " + juce::JSON::toString(fileData));
        }
        else
        {
			juce::Logger::writeToLog("LICENSE MANAGER: error parsing JSON activation data.");
        }

		// Convert file data to memory block for encryption
		juce::MemoryBlock dataBlock(activationData.toRawUTF8(), activationData.getNumBytesAsUTF8());

		// Create Blowfish encryption object
        juce::BlowFish blowfish = createBlowFishObject();

		// Encrypt the data
        juce::MemoryBlock encryptedData(dataBlock);
		blowfish.encrypt(encryptedData);

		// Write the encrypted data to the file
        juce::FileOutputStream outputStream(activationFile);
        if (outputStream.openedOk())
        {
            outputStream.write(encryptedData.getData(), encryptedData.getSize());
			outputStream.flush();
        }
    }
}

juce::String LicenseManager::loadAndDecryptActivationData()
{
    juce::String result;

    if (activationFile.existsAsFile())
    {
        // Read encrypted data from file if it exists
		juce::MemoryBlock encryptedData;
        juce::FileInputStream inputStream(activationFile);

		if (inputStream.openedOk())
        {
			// Read the encrypted data into a memory block ready for decryption
			inputStream.readIntoMemoryBlock(encryptedData);

			// Create Blowfish decryption object (which will, by definition, match the encryption object)
            juce::BlowFish blowfish = createBlowFishObject();

			// Decrypt the data
			juce::MemoryBlock decryptedData(encryptedData);
			blowfish.decrypt(decryptedData);

			// Convert decrypted data to string
			result = juce::String::createStringFromData(decryptedData.getData(), static_cast<int>(decryptedData.getSize()));
		}
    }

    return result;
}

void LicenseManager::clearActivation()
{
    if (activationFile.existsAsFile())
    {
        activationFile.deleteFile();
    }
    if (!activationFile.existsAsFile())
    {
		notifyLicenseDeactivated();
    }
}

juce::String LicenseManager::createSalt()
{
	// Create a salt from the company name, plugin ID, release year, and CPU info
    const juce::String companyName = "BitshiftDevices";
	const juce::String pluginReleaseYear = "2025";
	const juce::String cupInfo = juce::SystemStats::getCpuModel();

	// Hash the salt elements and return the result
	juce::MD5 md5((companyName + pluginID + pluginReleaseYear + cupInfo).toUTF8());
	return md5.toHexString();
}

juce::BlowFish LicenseManager::createBlowFishObject()
{
	// Create a BlowFish object with the encryption key derived from the hardware ID and salt
    juce::String saltedHardwareID = hardwareID + createSalt();
    juce::MD5 md5(saltedHardwareID.toUTF8());
    juce::String encryptionKey = md5.toHexString();
	return juce::BlowFish(encryptionKey.toRawUTF8(), encryptionKey.getNumBytesAsUTF8());
}