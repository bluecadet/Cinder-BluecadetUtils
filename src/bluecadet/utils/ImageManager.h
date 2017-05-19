//+---------------------------------------------------------------------------
//  Bluecadet Interactive 2016
//  Contents: 
//  Comments: 
//----------------------------------------------------------------------------

#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"

namespace bluecadet {
namespace utils {

typedef std::shared_ptr<class ImageManager> ImageManagerRef;

class ImageManager {

public:

	static ImageManagerRef getInstance() {
		static ImageManagerRef instance = nullptr;
		if (instance == nullptr) {
			instance = ImageManagerRef(new ImageManager());
		}
		return instance;
	}

public:
	~ImageManager();

	void					loadAllImagesInDirectory(const std::string &directory);
	void					loadAllImagesInDirectory(const std::string &directory, const ci::gl::Texture::Format format);
	bool					hasTexture(const std::string &filename);
	ci::gl::Texture2dRef	getTexture(const std::string &filename);

	ci::gl::Texture::Format	getDefaultFormat() const { return mDefaultFormat; }
	void					setDefaultFormat(const ci::gl::Texture::Format value) { mDefaultFormat = value; }

private:

	ImageManager();
	std::string		extractFilename(const std::string &filepath);
	std::string		extractFileExtension(const std::string &filepath);

	// All preloaded textures
	std::map<std::string, ci::gl::Texture2dRef>	mTexturesMap;
	std::set<std::string> mValidExtensions;
	ci::gl::Texture2d::Format mDefaultFormat;
};

}
}