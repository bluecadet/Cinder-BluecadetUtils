
#include "AsyncImageLoader.h"

#include "cinder/Log.h"
#include "cinder/Filesystem.h"
#include "cinder/imageIo.h"


using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace utils {
	
	// Static properties
	bool AsyncImageLoader::sIsInitialized = false;
	std::mutex AsyncImageLoader::mInitializationMutex;


	AsyncImageLoader::AsyncImageLoader(const unsigned int numThreads, const double maxMainThreadBlockDuration) :
		mMainThreadTasks(true, maxMainThreadBlockDuration),
		mBackgroundContexts(numThreads * 2)
	{
		for (unsigned int i = 0; i < mBackgroundContexts.getCapacity(); ++i) {
			gl::ContextRef context = gl::Context::create(gl::context());
			mBackgroundContexts.pushFront(context);
		}

		mWorkerThreadedTasks.setup(numThreads);
	}
	
	AsyncImageLoader::~AsyncImageLoader() {
		mBackgroundContexts.cancel();
		mWorkerThreadedTasks.destroy();
	}

	
	void AsyncImageLoader::load(const std::string path, Callback callback) {

		// check texture cache
		auto texIt = mTextureCache.find(path);
		if (texIt != mTextureCache.end()) {
			callback(path, texIt->second);
			return;
		}
		
		// check existing load tasks/callbacks
		lock_guard<mutex> lock(mCallbackMutex);
		auto cbIt = mCallbacks.find(path);
		if (cbIt != mCallbacks.end()) {
			mCallbacks[path].push_back(callback);
			return;
		}
		
		// load file and add callback
		mCallbacks[path].push_back(callback);
		
		mWorkerThreadedTasks.addTask([&, path] () {
			// configure for multithreading
			ci::ThreadSetup threadSetup;

			initializeLoader();

			if (!isLoading(path)) return; // abort if request has been cancelled
			
			auto data = loadFile(path);

			if (!data) {
				// Can't load image
				cancel(path);
				return;
			}

			if (!isLoading(path)) return; // abort if request has been cancelled

			// create cpu mem surface
			ci::Surface surface(data);
			gl::ContextRef context = nullptr;
			mBackgroundContexts.popBack(&context);

			if (!context) {
				CI_LOG_E("Could not create worker thread GL context when loading '" + path + "'");
				return;
			}

			context->makeCurrent();

			// create texture and store data on gpu memory
			const auto texture = gl::Texture::create(surface);

			// wait for upload to complete
			auto fence = gl::Sync::create();
			fence->clientWaitSync();

			// recycle context
			mBackgroundContexts.pushFront(context);

			context = nullptr;

			if (!isLoading(path)) return; // abort if request has been cancelled

			{
				// store in cache
				lock_guard<mutex> lock(mTextureMutex);
				mTextureCache[path] = texture;
			}
			
			// notifify callbacks
			App::get()->dispatchAsync([=] {
				triggerCallbacks(path, texture);
			});

			//createSurface(path, data);

			//mMainThreadTasks.add([=]{
				//if (!isLoading(path)) return; // abort if request has been cancelled

				//createTexture(path);
				//triggerCallbacks(path);
			//});
		});
	}
	
	void AsyncImageLoader::cancel(const std::string path) {
		// trigger pending callbacks immediately w/o waiting for load to finish
		// this will remove the callbacks and cancel any pending requests
		triggerCallbacks(path);
	}

	bool AsyncImageLoader::isLoading(const std::string path) {
		lock_guard<mutex> lock(mCallbackMutex);
		auto cbIt = mCallbacks.find(path);
		return cbIt != mCallbacks.end();
	}

	void AsyncImageLoader::cancelAll(const bool removeData) {
		lock_guard<mutex> lock(mCallbackMutex);
		for (auto it = mCallbacks.begin(); it != mCallbacks.end(); ) {
			const auto & path = it->first;
			const auto & callbacks = it->second;
			for (auto callback : callbacks) {
				callback(it->first, nullptr);
			}
			it = mCallbacks.erase(it);
		}

		if (removeData) {
			lock_guard<mutex> lock(mTextureMutex);
			mTextureCache.clear();
		}
	}
	
	bool AsyncImageLoader::hasTexture(const std::string path) {
		lock_guard<mutex> lock(mTextureMutex);
		return mTextureCache.find(path) != mTextureCache.end();
	}

	void AsyncImageLoader::removeTexture(const std::string path) {
		// remove texture if it was already loaded
		{
			lock_guard<mutex> lock(mTextureMutex);
			auto it = mTextureCache.find(path);
			if (it != mTextureCache.end()) {
				mTextureCache.erase(path);
			}
		}

		// trigger pending callbacks
		triggerCallbacks(path);
	}
	
	const ci::gl::TextureRef AsyncImageLoader::getTexture(const std::string path) {
		lock_guard<mutex> lock(mTextureMutex);
		auto it = mTextureCache.find(path);
		if (it == mTextureCache.end()) {
			return nullptr;
		}
		return it->second;
	}
	
	void AsyncImageLoader::triggerCallbacks(const std::string path, ci::gl::TextureRef texture) {
		std::vector<Callback> callbacks;
		
		{
			// grab callbacks
			lock_guard<mutex> lock(mCallbackMutex);
			auto cbIt = mCallbacks.find(path);
			
			if (cbIt != mCallbacks.end()) {
				
				// temp copy of callbacks
				callbacks = cbIt->second;
				
				// clear callbacks for this path before triggering to prevent blocking
				mCallbacks.erase(cbIt);
			}
		}

		if (!texture) {
			// try to get texture
			lock_guard<mutex> lock(mTextureMutex);
			auto texIt = mTextureCache.find(path);
			if (texIt != mTextureCache.end()) {
				texture = texIt->second;
			}
		}
		
		// trigger callbacks w texture or nullptr
		for (auto callback : callbacks) {
			callback(path, texture);
		}
	}

	inline gl::ContextRef AsyncImageLoader::getContext() {
		gl::ContextRef context = nullptr;
		mBackgroundContexts.popBack(&context);
		return context;
	}
	
	void AsyncImageLoader::initializeLoader() {
		lock_guard<mutex> lock(mInitializationMutex);

		if (sIsInitialized) {
			return;
		}

		App::get()->dispatchSync([=] {
			try {
				// load empty image to initialize Cinder's internal load factory on the main thread
				CI_LOG_D("Initializing Cinder loader");
				ci::loadImage("");
			} catch (Exception e) {
			}
			sIsInitialized = true;
		});
	}

	ci::ImageSourceRef AsyncImageLoader::loadFile(const std::string path) {
		try {
			if (ci::fs::exists(path)) {
				auto data = ci::loadImage(path);
				return data;
			} else {
				CI_LOG_E("Could not find image at " + path);
				return nullptr;
			}
		} catch (Exception e) {
			CI_LOG_EXCEPTION("Could not load image at " + path, e);
			return nullptr;
		}
	}
	
	void AsyncImageLoader::createSurface(const std::string path, const ci::ImageSourceRef source) {
		lock_guard<mutex> lock(mSurfaceMutex);
		// create surface and store on cpu memory
		mSurfaceCache[path] = Surface::create(source);
	}
	
	void AsyncImageLoader::createTexture(const std::string path) {
		ci::SurfaceRef surface = nullptr;
		
		{
			// try to get the surface
			lock_guard<mutex> lock(mSurfaceMutex);
			auto surfIt = mSurfaceCache.find(path);
			if (surfIt == mSurfaceCache.end()) {
				return;
			}
			
			// save surface locally so the mutex doesn't get locked for too long
			surface = surfIt->second;
			
			// remove surface from cache when done
			mSurfaceCache.erase(surfIt);
		}

		if (!surface) {
			CI_LOG_E("Could not create texture for '" + path + "' because surface is empty");
			return;
		}
		
		{
			ci::ThreadSetup threadSetup;
			auto context = getContext();

			if (!context) {
				CI_LOG_E("Could not create worker thread GL context when loading '" + path + "'");
				return;
			}

			context->makeCurrent();

			// create texture and store data on gpu memory
			const auto texture = gl::Texture::create(*surface);

			// wait for upload to complete
			auto fence = gl::Sync::create();
			fence->clientWaitSync();

			// store in cache
			lock_guard<mutex> lock(mTextureMutex);
			mTextureCache[path] = texture;
		}
	}
	
}
}
