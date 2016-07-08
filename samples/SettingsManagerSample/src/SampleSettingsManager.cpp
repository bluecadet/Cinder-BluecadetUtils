#include "SampleSettingsManager.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace bluecadet::utils;

// Defaults
bool	SampleSettingsManager::mTestBool = true;
string	SampleSettingsManager::mTestString = "";
int		SampleSettingsManager::mTestInt = -1;

SampleSettingsManager::SampleSettingsManager() : SettingsManager(){
}

SampleSettingsManager::~SampleSettingsManager(){
}

void SampleSettingsManager::setup() {

}

void SampleSettingsManager::loadProjectSpecificAppSettings(const ci::fs::path& path) {

	//If the path exists, load it
	if (!fs::exists(path)) {
		console() << "SettingsManager: ERROR: Project specific settings file does not exist at '" << path << "'" << std::endl;
		return;
	}

	console() << "SettingsManager: Loading settings from '" << path << "'" << endl;

	try {
		JsonTree appSettingsDoc(loadFile(path));

		setFieldFromJson(&mTestBool, "settings.testBool", appSettingsDoc);
		setFieldFromJson(&mTestString, "settings.testString", appSettingsDoc);
		setFieldFromJson(&mTestInt, "settings.testInt", appSettingsDoc);

	}
	catch (Exception e) {
		console() << "SettingsManager: ERROR: Could not load settings json: " << e.what() << endl;
	}

}