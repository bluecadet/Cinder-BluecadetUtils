//+---------------------------------------------------------------------------
//  Bluecadet Interactive 2016
//  Contents: 
//  Comments: Instead of "SampleSettingsManager" we could name this specific to the project, 
//				for example "CTSettingsManager" or "NasmSettingsManager"
//----------------------------------------------------------------------------
#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "SettingsManager.h"

typedef std::shared_ptr<class SampleSettingsManager> SampleSettingsManagerRef;

class SampleSettingsManager : public bluecadet::utils::SettingsManager {

public:
	SampleSettingsManager();
	~SampleSettingsManager();

	void setup();

	static void loadProjectSpecificAppSettings(const ci::fs::path& path);

	static bool			mTestBool;
	static std::string	mTestString;
	static int			mTestInt;

private:

	//! Properties specific to this app

};