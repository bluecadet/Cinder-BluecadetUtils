#include "FileUtils.h"

#include "cinder/Filesystem.h"
#include "cinder/Log.h"

#include <algorithm>
#include <string>

using namespace ci;
using namespace std;

namespace bluecadet {
namespace utils {

void FileUtils::find(const ci::fs::path & dir, FileCallback callback, const std::set<std::string> extensions, const bool recursive, const bool lowercaseExtensions) {
	if (!fs::exists(dir)) {
		CI_LOG_E("Directory at '" + dir.string() + "' doesn't exist.");
		return;
	}

	int numVideosLoaded = 0;

	for (fs::directory_iterator it(dir); it != fs::directory_iterator(); ++it) {
		const fs::path filePath = it->path();

		if (is_directory(filePath)) {
			find(filePath, callback, extensions, recursive);
			continue;
		}

		if (!fs::exists(filePath)) {
			CI_LOG_E("Could not access file at '" + filePath.string() + "'.");
			continue;
		}

		string extension = getExtension(filePath);

		if (lowercaseExtensions) {
			transform(extension.begin(), extension.end(), extension.begin(), tolower);
		}

		if (!extensions.empty() && extensions.find(extension) == extensions.end()) {
			continue;
		}

		callback(filePath);
	}
}

std::string FileUtils::getFilename(const ci::fs::path & path) {
	return path.filename().string(); // You can ignore any IntelliSense errors here. This should build.
}

std::string FileUtils::getExtension(const ci::fs::path & path) {
	return path.extension().string(); // You can ignore any IntelliSense errors here. This should build.
}

}
}
