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

typedef std::shared_ptr<class SettingsManager> SettingsManagerRef;

class SettingsManager {
public:

	SettingsManager();
	~SettingsManager();

	//! Singleton
	static SettingsManagerRef getInstance() {
		static SettingsManagerRef instance = nullptr;
		if (!instance) instance = SettingsManagerRef(new SettingsManager());
		return instance;
	}
 
	/*!
	Load the standard settings that all apps share from the block

	Supported arguments:
		debug=[true/false]
		size=w,h
		fullscreen=[true/false]
		borderless=[true/false]
		vsync=[true/false]
		console=[tru/false]
		cursor=[true/false] or mouse=[true/false]
	*/
	void setup(const ci::fs::path& jsonPath, ci::app::App::Settings* appSettings);

	//! Returns the app params, creates new params 
	ci::params::InterfaceGlRef getParams();
	
	//! General
	bool			mConsoleWindowEnabled;
	//! Graphics
	bool			mVerticalSync;
	//! Debug
	bool			mDebugMode;
	bool			mDebugDrawTouches;
	bool			mDebugDrawScreenLayout;
	bool			mDebugFullscreen;
	bool			mDebugBorderless;
	ci::ivec2		mDebugWindowSize;
	bool			mShowMouse;
	bool			mDrawMinimap;

	//! Analytics
	std::string		mAnalyticsAppName;
	std::string		mAnalyticsTrackingId;
	std::string		mAnalyticsClientId;

protected:
	virtual void applySettings(ci::app::App::Settings* appSettings);
	virtual void parseCommandLineArgs(const std::vector<std::string>& args);
	virtual void parseArgumentsMap(const std::map<std::string, std::string>& argsMap);

	//! Helpers to get string from primitive types and strings since we can't call to_string on strings
	template <typename T> std::string castToString(T* target) { return to_string(*target); }
	template <> std::string castToString<std::string>(std::string* target) { return *target; }


	//! Helpers
	template <typename T>
	void setFieldFromJson(T* target, const std::string& jsonFieldName, const ci::JsonTree& json) {
		try {
			if (!json.hasChild(jsonFieldName)) {
				//! Abort if the settings value couldn't be found
				console() << "SettingsManager: Could not find settings value for field name '" << jsonFieldName << "' in json file" << endl;
				return;
			}
			*target = json.getValueForKey<T>(jsonFieldName);
			console() << "SettingsManager: Set '" << jsonFieldName << "' to '" << castToString(target) << "' from json file" << endl;

		} catch (Exception e) {
			console() << "SettingsManager: Could not set '" << jsonFieldName << "' from json file: " << e.what() << endl;
		}
	}
};

}
}