#pragma once

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"

#include "ThreadedTaskQueue.h"
#include "TimedTaskQueue.h"

namespace bluecadet {
namespace utils {
	
class ThreadedImageLoader {
	// texture will be nullptr if no success or canceled
	typedef std::function<void(const std::string path, ci::gl::TextureRef textureOrNull)> Callback;
	
public:
	
	//! numThreads: Threads used for loading + decoding images
	//! maxMainThreadBlockDuration: Max amount of time used per frame on the main thread to upload textures
	ThreadedImageLoader(const unsigned int numThreads = 4, const double maxMainThreadBlockDuration = 0.05);
	virtual ~ThreadedImageLoader();
	
	void load(const std::string path, Callback callback);
	void cancel(const std::string path);
	bool isLoading(const std::string path);
	
	bool hasTexture(const std::string path);
	void removeTexture(const std::string path); // removes texture if it exists and cancels pending requests if it has any
	const ci::gl::TextureRef getTexture(const std::string path);
	
protected:
	void initializeLoader(); // makes sure that Cinder's image loader is initialized once on the main thread
	ci::ImageSourceRef loadFile(const std::string path); // on worker thread
	void createSurface(const std::string path, const ci::ImageSourceRef source); // on worker thread
	
	void createTexture(const std::string path); // on main thread
	void triggerCallbacks(const std::string path); // on main thread
	
	static bool sIsInitialized; // ned to initialize Cinder image factory on main thread 

	std::map<std::string, std::vector<Callback>> mCallbacks;
	std::map<std::string, ci::gl::TextureRef> mTextureCache;
	std::map<std::string, ci::SurfaceRef> mSurfaceCache;
	
	std::mutex mInitializationMutex;
	std::mutex mCallbackMutex;
	std::mutex mSurfaceMutex;
	std::mutex mTextureMutex;
	
	ThreadedTaskQueue mWorkerThreadedTasks;
	TimedTaskQueue mMainThreadTasks;
	
};

}
}
