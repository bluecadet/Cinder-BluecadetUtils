#include "SettingsManager.h"

#include <algorithm>
#include <string>	

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace utils {

std::map<std::string, std::string> SettingsManager::mCommandLineArgs;

// General
bool  SettingsManager::mConsoleWindowEnabled = true;
float SettingsManager::mMaxTaskProcessingTime = 0.05f / (float)SETTINGS_FPS; // % of frame time allocated to asset processing etc
// Graphics
bool  SettingsManager::mVerticalSync = true;
// Debug
float SettingsManager::mDebugScale = 1.0f;
bool  SettingsManager::mDebugMode = true;
bool  SettingsManager::mDrawMinimap = true;
bool  SettingsManager::mDrawDisplayOutlines = true;
bool  SettingsManager::mDebugDrawTouches = false;
bool  SettingsManager::mDebugDrawScreenLayout = false;
bool  SettingsManager::mDebugFullscreen = true;
bool  SettingsManager::mDebugBorderless = true;
bool  SettingsManager::mShowMouse = false;
ci::vec2 SettingsManager::mDebugTranslation = vec2(0);
ci::ivec2 SettingsManager::mDebugWindowSize = ivec2(0);
// Reset
float SettingsManager::mReset_timeToWarning = 60.0f;
float SettingsManager::mReset_timeWarningIsOnScreen = 15.0f;
// Analytics
std::string SettingsManager::mAnalyticsAppName = "";
std::string	SettingsManager::mAnalyticsTrackingId = "";
std::string	SettingsManager::mAnalyticsClientId = "";

// Initialization
SettingsManager::SettingsManager() {}
SettingsManager::~SettingsManager() {}

void SettingsManager::parseCommandLineArgs(const std::vector<std::string>& args) {
	// Process command line args
	if (!args.empty()) {
		for (const std::string carg : args) {
			std::string arg = carg;
			int splitIndex = (int)arg.find_first_of("=");
			if (splitIndex != std::string::npos) {
				std::transform(arg.begin(), arg.end(), arg.begin(), ::tolower);
				std::string key = arg.substr(0, splitIndex);
				std::string value = arg.substr(splitIndex + 1, arg.size() - splitIndex - 1);
				mCommandLineArgs[key] = value;
			}
		}

		parseArguments(mCommandLineArgs);
	}
}

// Pull in the shared/standard app settings JSON
void SettingsManager::loadStandardAppSettings(const ci::fs::path& path) {

	//If the path exists, load it
	if (!fs::exists(path)) {
		console() << "SettingsManager: ERROR: Settings file does not exist at '" << path << "'" << std::endl;
		return;
	}

	console() << "SettingsManager: Loading settings from '" << path << "'" << endl;

	try {
		JsonTree appSettingsDoc(loadFile(path));
		const auto& args = mCommandLineArgs;

		// General
		if (!args.count("console")) setFieldFromJson(&mConsoleWindowEnabled, "settings.consoleWindowEnabled", appSettingsDoc);
		setFieldFromJson(&mMaxTaskProcessingTime, "settings.maxTaskProcessingTime", appSettingsDoc);

		// Graphics
		if (!args.count("vsync")) setFieldFromJson(&mVerticalSync, "settings.graphics.verticalSync", appSettingsDoc);

		// Debug
		setFieldFromJson(&mDebugScale, "settings.debug.scale", appSettingsDoc);
		if (!args.count("debug")) setFieldFromJson(&mDebugMode, "settings.debug.debugMode", appSettingsDoc);
		setFieldFromJson(&mDrawMinimap, "settings.debug.drawMinimap", appSettingsDoc);
		setFieldFromJson(&mDrawDisplayOutlines, "settings.debug.drawDisplayOutlines", appSettingsDoc);
		setFieldFromJson(&mDebugDrawTouches, "settings.debug.drawDebugTouches", appSettingsDoc);
		setFieldFromJson(&mDebugDrawScreenLayout, "settings.debug.drawDebugScreenLayout", appSettingsDoc);
		setFieldFromJson(&mDebugFullscreen, "settings.debug.debugFullscreen", appSettingsDoc);
		setFieldFromJson(&mDebugBorderless, "settings.debug.debugBorderless", appSettingsDoc);
		if (!args.count("cursor") && !args.count("mouse")) setFieldFromJson(&mShowMouse, "settings.debug.showMouse", appSettingsDoc);
		
		// Reset
		setFieldFromJson(&mReset_timeToWarning, "settings.reset.timeToWarning", appSettingsDoc);
		setFieldFromJson(&mReset_timeWarningIsOnScreen, "settings.reset.timeWarningIsOnScreen", appSettingsDoc);

		// Analytics
		setFieldFromJson(&mAnalyticsAppName, "settings.analytics.appName", appSettingsDoc);
		setFieldFromJson(&mAnalyticsTrackingId, "settings.analytics.trackingID", appSettingsDoc);
		setFieldFromJson(&mAnalyticsClientId, "settings.analytics.clientID", appSettingsDoc);
	}
	catch (Exception e) {
		console() << "SettingsManager: ERROR: Could not load settings json: " << e.what() << endl;
	}
}

void SettingsManager::parseArguments(const std::map<std::string, std::string>& argsMap) {

	// default window size to main display size
	mDebugWindowSize = Display::getMainDisplay()->getSize();

	// process command line args
	for (std::pair<std::string, std::string> arg : argsMap) {
		
		if (arg.first == "debug") {
			mDebugMode = arg.second == "true";
		}

		else if (arg.first == "size") {
			int commaIndex = (int)arg.second.find(",");
			if (commaIndex != string::npos) {
				string wStr = arg.second.substr(0, commaIndex);
				string hStr = arg.second.substr(commaIndex + 1, arg.second.size() - commaIndex - 1);
				mDebugWindowSize = ivec2(stoi(wStr), stoi(hStr));
			}
		}

		else if (arg.first == "fullscreen") {
			mDebugFullscreen = arg.second == "true";
		}

		else if (arg.first == "borderless") {
			mDebugBorderless = arg.second == "true";
		}

		else if (arg.first == "vsync") {
			mVerticalSync = arg.second == "true";
		}

		else if (arg.first == "console") {
			mConsoleWindowEnabled = arg.second == "true";
		}

		else if (arg.first == "cursor" || arg.first == "mouse") {
			mShowMouse = arg.second == "true";
		}
	}
}

ci::params::InterfaceGlRef SettingsManager::getParams() {
	static ci::params::InterfaceGlRef params = nullptr;
	if (!params) {
		params = ci::params::InterfaceGl::create("Settings", ci::ivec2(250, 500));

		params->addParam("Show Cursor", &mShowMouse).updateFn([&] { mShowMouse ? ci::app::AppBase::get()->showCursor() : ci::app::AppBase::get()->hideCursor(); }).key("m");

		params->addText("Viewport");
		params->addParam("Scale", &mDebugScale);
		params->addParam("Translate X", &mDebugTranslation.x, "min=-10000.0 step=1");
		params->addParam("Translate Y", &mDebugTranslation.y, "min=-10000.0 step=1");
		params->addParam("Minimap", &mDrawMinimap);

		params->addSeparator();
		params->addText("Timing");
		params->addParam("Reset - time to warning", &mReset_timeToWarning);
		params->addParam("Reset - warning displayed time", &mReset_timeWarningIsOnScreen);
		params->addParam("Max Asset Processing Duration", &mMaxTaskProcessingTime);

		params->addSeparator();
		params->addText("Debugging");
		params->addParam("Show Touches", &mDebugDrawTouches);
		params->addParam("Show Layout", &mDebugDrawScreenLayout);
	}
	return params;
}

}
}