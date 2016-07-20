#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "BaseApp.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace bluecadet::utils;

class AppSampleApp : public BaseApp {
public:
	void setup() override;
	void update() override;
	void draw() override;
};

void AppSampleApp::setup()
{
	BaseApp::setup();
}

void AppSampleApp::update()
{
	BaseApp::update();
}

void AppSampleApp::draw()
{
	BaseApp::draw();
}

CINDER_APP(AppSampleApp, RendererGl)
