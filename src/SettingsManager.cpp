#include "SettingsManager.h"

#include <algorithm>
#include <string>	

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace utils {

// Initialization
SettingsManager::SettingsManager(){

	sAppSettingsDoc = JsonTree();

	// General
	mConsoleWindowEnabled = true;
	mFps = 60;
	mAppVersion = "";
	mAppWidth = 1920;
	mAppHeight = 1080;

	// Graphics
	mVerticalSync = true;
	
	// Debug
	mDebugMode = true;
	mDrawMinimap = true;
	mDebugDrawTouches = false;
	mDebugDrawScreenLayout = false;
	mDebugFullscreen = true;
	mDebugBorderless = true;
	mShowMouse = true;
	mDebugWindowSize = ivec2(0);
	
	// Analytics
	mAnalyticsAppName = "";
	mAnalyticsTrackingId = "";
	mAnalyticsClientId = "";
}
SettingsManager::~SettingsManager() {}

// Pull in the shared/standard app settings JSON
void SettingsManager::setup(const ci::fs::path& jsonPath, ci::app::App::Settings* appSettings) {

	// If the path exists, load it
	if (fs::exists(jsonPath)) {
		console() << "SettingsManager: Loading settings from '" << jsonPath << "'" << endl;

		try {
			DataSourceRef tree = loadFile(jsonPath);
			sAppSettingsDoc = (JsonTree)tree;

			// General
			setFieldFromJson(&mConsoleWindowEnabled, "settings.general.consoleWindowEnabled", sAppSettingsDoc);
			setFieldFromJson(&mFps, "settings.general.FPS", sAppSettingsDoc);
			setFieldFromJson(&mAppVersion, "settings.general.appVersion", sAppSettingsDoc);

			// Graphics
			setFieldFromJson(&mVerticalSync, "settings.graphics.verticalSync", sAppSettingsDoc);

			// Debug
			setFieldFromJson(&mDebugMode, "settings.debug.debugMode", sAppSettingsDoc);
			setFieldFromJson(&mDrawMinimap, "settings.debug.drawMinimap", sAppSettingsDoc);
			setFieldFromJson(&mDebugDrawTouches, "settings.debug.drawTouches", sAppSettingsDoc);
			setFieldFromJson(&mDebugDrawScreenLayout, "settings.debug.drawScreenLayout", sAppSettingsDoc);
			setFieldFromJson(&mDebugFullscreen, "settings.debug.fullscreen", sAppSettingsDoc);
			setFieldFromJson(&mDebugBorderless, "settings.debug.borderless", sAppSettingsDoc);
			setFieldFromJson(&mShowMouse, "settings.debug.showMouse", sAppSettingsDoc);

			// Analytics
			setFieldFromJson(&mAnalyticsAppName, "settings.analytics.appName", sAppSettingsDoc);
			setFieldFromJson(&mAnalyticsTrackingId, "settings.analytics.trackingID", sAppSettingsDoc);
			setFieldFromJson(&mAnalyticsClientId, "settings.analytics.clientID", sAppSettingsDoc);

		} catch (Exception e) {
			console() << "SettingsManager: ERROR: Could not load settings json: " << e.what() << endl;
		}
	} else {
		console() << "SettingsManager: ERROR: Settings file does not exist at '" << jsonPath << "'" << std::endl;
	}

	// Parse arguments from command line
	parseCommandLineArgs(appSettings->getCommandLineArgs());

	// Apply settings
	applySettings(appSettings);
}

void SettingsManager::applySettings(ci::app::App::Settings* appSettings) {

	// Set width and height of app
//	mAppWidth = ScreenLayout::getInstance()->getDisplayWidth()*ScreenLayout::getInstance()->getDisplayTotalRows();
//	mAppHeight = ScreenLayout::getInstance()->getDisplayHeight()*ScreenLayout::getInstance()->getDisplayTotalColumns();

	appSettings->setConsoleWindowEnabled(mConsoleWindowEnabled);
	appSettings->setFrameRate(mFps);
	appSettings->setFullScreen(mDebugFullscreen);

	if (mDebugWindowSize.x > 0 && mDebugWindowSize.y > 0) {
		appSettings->setWindowSize(mDebugWindowSize);
	}

	appSettings->setBorderless(mDebugBorderless);
}

void SettingsManager::parseArgumentsMap(const std::map<std::string, std::string>& argsMap) {

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

void SettingsManager::parseCommandLineArgs(const std::vector<std::string>& args) {
	// Process command line args
	if (!args.empty()) {

		std::map<std::string, std::string> argsMap;

		for (const std::string carg : args) {
			std::string arg = carg;
			int splitIndex = (int)arg.find_first_of("=");
			if (splitIndex != std::string::npos) {
				std::transform(arg.begin(), arg.end(), arg.begin(), ::tolower);
				std::string key = arg.substr(0, splitIndex);
				std::string value = arg.substr(splitIndex + 1, arg.size() - splitIndex - 1);
				argsMap[key] = value;
			}
		}

		parseArgumentsMap(argsMap);
	}
}

ci::params::InterfaceGlRef SettingsManager::getParams() {
	static ci::params::InterfaceGlRef params = nullptr;
	if (!params) {
		params = ci::params::InterfaceGl::create("Settings", ci::ivec2(250, 500));
		params->addParam("Minimap", &mDrawMinimap);
		params->addParam("Show Touches", &mDebugDrawTouches);
		params->addParam("Show Layout", &mDebugDrawScreenLayout);
		params->addParam("Show Cursor", &mShowMouse).updateFn([&] { mShowMouse ? ci::app::AppBase::get()->showCursor() : ci::app::AppBase::get()->hideCursor(); }).key("m");
	}
	return params;
}

}
}