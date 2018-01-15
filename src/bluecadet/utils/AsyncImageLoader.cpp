
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


	AsyncImageLoader::AsyncImageLoader(const unsigned int numThreads) :
		mNumThreads(numThreads),
		mTextureBuffer(mNumThreads * 2)
	{
	}
	
	AsyncImageLoader::~AsyncImageLoader() {
		mWasCanceled = true;
		mRequestLock.notify_all();
		for (auto thread : mThreads) {
			thread->join();
		}
		mTextureBuffer.cancel();
	}

	void AsyncImageLoader::loadImages(ci::gl::ContextRef context) {
		ci::ThreadSetup threadSetup;

		context->makeCurrent();
		initializeLoader();

		while (!mWasCanceled) {
			std::string path;

			{
				// wait for new requests
				unique_lock<mutex> lock(mRequestMutex);
				while (!mWasCanceled && mRequests.empty()) {
					mRequestLock.wait(lock);
				}

				if (mWasCanceled) return;

				path = mRequests.front();
				mRequests.pop_front();
			}

			if (!path.empty()) {
				try {

					auto data = loadImage(path);

					if (!data) {
						// Can't load image
						cancel(path);
						continue;
					}

					if (!isLoading(path)) continue; // abort if request has been cancelled
					if (mWasCanceled) return;

					// create cpu mem surface
					ci::Surface surface(data);

					// create texture and store data on gpu memory
					const auto texture = gl::Texture::create(surface);

					// create fence after all gpu commands
					auto fence = gl::Sync::create();

					// waits until fence has been executed
					fence->clientWaitSync();

					if (!isLoading(path)) continue; // abort if request has been cancelled
					if (mWasCanceled) return;

					Request request(path, texture);
					mTextureBuffer.pushFront(request);

				} catch (ci::Exception e) {
					CI_LOG_EXCEPTION("Could not load image at '" + path + "'.", e);
				}
			}
		}
	}

	void AsyncImageLoader::transferTexturesToMain() {
		Request request("", nullptr);
		while (mTextureBuffer.tryPopBack(&request)) {
			mTextureCache[request.path] = request.texture;
			triggerCallbacks(request.path, request.texture);
		}
	}

	
	void AsyncImageLoader::load(const std::string path, Callback callback) {
		setup();

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
		{
			unique_lock<mutex> lock(mRequestMutex);
			mRequests.push_back(path);
			mRequestLock.notify_one();
		}
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

	void AsyncImageLoader::setup() {
		// only set up if # threads has changed
		if (mNumThreads == mThreads.size()) {
			return;
		}

		// reset previous
		App::get()->dispatchSync([=] {
			
			mBackgroundContexts.clear();
			mThreads.clear();
			mSignalConnections.clear();

			for (unsigned int i = 0; i < mNumThreads; ++i) {
				auto context = gl::Context::create(gl::context());
				auto thread = make_shared<std::thread>(bind(&AsyncImageLoader::loadImages, this, context));
				mThreads.insert(thread);
				mBackgroundContexts.insert(context);
			}

			mSignalConnections += App::get()->getSignalUpdate().connect(bind(&AsyncImageLoader::transferTexturesToMain, this));

		});
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
	
}
}
