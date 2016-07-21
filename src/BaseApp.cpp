#include "BaseApp.h"
#include "SettingsManager.h"

#include "ScreenLayout.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace utils {

BaseApp::BaseApp() :
	ci::app::App(),
	mLastFrameTime(0),
	mRootView(views::BaseViewRef(new views::BaseView()))
{
}

BaseApp::~BaseApp() {
}

void BaseApp::prepareSettings(ci::app::App::Settings* settings)
{
	// Init settings manager
	fs::path appSettingsPath = ci::app::getAssetPath("appSettings.json");
	SettingsManager::getInstance()->setup(appSettingsPath, settings);

	ScreenLayout::getInstance()->setup();
}

void BaseApp::setup()
{

	// Set up settings
	if (SettingsManager::getInstance()->mShowMouse) {
		showCursor();
	} else {
		hideCursor();
	}

	// Set up graphics
	gl::enableVerticalSync(SettingsManager::getInstance()->mVerticalSync);
	gl::enableAlphaBlending();

	// Set up touches
	mMouseDriver.connect();
	mTuioDriver.connect();
}

void BaseApp::update()
{
	const double currentTime = getElapsedSeconds();
	const double deltaTime = mLastFrameTime == 0 ? 0 : currentTime - mLastFrameTime;
	mLastFrameTime = currentTime;

	touch::TouchManager::getInstance()->update(mRootView);
	mRootView->updateScene(deltaTime);
}

void BaseApp::draw()
{
	gl::clear(Color(0, 0, 0));
	mRootView->drawScene();

	if(SettingsManager::getInstance()->mDebugDrawScreenLayout)
		ScreenLayout::getInstance()->drawDisplayBounds();
}

void BaseApp::keyDown(KeyEvent event)
{
	switch (event.getCode()) {
		case KeyEvent::KEY_q:
			quit();
			break;
	}
}

}
}