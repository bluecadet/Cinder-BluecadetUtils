#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/Log.h"
#include "cinder/params/Params.h"

#include "bluecadet/utils/AsyncImageLoader.h"
#include "bluecadet/utils/FileUtils.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace bluecadet::utils;

class AsyncImageLoadingSampleApp : public App {
public:
	void setup() override;
	void draw() override;

	AsyncImageLoader mLoader;
	ThreadedTaskQueue mThreadedQueue;
	params::InterfaceGlRef mParams;

	int mNumTexturesToLoad = 0;
	int mNumTexturesLoaded = 0;
	std::vector<gl::TextureRef> mTextures;
};

void AsyncImageLoadingSampleApp::setup() {
	setFpsSampleInterval(0.1);
	mThreadedQueue.setup(4);

	mParams = params::InterfaceGl::create("Settings", ivec2(250, 150));
	//mParams->addButton("Load blue image", [=] {
	//	AsyncImageLoader::getInstance()->load(getAssetPath("bluecadet-blue.png").string(), [=](const string path, gl::TextureRef texture) {
	//		CI_LOG_D("Loaded " + path);
	//	});
	//});
	//mParams->addButton("Load white image", [=] {
	//	AsyncImageLoader::getInstance()->load(getAssetPath("bluecadet-white.png").string(), [=](const string path, gl::TextureRef texture) {
	//		CI_LOG_D("Loaded " + path);
	//	});
	//});
	//mParams->addButton("Load cadet image", [=] {
	//	AsyncImageLoader::getInstance()->load(getAssetPath("bluecadet-cadet.png").string(), [=](const string path, gl::TextureRef texture) {
	//		CI_LOG_D("Loaded " + path);
	//	});
	//});
	//mParams->addButton("Load cadet x 1000", [=] {
	//	for (int i = 0; i < 1000; ++i) {
	//		AsyncImageLoader::getInstance()->load(getAssetPath("bluecadet-cadet.png").string(), [=](const string path, gl::TextureRef texture) {
	//			if (texture) {
	//				CI_LOG_D("Loaded cadet " + to_string(i));
	//			} else {
	//				CI_LOG_D("Could not load cadet " + to_string(i));
	//			}
	//		});
	//	}
	//}, "key=l");
	//mParams->addButton("Cancel & remove cadet", [=] {
	//	CI_LOG_D("Canceling cadet");
	//	AsyncImageLoader::getInstance()->cancel(getAssetPath("bluecadet-cadet.png").string());
	//	AsyncImageLoader::getInstance()->removeTexture(getAssetPath("bluecadet-cadet.png").string());
	//}, "key=c");
	//mParams->addButton("Threaded Load & Cancel", [=] {
	//	for (int i = 0; i < 100; ++i) {
	//		mThreadedQueue.addTask([=] {
	//			AsyncImageLoader::getInstance()->load(getAssetPath("bluecadet-cadet.png").string(), [=](const string path, gl::TextureRef texture) {
	//				if (texture) {
	//					CI_LOG_D("Loaded cadet a " + to_string(i));
	//				} else {
	//					CI_LOG_D("Could not load cadet a " + to_string(i));
	//				}
	//			});
	//		});
	//	}
	//	mThreadedQueue.addTask([=] {
	//		AsyncImageLoader::getInstance()->cancel(getAssetPath("bluecadet-cadet.png").string());
	//	});
	//	for (int i = 0; i < 100; ++i) {
	//		AsyncImageLoader::getInstance()->load(getAssetPath("bluecadet-cadet.png").string(), [=](const string path, gl::TextureRef texture) {
	//			if (texture) {
	//				CI_LOG_D("Loaded cadet b " + to_string(i));
	//			} else {
	//				CI_LOG_D("Could not load cadet b " + to_string(i));
	//			}
	//		});
	//	}
	//	AsyncImageLoader::getInstance()->cancel(getAssetPath("bluecadet-cadet.png").string()); // most 'a' requests should fail, most 'b' requests should succeed
	//});
	mParams->addButton("Load All Assets", [=] {
		mNumTexturesToLoad = 0;
		mNumTexturesLoaded = 0;

		FileUtils::find(getAssetPath("thf_large"), [=] (const ci::fs::path & path) {
			mNumTexturesToLoad++;

			AsyncImageLoader::getInstance()->load(path.string(), [=] (const string path, gl::TextureRef texture) {
				if (texture) {
					CI_LOG_D("Loaded image " + path);
					mNumTexturesLoaded++;
					//mTextures.push_back(texture);

				} else {
					CI_LOG_D("Could not load image " + path);
				}
			});
		});
	}, "key=l");
	mParams->addButton("Cancel All", [=] { AsyncImageLoader::getInstance()->cancelAll(); mTextures.clear(); mNumTexturesLoaded = 0; mNumTexturesToLoad = 0; }, "key=c");
}

void AsyncImageLoadingSampleApp::draw() {
	gl::enableVerticalSync(false);
	gl::clear(Color(0, 0, 0));
	gl::enableAlphaBlending();

	// getTexture will return nullptr if it doesn't exist, so the below would be safe bcause draw() checks for nullptr
	//gl::draw(AsyncImageLoader::getInstance()->getTexture(getAssetPath("bluecadet-blue.png").string()));
	//gl::draw(AsyncImageLoader::getInstance()->getTexture(getAssetPath("bluecadet-white.png").string()));
	//gl::draw(AsyncImageLoader::getInstance()->getTexture(getAssetPath("thf_select\\1968RiversideARRC_087.jpg").string()));
	//gl::draw(AsyncImageLoader::getInstance()->getTexture(getAssetPath("thf_select\\Indianapolis50005-64_1158.jpg").string()));

	//// you can also check for textures explicitly if you want to be a responsible citizen
	//if (AsyncImageLoader::getInstance()->hasTexture(getAssetPath("bluecadet-cadet.png").string())) {
	//	gl::draw(AsyncImageLoader::getInstance()->getTexture(getAssetPath("bluecadet-cadet.png").string()));
	//}

	const vec2 numCells(ceilf(sqrtf(mNumTexturesToLoad)), ceilf(sqrtf(mNumTexturesToLoad)));
	vec2 cell = vec2(0, 0);
	vec2 cellSize = vec2(getWindowSize()) / numCells;

	/*for (auto tex : mTextures) {
		vec2 pos = cell * cellSize;
		Rectf rect(pos, pos + cellSize);
		gl::draw(tex, rect);
		cell.x += 1;
		if (cell.x >= numCells.x) {
			cell.x = 0;
			cell.y += 1;
		}
	}*/
	
	vec2 size(200);
	vec2 pos((sinf(getElapsedSeconds()) * 0.5f + 0.5f) * (vec2(getWindowSize()) - size));
	gl::drawSolidRect(Rectf(pos, pos + size));

	static Font font("Arial", 40);
	static Color color = Color(0, 1, 1);
	gl::drawString("FPS: " + to_string(getAverageFps()), vec2(0, getWindowHeight() - 20 - 2.0f * font.getSize()), color, font);
	gl::drawString("Loaded  " + to_string(mNumTexturesLoaded) + "/" + to_string(mNumTexturesToLoad), vec2(0, getWindowHeight() - 20 - font.getSize()), color, font);

	mParams->draw();
}

CINDER_APP(AsyncImageLoadingSampleApp, RendererGl, [=](ci::app::App::Settings * settings) {
	settings->setWindowSize(ivec2(1280, 720));
	settings->disableFrameRate();
})
