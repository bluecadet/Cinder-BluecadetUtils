#include "BaseApp.h"

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

void BaseApp::setup()
{
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
}

}
}