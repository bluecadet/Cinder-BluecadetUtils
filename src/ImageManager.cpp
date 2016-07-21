#include "ImageManager.h"

#include "cinder/Filesystem.h"
#include "cinder/imageIo.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace utils {

ImageManager::ImageManager() {
	mValidExtensions.insert("png");
	mValidExtensions.insert("PNG");
	mValidExtensions.insert("jpg");
	mValidExtensions.insert("JPG");
	mValidExtensions.insert("jpeg");
	mValidExtensions.insert("JPEG");

	mDefaultFormat = gl::Texture::Format();
	mDefaultFormat.setMaxAnisotropy(gl::Texture2d::getMaxAnisotropyMax());
	mDefaultFormat.enableMipmapping(true);
	mDefaultFormat.setMaxMipmapLevel(2);
	mDefaultFormat.setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
	mDefaultFormat.setMagFilter(GL_LINEAR);
	mDefaultFormat.loadTopDown(true);
}

ImageManager::~ImageManager() {
}

void ImageManager::loadAllImagesInDirectory(const std::string &directoryName) {

	fs::path directory(getAssetPath(directoryName));

	if (!fs::exists(directory)) {
		cout << "ImageManager: Image directory '" << directory << "' does not exist." << endl;
		return;
	}

	int numImagesLoaded = 0;
	// Iterate over contents of the folder
	for (fs::directory_iterator it(directory); it != fs::directory_iterator(); ++it){
		if (!is_directory(*it)) {
			fs::path filePath = fs::path(*it);
			std::string fileName = extractFilename(filePath.string());
			std::string fileExtension = extractFileExtension(fileName);
			
			if (!fs::exists(filePath)) {
				cout << "ImageManager: Image '" << filePath << "' does not exist in " << directoryName << " folder." << endl;
				continue;
			}

			if (mValidExtensions.find(fileExtension) == mValidExtensions.end()) {
				cout << "ImageManager: Skipping file '" << fileName << "' because its extension '" << fileExtension << "' is not supported." << endl;
				continue;
			}

			// Load the image
			try {
				auto img = loadImage(loadAsset(directoryName + "/" + fileName));
				if (img) {
					if (img->getWidth() > 3780 || img->getHeight() > 2880) {
						cout << "ImageManager: Warning: Image " + fileName + " is very large (" + to_string(img->getWidth()) + "," + to_string(img->getHeight()) + ")" << endl;
					}
					mTexturesMap[fileName] = gl::Texture2d::create(img, mDefaultFormat);
					numImagesLoaded++;
				}
			}
			catch (Exception& e) {
				cout << "ImageManager: Couldn't load image '" << fileName << "': " << e.what();
			}
		}
	}

	cout << "ImageManager: Loaded " << to_string(numImagesLoaded) << " images from '" << directory << "'" << endl;
}

std::string ImageManager::extractFilename(const std::string &filepath) {
	std::size_t found = filepath.find_last_of("/\\");
	string filename = filepath.substr(found + 1);
	return filename;
}

std::string ImageManager::extractFileExtension(const std::string &filepath) {
	std::size_t found = filepath.find_last_of(".");
	string extension = filepath.substr(found + 1);
	return extension;
}

ci::gl::Texture2dRef ImageManager::getTexture(const std::string &fileName) {
	auto& it = mTexturesMap.find(fileName);
	if (it == mTexturesMap.end()) {
		cout << "ImageManager: Could not find image '" << fileName << "'. Check that it exists in folder." << endl;
		return nullptr;
	}
	return it->second;
}

}
}