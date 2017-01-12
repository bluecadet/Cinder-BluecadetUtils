
#include "ThreadedImageLoader.h"

#include "cinder/Filesystem.h"
#include "cinder/imageIo.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace utils {

	ThreadedImageLoader::ThreadedImageLoader(const unsigned int numThreads) {
		mTasks.setup(numThreads);
	}
	
	ThreadedImageLoader::~ThreadedImageLoader() {
		
	}
	
	void ThreadedImageLoader::load(const std::string path, Callback callback) {
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
		
		mTasks.addTask([=] () {
			cout << "loading " << path << endl;
			
			// worker thread
			auto data = loadFile(path);
			createSurface(path, data);
			
			App::get()->dispatchAsync([=]{
				// main thread
				createTexture(path); // this could be offloaded to a timed queue for additional optimization
				triggerCallbacks(path);
			});
		});
	}
	
	void ThreadedImageLoader::cancel(const std::string path) {
		triggerCallbacks(path);
	}
	
	bool ThreadedImageLoader::hasTexture(const std::string path) const {
		return mTextureCache.find(path) != mTextureCache.end();
	}
	
	const ci::gl::TextureRef ThreadedImageLoader::getTexture(const std::string path) const {
		auto it = mTextureCache.find(path);
		if (it == mTextureCache.end()) {
			return nullptr;
		}
		return it->second;
	}
	
	bool ThreadedImageLoader::isLoading(const std::string path) {
		lock_guard<mutex> lock(mCallbackMutex);
		auto cbIt = mCallbacks.find(path);
		return cbIt != mCallbacks.end();
	}
	
	void ThreadedImageLoader::triggerCallbacks(const std::string path) {
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
		
		// try to get texture
		gl::TextureRef tex = nullptr;
		auto texIt = mTextureCache.find(path);
		if (texIt != mTextureCache.end()) {
			tex = texIt->second;
		}
		
		// trigger callbacks w texture or nullptr
		for (auto callback : callbacks) {
			callback(path, tex);
		}
	}
	
	ci::ImageSourceRef ThreadedImageLoader::loadFile(const std::string path) {
		return ci::loadImage(path);
	}
	
	void ThreadedImageLoader::createSurface(const std::string path, const ci::ImageSourceRef source) {
		lock_guard<mutex> lock(mSurfaceMutex);
		// create surface and store on cpu memory
		mSurfaceCache[path] = Surface::create(source);
	}
	
	void ThreadedImageLoader::createTexture(const std::string path) {
		ci::SurfaceRef surface = nullptr;
		
		{
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
		
		// create texture and store data on gpu memory
		mTextureCache[path] = gl::Texture::create(*surface);
	}
	
}
}
