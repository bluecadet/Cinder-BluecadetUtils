#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "ThreadedImageLoader.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace bluecadet::utils;

class ThreadedImageLoadingSampleApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
	
	ThreadedImageLoader mLoader;
	
};

void ThreadedImageLoadingSampleApp::setup()
{
	console() << "starting setup..." << endl;
	mLoader.load(getAssetPath("blue.png").string(), [=] (const string path, gl::TextureRef texture) {
		console() << "loaded " << path << endl;
	});
	mLoader.load(getAssetPath("white.png").string(), [=] (const string path, gl::TextureRef texture) {
		console() << "loaded " << path << endl;
	});
	mLoader.load(getAssetPath("small-cadet_black-01.png").string(), [=] (const string path, gl::TextureRef texture) {
		console() << "loaded " << path << endl;
	});
	console() << "...setup complete" << endl;
}

void ThreadedImageLoadingSampleApp::mouseDown( MouseEvent event )
{
}

void ThreadedImageLoadingSampleApp::update()
{
}

void ThreadedImageLoadingSampleApp::draw()
{
	gl::clear(Color(0, 0, 0));
	gl::enableAlphaBlending();
	
	// getTexture will return nullptr if it doesn't exist, so the below would be safe
	gl::draw(mLoader.getTexture(getAssetPath("blue.png").string()));
	gl::draw(mLoader.getTexture(getAssetPath("white.png").string()));
	gl::draw(mLoader.getTexture(getAssetPath("small-cadet_black-01.png").string()));
	
	// you can also check for textures explicitly
//	if (mLoader.hasTexture(getAssetPath("small-cadet_black-01.png").string())) {
//		gl::draw(mLoader.getTexture(getAssetPath("small-cadet_black-01.png").string()));
//	}
}

CINDER_APP( ThreadedImageLoadingSampleApp, RendererGl )
