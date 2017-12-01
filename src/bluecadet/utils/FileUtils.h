#pragma once

#include <functional>
#include <set>

#include "cinder/Filesystem.h"

namespace bluecadet {
namespace utils {

class FileUtils {

public:
	
	typedef std::function<void(const ci::fs::path & path)> FileCallback;

	/// <summary>
	/// Finds all files in a directory and triggers callback on each depth-first.
	/// </summary>
	/// <param name="dir">The absolute dir path.</param>
	/// <param name="callback">Callback that will be triggered for each file.</param>
	/// <param name="extensions">Valid extensions to load, e.g. {".json", ".csv"}. If lowercaseExtensions is true, all extensions should be lowercase only.</param>
	/// <param name="recursive">Will load all subdirectories too if set to true.</param>
	/// <param name="lowercaseExtensions">If true, will convert all found extensions to lowercase. This presumes that all extenssions in the extensions parameter are also lowercase.</param>
	static void find(const ci::fs::path & dir, FileCallback callback, const std::set<std::string> extensions = {}, const bool recursive = true, const bool lowercaseExtensions = true);

	//! Filename including extension. Avoids issues with compiler/IDE version discrepancies.
	static std::string getFilename(const ci::fs::path & path);

	//! Extension starting at the last '.' (e.g. 'file.ext' will return '.ext'). Avoids issues with compiler/IDE version discrepancies.
	static std::string getExtension(const ci::fs::path & path);
};

}
}