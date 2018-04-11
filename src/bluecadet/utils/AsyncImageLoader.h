#pragma once

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/ConcurrentCircularBuffer.h"

#include "ThreadedTaskQueue.h"
#include "TimedTaskQueue.h"

namespace bluecadet {
namespace utils {

typedef std::shared_ptr<class AsyncImageLoader> AsyncImageLoaderRef;
	
class AsyncImageLoader {
	
public:

	//! Optional shared instance. This class can still be independently instantiated.
	static AsyncImageLoaderRef getInstance() {
		static AsyncImageLoaderRef instance = nullptr;
		if (!instance) {
			instance = std::make_shared<AsyncImageLoader>();
		}
		return instance;
	};

	struct Request {
		std::string path;
		ci::gl::TextureRef texture = nullptr;

		Request(const std::string path, const ci::gl::TextureRef texture) : path(path), texture(texture) {}
	};
	
	// Callback type for load requests. Resulting texture will be nullptr if request failed or canceled
	typedef std::function<void(const std::string path, ci::gl::TextureRef textureOrNull)> Callback;
	
	//! numThreads: Threads used for loading + decoding images
	AsyncImageLoader(const unsigned int numThreads = 1);
	virtual ~AsyncImageLoader();
	
	void load(const std::string path, Callback callback);
	void cancel(const std::string path);
	bool isLoading(const std::string path);
	
	void cancelAll(const bool removeData = true); // cancels any pending loads and removes existing surfaces and textures

	bool hasTexture(const std::string path);
	void removeTexture(const std::string path); // removes texture if it exists and cancels pending requests if it has any
	const ci::gl::TextureRef getTexture(const std::string path);

	void setNumThreads(const unsigned int value) { mNumThreads = value; setup(); }
	unsigned int getNumThreads() const { return mNumThreads; }

	static const ci::gl::Texture::Format & getDefaultFormat();
	static void setDefaultFormat(ci::gl::Texture::Format value);
	
protected:
	void loadImages(ci::gl::ContextRef context); // on worker thread
	void transferTexturesToMain(); // on main thread
	void triggerCallbacks(const std::string path, ci::gl::TextureRef texture = nullptr); // on main thread

	void setup();
	static void initializeLoader(); // makes sure that Cinder's internal static factories are initialized once on the main thread
	static bool sIsInitialized; // need to initialize Cinder image factory on main thread 
	static std::mutex mInitializationMutex;

	unsigned int mNumThreads = -1;

	std::map<std::string, std::vector<Callback>> mCallbacks;
	std::map<std::string, ci::gl::TextureRef> mTextureCache;
	std::set<ci::gl::ContextRef> mBackgroundContexts;
	ci::ConcurrentCircularBuffer<Request> mTextureBuffer;

	std::mutex mCallbackMutex;
	std::mutex mTextureMutex;
	std::mutex mRequestMutex;

	std::condition_variable mRequestLock;
	std::deque<std::string> mRequests;

	std::set<std::shared_ptr<std::thread>> mThreads;
	std::atomic<bool> mThreadsAreAlive = true;
	ci::signals::ConnectionList mSignalConnections;
	
	static ci::gl::Texture2d::Format sDefaultFormat;
	static bool sDefaultFormatInitialized;
};

}
}
