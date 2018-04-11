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
			instance = std::make_shared<ImageManager>();
		}
		return instance;
	}

	virtual ~ImageManager();

	/// <summary>
	/// Loads a single image at absFilePath and uses its filename as key. Does not check if the image already exists (call hasTexture() explicitly for this).
	/// </summary>
	void load(const ci::fs::path & absFilePath, const ci::gl::Texture::Format & format = getDefaultFormat());

	/// <summary>
	/// Loads a single image at absFilePath and stores it under key. Does not check if the image already exists (call hasTexture() explicitly for this).
	/// </summary>
	virtual void load(const ci::fs::path & absFilePath, const std::string & key, const ci::gl::Texture::Format & format = getDefaultFormat());

	/// <summary>
	/// Loads all from dir.
	/// </summary>
	/// <param name="absDirPath">The absolute dir path.</param>
	/// <param name="extensions">Valid extensions to load, e.g. {".jpg", ".png"}.</param>
	/// <param name="recursive">Will load all subdirectories too if set to true.</param>
	/// <param name="fileNameAsKey">When true, will use the filename (e.g. "image.jpg"). When false, will use the full path as key (e.g. "C:\Users\Dev\Documents\image.jpg").</param>
	/// <param name="format">The texture format used for textures.</param>
	void loadAllFromDir(const ci::fs::path absDirPath, const std::set<std::string> extensions = {".jpg", ".jpeg", ".png"}, const bool recursive = true, const bool fileNameAsKey = true, const ci::gl::Texture::Format & format = getDefaultFormat());

	bool hasTexture(const std::string & key) const;
	ci::gl::Texture2dRef getTexture(const std::string & key) const;

	static const ci::gl::Texture::Format & getDefaultFormat();
	static void setDefaultFormat(ci::gl::Texture::Format value);

private:

	ImageManager();

	// All preloaded textures
	std::map<std::string, ci::gl::Texture2dRef>	mTexturesMap;

	static ci::gl::Texture2d::Format sDefaultFormat;
	static bool sDefaultFormatInitialized;
};

}
}