#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "BaseView.h"

#include "drivers/MouseDriver.h"
#include "drivers/TuioDriver.h"
#include "TouchManager.h"

namespace bluecadet {
namespace utils {

typedef std::shared_ptr<class BaseApp> BaseAppRef;

class BaseApp : public ci::app::App {

public:

	BaseApp();
	virtual ~BaseApp();

	void setup() override;
	void update() override;
	void draw() override;

protected:
	views::BaseViewRef			mRootView;
	touch::drivers::MouseDriver	mMouseDriver;
	touch::drivers::TuioDriver	mTuioDriver;

	double						mLastFrameTime;

};

}
}