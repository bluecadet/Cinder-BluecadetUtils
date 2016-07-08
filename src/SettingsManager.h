#pragma once
#include "cinder/Json.h"
#include "cinder/app/App.h"
#include "cinder/params/Params.h"

namespace bluecadet {
namespace utils {

#define SETTINGS_APP_VERSION "alpha-1.0"

#define SETTINGS_APP_WIDTH 7560		//! Width of app
#define SETTINGS_APP_HEIGHT 5760	//! Height of app
#define SETTINGS_DISPLAY_WIDTH 1080 //! Width of single display
#define SETTINGS_DISPLAY_HEIGHT 1920//! Height of single display
#define SETTINGS_DISPLAY_NUM_ROWS 1 //! Total number of screens vertically
#define SETTINGS_DISPLAY_NUM_COLS 5 //! Total number of screens horizontally

#define SETTINGS_FPS 60

class SettingsManager {
public:

	SettingsManager();
	~SettingsManager();

	/**
	Supported arguments:
		debug=[true/false]
		size=w,h
		fullscreen=[true/false]
		borderless=[true/false]
		vsync=[true/false]
		console=[tru/false]
		cursor=[true/false] or mouse=[true/false]
	*/
	//! Parse command line arguments passed in when starting app
	static void parseCommandLineArgs(const std::vector<std::string>& args);

	//! Load the standard settings that all apps share from the block
	static void loadStandardAppSettings(const ci::fs::path& path);

	//! Returns the app params, creates new params 
	static ci::params::InterfaceGlRef getParams();
	
	//! General
	static bool			mConsoleWindowEnabled;
	static float		mMaxTaskProcessingTime;
	//! Graphics
	static bool			mVerticalSync;
	//! Debug
	static float		mDebugScale;
	static bool			mDebugMode;
	static bool			mDrawMinimap;
	static bool			mDrawDisplayOutlines;
	static bool			mDebugDrawTouches;
	static bool			mDebugDrawScreenLayout;
	static bool			mDebugFullscreen;
	static bool			mDebugBorderless;
	static bool			mShowMouse;
	static ci::vec2		mDebugTranslation;
	static ci::ivec2	mDebugWindowSize;
	//! Reset
	static float		mReset_timeToWarning;
	static float		mReset_timeWarningIsOnScreen;
	//! Analytics
	static std::string	mAnalyticsAppName;
	static std::string	mAnalyticsTrackingId;
	static std::string	mAnalyticsClientId;

	//! Helpers
	template <typename T>
	static void setFieldFromJson(T* target, const std::string& jsonFieldName, const ci::JsonTree& json) {
		try {
			if (!json.hasChild(jsonFieldName)) {
				//! Abort if the settings value couldn't be found
				console() << "SettingsManager: Could not find settings value for field name '" << jsonFieldName << "' in json file" << endl;
				return;
			}
			*target = json.getValueForKey<T>(jsonFieldName);
			console() << "SettingsManager: Set '" << jsonFieldName << "' to '" << castToString(target) << "' from json file" << endl;

		}
		catch (Exception e) {
			console() << "SettingsManager: Could not set '" << jsonFieldName << "' from json file: " << e.what() << endl;
		}
	}

private:

	
	static void parseArguments(const std::map<std::string, std::string>& argsMap);
	static std::map<std::string, std::string> mCommandLineArgs;

	//! Helpers to get string from primitive types and strings since we can't call to_string on strings
	template <typename T> static std::string castToString(T* target) { return to_string(*target); }
	template <> static std::string castToString<std::string>(std::string* target) { return *target; }
};

}
}