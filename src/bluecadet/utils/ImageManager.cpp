#include "ImageManager.h"

#include "cinder/Filesystem.h"
#include "cinder/imageIo.h"
#include "cinder/Log.h"

#include "FileUtils.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace utils {

ci::gl::Texture::Format ImageManager::sDefaultFormat;
bool ImageManager::sDefaultFormatInitialized = false;

ImageManager::ImageManager() {
}

ImageManager::~ImageManager() {
}

void ImageManager::load(const ci::fs::path & absFilePath, const ci::gl::Texture::Format & format) {
	const std::string & key = FileUtils::getFilename(absFilePath);
	load(absFilePath, key, format);
}

void ImageManager::load(const ci::fs::path & absFilePath, const std::string & key, const ci::gl::Texture::Format & format) {
	try {
		if (const auto img = loadImage(absFilePath)) {
			mTexturesMap[key] = gl::Texture2d::create(img, format);

		} else {
			throw ci::Exception("Could not load image from '" + absFilePath.string() + "'");
		}
	} catch (Exception e) {
		CI_LOG_EXCEPTION("Could not load image from '" + absFilePath.string() + "'", e);
	}
}

void ImageManager::loadAllFromDir(const ci::fs::path absDirPath, const LoadOptions options, const ci::gl::Texture::Format & format) {
	int numImagesLoaded = 0;
	string absDirStr = absDirPath.string();

	if (!absDirStr.empty() && absDirStr.back() != ci::fs::path::preferred_separator) {
		// add trailing separator to directory path (makes relative path keys more legible)
		absDirStr += ci::fs::path::preferred_separator;
	}

	FileUtils::find(absDirPath, [&](const ci::fs::path & path) {

		string key = options.getKeyMapping() == KeyMapping::Filename ? FileUtils::getFilename(path) : path.string();

		if (options.getKeyMapping() == KeyMapping::RelativePath) {
			const size_t absDirLength = absDirStr.size();
			key = key.substr(absDirLength, key.size() - absDirLength);
		}

		if (options.getForceForwardSlashes()) {
			std::replace(key.begin(), key.end(), '\\', '/');
		}

		load(path, key, format);
		numImagesLoaded++;
	}, options.getExtensions(), options.getRecursive(), options.getForceLowercaseExtensions());

	CI_LOG_I("Loaded " + to_string(numImagesLoaded) + " images from " + absDirPath.string());
}

bool ImageManager::hasTexture(const std::string & key) const {
	return mTexturesMap.find(key) != mTexturesMap.end();
}

ci::gl::Texture2dRef ImageManager::getTexture(const std::string & key) const {
	const auto & it = mTexturesMap.find(key);
	if (it == mTexturesMap.end()) {
		CI_LOG_W("Could not find image with key '" << key << "'.");
		return nullptr;
	}
	return it->second;
}

void ImageManager::add(const std::string& key, ci::gl::TextureRef texture) {
	mTexturesMap[key] = texture;
}

void ImageManager::removeAll() {
	mTexturesMap.clear();
}

const ci::gl::Texture::Format & ImageManager::getDefaultFormat() {
	if (!sDefaultFormatInitialized) {
		sDefaultFormatInitialized = true;

		sDefaultFormat = gl::Texture::Format();
		sDefaultFormat.setMaxAnisotropy(gl::Texture2d::getMaxAnisotropyMax());
		sDefaultFormat.enableMipmapping(true);
		sDefaultFormat.setMaxMipmapLevel(2);
		sDefaultFormat.setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
		sDefaultFormat.setMagFilter(GL_LINEAR);
	}
	return sDefaultFormat;
}

void ImageManager::setDefaultFormat(ci::gl::Texture::Format format) {
	sDefaultFormat = format;
	sDefaultFormatInitialized = true;
}

}
}