#pragma once

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"

#include "ThreadedTaskQueue.h"

namespace bluecadet {
namespace utils {
	
class ThreadedImageLoader {
	// texture will be nullptr if no success or canceled
	typedef std::function<void(const std::string path, ci::gl::TextureRef textureOrNull)> Callback;
	
public:
	
	ThreadedImageLoader(const unsigned int numThreads = 3);
	virtual ~ThreadedImageLoader();
	
	void load(const std::string path, Callback callback);
	void cancel(const std::string path);
	
	bool isLoading(const std::string path);
	bool hasTexture(const std::string path) const;
	const ci::gl::TextureRef getTexture(const std::string path) const;
	
protected:
	ci::ImageSourceRef loadFile(const std::string path); // on worker thread
	void createSurface(const std::string path, const ci::ImageSourceRef source); // on worker thread
	
	void createTexture(const std::string path); // on main thread
	void triggerCallbacks(const std::string path); // on main thread
	
	std::map<std::string, std::vector<Callback>> mCallbacks;
	std::map<std::string, ci::gl::TextureRef> mTextureCache;
	std::map<std::string, ci::SurfaceRef> mSurfaceCache;
	
	std::mutex mCallbackMutex;
	std::mutex mSurfaceMutex;
	
	ThreadedTaskQueue mTasks;
	
};

}
}
