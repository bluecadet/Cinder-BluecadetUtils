#pragma once

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"

#include "ThreadedTaskQueue.h"
#include "TimedTaskQueue.h"

namespace bluecadet {
namespace utils {

typedef std::shared_ptr<class AsyncImageLoader> AsyncImageLoaderRef;
	
class AsyncImageLoader {
	
public:

	//! Optional shared instance. This class can still be independently instantiated.
	static AsyncImageLoaderRef getSharedInstance() {
		static AsyncImageLoaderRef instance = nullptr;
		if (!instance) {
			instance = AsyncImageLoaderRef(new AsyncImageLoader());
		}
		return instance;
	};
	
	// Callback type for load requests. Resulting texture will be nullptr if request failed or canceled
	typedef std::function<void(const std::string path, ci::gl::TextureRef textureOrNull)> Callback;
	
	//! numThreads: Threads used for loading + decoding images
	//! maxMainThreadBlockDuration: Max amount of time used per frame on the main thread to upload textures
	AsyncImageLoader(const unsigned int numThreads = 4, const double maxMainThreadBlockDuration = 0.05);
	virtual ~AsyncImageLoader();
	
	void load(const std::string path, Callback callback);
	void cancel(const std::string path);
	bool isLoading(const std::string path);
	
	bool hasTexture(const std::string path);
	void removeTexture(const std::string path); // removes texture if it exists and cancels pending requests if it has any
	const ci::gl::TextureRef getTexture(const std::string path);
	
protected:
	ci::ImageSourceRef loadFile(const std::string path); // on worker thread
	void createSurface(const std::string path, const ci::ImageSourceRef source); // on worker thread
	
	void createTexture(const std::string path); // on main thread
	void triggerCallbacks(const std::string path); // on main thread
	
	static void initializeLoader(); // makes sure that Cinder's internal static factories are initialized once on the main thread
	static bool sIsInitialized; // need to initialize Cinder image factory on main thread 
	static std::mutex mInitializationMutex;

	std::map<std::string, std::vector<Callback>> mCallbacks;
	std::map<std::string, ci::gl::TextureRef> mTextureCache;
	std::map<std::string, ci::SurfaceRef> mSurfaceCache;
	
	std::mutex mCallbackMutex;
	std::mutex mSurfaceMutex;
	std::mutex mTextureMutex;
	
	ThreadedTaskQueue mWorkerThreadedTasks;
	TimedTaskQueue mMainThreadTasks;
	
};

}
}
