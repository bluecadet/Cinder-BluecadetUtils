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
	
	void loadSimple();
	void loadWithLoader();
	
	ThreadedTaskQueue mWorkerThreadedTasks;
	ThreadedImageLoader mLoader;
	
};

void ThreadedImageLoadingSampleApp::setup()
{
	loadSimple();
	loadWithLoader();
}

void ThreadedImageLoadingSampleApp::loadSimple() {
	mWorkerThreadedTasks.addTask([=]{
		auto data = loadImage(getAssetPath("blue.png"));
		
		// Since our surface is a shared ptr it'll live in heap memory and not in stack.
		// If you're using a local surface by value, make sure you're not causing a stack
		// overflow with large surfaces.
		auto surface = Surface::create(data);
		
		// App::get() is redundant here since we're already in App, but kept it for clarity
		App::get()->dispatchAsync([=]{
			auto texture = gl::Texture::create(*surface);
			console() << "texture loaded" << endl;
		});
	});
}

void ThreadedImageLoadingSampleApp::loadWithLoader() {
	console() << "starting setup..." << endl;
	for (int i = 0; i < 10000; ++i) {
		mLoader.load(getAssetPath("blue.png").string(), [=] (const string path, gl::TextureRef texture) {
			console() << "loaded " << path << endl;
		});
	}
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
