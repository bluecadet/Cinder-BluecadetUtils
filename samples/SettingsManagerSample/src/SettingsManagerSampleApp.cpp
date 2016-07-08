#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "SampleSettingsManager.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class SettingsManagerSampleApp : public App {
  public:
	static void prepareSettings(App::Settings *settings);

	void setup() override;
	void update() override;
	void draw() override;

private:

};

void SettingsManagerSampleApp::prepareSettings(App::Settings *settings) {
	
	//! Load settings

	// Parse whatever command line arguments the app is starting with
	//  -can set these in Visual Studio in Properties > Debugging > Command Arguments (don't forget to do this for Debug and Release)
	SampleSettingsManager::parseCommandLineArgs(settings->getCommandLineArgs());
	// Load the JSON that holds the settings that are consistent between most projects. Whatever variables are not set with command line arguments will be set here.
	SampleSettingsManager::loadStandardAppSettings(ci::app::getAssetPath("standardAppSettings.json"));
	// Load the JSON that holds any settings specific to this project
	SampleSettingsManager::loadProjectSpecificAppSettings(ci::app::getAssetPath("projectSpecificAppSettings.json"));
	
	//! Apply Settings

	// Separate console window (shows "cout" logs )
	if (SampleSettingsManager::mConsoleWindowEnabled) settings->setConsoleWindowEnabled(true);
	// Set framerate
	settings->setFrameRate(SETTINGS_FPS);
}

void SettingsManagerSampleApp::setup(){
	
}

void SettingsManagerSampleApp::update(){
}

void SettingsManagerSampleApp::draw(){
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP(
	SettingsManagerSampleApp,
	RendererGl(RendererGl::Options().msaa(8)), // enable MSAA to smoothly animate text and thin lines
	SettingsManagerSampleApp::prepareSettings
)
