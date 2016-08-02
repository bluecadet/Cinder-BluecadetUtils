#pragma once
#include "cinder/Json.h"
#include "cinder/app/App.h"
#include "cinder/params/Params.h"

namespace bluecadet {
namespace utils {

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
		console=[true/false]
		cursor=[true/false] or mouse=[true/false]
	*/
	void setup(const ci::fs::path& jsonPath, ci::app::App::Settings* appSettings);

	//! Returns the app params, creates new params 
	ci::params::InterfaceGlRef getParams();

	//! General
	bool			mConsoleWindowEnabled;
	int				mFps;
	std::string		mAppVersion;

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

	//! Get the value of field from within the json
	template <typename T>
	T getField(const std::string& field) {
		try {
			if (!mAppSettingsDoc.hasChild(field)) {
				//! Abort if the settings value couldn't be found
				console() << "SettingsManager: Could not find settings value for field name '" << field << "' in json file" << endl;
				return T();
			}
			return mAppSettingsDoc.getValueForKey<T>(field);
		}
		catch (Exception e) {
			console() << "SettingsManager: Could not find '" << field << "' in json file: " << e.what() << endl;
			return T();
		}
	};

protected:
	//! Set fields within the settings manager class if the setting is defined in the json
	template <typename T>
	void updatePropertyFromField(T* target, const std::string& jsonFieldName) {
		try {
			if (!mAppSettingsDoc.hasChild(jsonFieldName)) {
				//! Abort if the settings value couldn't be found
				console() << "SettingsManager: Could not find settings value for field name '" << jsonFieldName << "' in json file" << endl;
				return;
			}
			*target = mAppSettingsDoc.getValueForKey<T>(jsonFieldName);
			console() << "SettingsManager: Set '" << jsonFieldName << "' to '" << castToString(target) << "' from json file" << endl;
		}
		catch (Exception e) {
			console() << "SettingsManager: Could not set '" << jsonFieldName << "' from json file: " << e.what() << endl;
		}
	}
	
	virtual void applySettings(ci::app::App::Settings* appSettings);
	virtual void parseCommandLineArgs(const std::vector<std::string>& args);
	virtual void parseArgumentsMap(const std::map<std::string, std::string>& argsMap);

	//! Helpers to get string from primitive types and strings since we can't call to_string on strings
	template <typename T> std::string castToString(T* target) { return to_string(*target); }
	template <> std::string castToString<std::string>(std::string* target) { return *target; }

	//! Base appSettings json
	ci::JsonTree mAppSettingsDoc;
};

}
}