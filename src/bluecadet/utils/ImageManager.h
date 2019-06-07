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

	enum KeyMapping {
		Filename,		/// Uses just the filename (incld. extension) as key ("c:\downloads\2017\12\image.png" would have a key of "image.png")
		RelativePath,	/// Uses the relative path as key (e.g. when loading images from "c:\downloads\", then "c:\downloads\2017\12\image.png" would have a key of "2017\12\image.png")
		AbsolutePath	/// Uses the full absolute path as key ("c:\downloads\2017\12\image.png" would have a key of "c:\downloads\2017\12\image.png")
	};

	struct LoadOptions {
		inline LoadOptions & recursive(const bool recursive)						{ mRecursive = recursive; return *this; }
		inline LoadOptions & keyMapping(const KeyMapping keyMapping)				{ mKeyMapping = keyMapping; return *this; }
		inline LoadOptions & extensions(const std::set<std::string> & extensions)	{ mExtensions = extensions; return *this; }
		inline LoadOptions & forceLowercaseExtensions(const bool force)				{ mForceLowercaseExtensions = force; return *this; }
		inline LoadOptions & forceForwardSlashes(const bool force)					{ mForceForwardSlashes = force; return *this; }

		inline const bool						getRecursive() const				{ return mRecursive; }
		inline const KeyMapping					getKeyMapping() const				{ return mKeyMapping; }
		inline const std::set<std::string> &	getExtensions() const				{ return mExtensions; }
		inline const bool						getForceLowercaseExtensions() const { return mForceLowercaseExtensions; }
		inline const bool						getForceForwardSlashes() const		{ return mForceForwardSlashes; }

	private:
		bool					mRecursive = true;
		KeyMapping				mKeyMapping = KeyMapping::RelativePath;
		std::set<std::string>	mExtensions = {".jpg", ".jpeg", ".png"};
		bool					mForceLowercaseExtensions = true;
		bool					mForceForwardSlashes = true;
	};

	static ImageManagerRef getInstance() {
		static ImageManagerRef instance = nullptr;
		if (instance == nullptr) {
			instance = ImageManagerRef(new ImageManager());
		}
		return instance;
	}

	~ImageManager();

	/// <summary>
	/// Loads a single image at absFilePath and uses its filename as key.
	/// </summary>
	void load(const ci::fs::path & absFilePath, const ci::gl::Texture::Format & format = getDefaultFormat());

	/// <summary>
	/// Loads a single image at absFilePath and stores it under key.
	/// </summary>
	void load(const ci::fs::path & absFilePath, const std::string & key, const ci::gl::Texture::Format & format = getDefaultFormat());

	/// <summary>
	/// Loads all from dir.
	/// </summary>
	/// <param name="absDirPath">The absolute dir path.</param>
	/// <param name="options">Options used or loading.</param>
	/// <param name="format">The texture format used for textures.</param>
	void loadAllFromDir(const ci::fs::path absDirPath, const LoadOptions options = LoadOptions(), const ci::gl::Texture::Format & format = getDefaultFormat());

	bool hasTexture(const std::string & key) const;
	ci::gl::Texture2dRef getTexture(const std::string & key) const;

	/// <summary>
	/// Adds a texture for 'key'.
	/// If 'key' exists, the texture for that key will be overwritten.
	/// </summary>
	void add(const std::string & key, ci::gl::TextureRef texture);

	/// <summary>
	/// Removes all references to local textures and keys.
	/// Note that this does not destroy the textures if they're still references by other code.
	/// </summary>
	void removeAll();


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