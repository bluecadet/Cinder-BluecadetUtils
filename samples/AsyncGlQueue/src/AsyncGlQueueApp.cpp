#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/Log.h"

#include "bluecadet/utils/AsyncGlQueue.h"

using namespace ci;
using namespace ci::app;
using namespace std;

using namespace bluecadet::utils;

class AsyncGlQueueApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
};

void AsyncGlQueueApp::setup()
{
	
}

void AsyncGlQueueApp::mouseDown( MouseEvent event )
{
	for (int i = 0; i < 2560; i++) {
		AsyncGlQueue::getInstance()->run([] {
			CI_LOG_I("running");
		}, [](bool completed, bool canceled) {
			CI_LOG_I("done");
		});
	}
}

void AsyncGlQueueApp::update()
{
}

void AsyncGlQueueApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP( AsyncGlQueueApp, RendererGl )
