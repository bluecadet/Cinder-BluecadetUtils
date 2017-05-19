#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/Log.h"
#include "cinder/params/Params.h"

#include "ThreadedImageLoader.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace bluecadet::utils;

class ThreadedImageLoadingSampleApp : public App {
public:
	void setup() override;
	void draw() override;

	ThreadedImageLoader mLoader;
	ThreadedTaskQueue mThreadedQueue;
	params::InterfaceGlRef mParams;
};

void ThreadedImageLoadingSampleApp::setup() {
	mThreadedQueue.setup(4);

	mParams = params::InterfaceGl::create("Settings", ivec2(250, 150));
	mParams->addButton("Load blue image", [=] {
		mLoader.load(getAssetPath("bluecadet-blue.png").string(), [=](const string path, gl::TextureRef texture) {
			CI_LOG_D("Loaded " + path);
		});
	});
	mParams->addButton("Load white image", [=] {
		mLoader.load(getAssetPath("bluecadet-white.png").string(), [=](const string path, gl::TextureRef texture) {
			CI_LOG_D("Loaded " + path);
		});
	});
	mParams->addButton("Load cadet image", [=] {
		mLoader.load(getAssetPath("bluecadet-cadet.png").string(), [=](const string path, gl::TextureRef texture) {
			CI_LOG_D("Loaded " + path);
		});
	});
	mParams->addButton("Load cadet x 1000", [=] {
		for (int i = 0; i < 1000; ++i) {
			mLoader.load(getAssetPath("bluecadet-cadet.png").string(), [=](const string path, gl::TextureRef texture) {
				if (texture) {
					CI_LOG_D("Loaded cadet " + to_string(i));
				} else {
					CI_LOG_D("Could not load cadet " + to_string(i));
				}
			});
		}
	}, "key=l");
	mParams->addButton("Cancel & remove cadet", [=] {
		CI_LOG_D("Canceling cadet");
		mLoader.cancel(getAssetPath("bluecadet-cadet.png").string());
		mLoader.removeTexture(getAssetPath("bluecadet-cadet.png").string());
	}, "key=c");
	mParams->addButton("Threaded Load & Cancel", [=] {
		for (int i = 0; i < 100; ++i) {
			mThreadedQueue.addTask([=] {
				mLoader.load(getAssetPath("bluecadet-cadet.png").string(), [=](const string path, gl::TextureRef texture) {
					if (texture) {
						CI_LOG_D("Loaded cadet a " + to_string(i));
					} else {
						CI_LOG_D("Could not load cadet a " + to_string(i));
					}
				});
			});
		}
		mThreadedQueue.addTask([=] {
			mLoader.cancel(getAssetPath("bluecadet-cadet.png").string());
		});
		for (int i = 0; i < 100; ++i) {
			mLoader.load(getAssetPath("bluecadet-cadet.png").string(), [=](const string path, gl::TextureRef texture) {
				if (texture) {
					CI_LOG_D("Loaded cadet b " + to_string(i));
				} else {
					CI_LOG_D("Could not load cadet b " + to_string(i));
				}
			});
		}
		mLoader.cancel(getAssetPath("bluecadet-cadet.png").string()); // most 'a' requests should fail, most 'b' requests should succeed
	});
}

void ThreadedImageLoadingSampleApp::draw() {
	gl::clear(Color(0, 0, 0));
	gl::enableAlphaBlending();

	// getTexture will return nullptr if it doesn't exist, so the below would be safe bcause draw() checks for nullptr
	gl::draw(mLoader.getTexture(getAssetPath("bluecadet-blue.png").string()));
	gl::draw(mLoader.getTexture(getAssetPath("bluecadet-white.png").string()));

	// you can also check for textures explicitly if you want to be a responsible citizen
	if (mLoader.hasTexture(getAssetPath("bluecadet-cadet.png").string())) {
		gl::draw(mLoader.getTexture(getAssetPath("bluecadet-cadet.png").string()));
	}

	mParams->draw();
}

CINDER_APP(ThreadedImageLoadingSampleApp, RendererGl)
