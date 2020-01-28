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
	params::InterfaceGlRef mParams;

	int mNumTexturesToLoad = 0;
	int mNumTexturesLoaded = 0;
	std::vector<gl::TextureRef> mTextures;
};

void AsyncImageLoadingSampleApp::setup() {
	setFpsSampleInterval(0.1);

	mParams = params::InterfaceGl::create("Settings", toPixels(ivec2(250, 150)));
	mParams->addButton("Load All Assets", [=] {
		mNumTexturesToLoad = 0;
		mNumTexturesLoaded = 0;

		FileUtils::find(getAssetPath("thf_large"), [=] (const ci::fs::path & path) {
			mNumTexturesToLoad++;

			AsyncImageLoader::get()->load(path.string(), [=] (const string path, gl::TextureRef texture) {
				if (texture) {
					//CI_LOG_I("Loaded image " + path);
					mNumTexturesLoaded++;
					mTextures.push_back(texture);

				} else {
					CI_LOG_I("Could not load image " + path);
				}
			});
		});
	}, "key=l");
	mParams->addParam<int>("Num Threads", [=](int v) { AsyncImageLoader::get()->setNumThreads(v); }, [=] { return AsyncImageLoader::get()->getNumThreads(); });
	mParams->addButton("Cancel All", [=] { AsyncImageLoader::get()->cancelAll(); mTextures.clear(); mNumTexturesLoaded = 0; mNumTexturesToLoad = 0; }, "key=c");
}

void AsyncImageLoadingSampleApp::draw() {
	gl::enableVerticalSync(false);
	gl::clear(Color(0, 0, 0));
	gl::enableAlphaBlending();

	const vec2 numCells(ceilf(sqrtf(mNumTexturesToLoad)), ceilf(sqrtf(mNumTexturesToLoad)));
	vec2 cell = vec2(0, 0);
	vec2 cellSize = vec2(getWindowSize()) / numCells;

	for (auto tex : mTextures) {
		vec2 pos = cell * cellSize;
		Rectf rect(pos, pos + cellSize);
		gl::draw(tex, rect);
		cell.x += 1;
		if (cell.x >= numCells.x) {
			cell.x = 0;
			cell.y += 1;
		}
	}
	
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
	settings->setHighDensityDisplayEnabled(true);
	settings->setWindowSize(ivec2(1280, 720));
	settings->disableFrameRate();
})
