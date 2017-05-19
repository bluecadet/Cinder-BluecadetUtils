#include "ShaderManager.h"
#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace utils {

ShaderManager::ShaderManager(){}
ShaderManager::~ShaderManager(){}

void ShaderManager::reloadAll() {
	CI_LOG_I("ShaderManager: Reloading all shaders...");

	for (auto & infoPair : mShaderInfoMap) {
		auto key = infoPair.first;
		auto shader = loadShader(infoPair.second);
		mShaderMap[key] = shader;
		CI_LOG_I("ShaderManager: Set up shader '" + key + "'");
		mDidUpdateShader.emit(infoPair.first, shader);
	}

	mDidReloadAll.emit();
}

gl::GlslProgRef ShaderManager::setupShader(const std::string key, const ShaderInfo info) {
	bool existed = hasShader(key);

	mShaderInfoMap[key] = info;
	auto shader = loadShader(info);
	mShaderMap[key] = shader;

	CI_LOG_I("ShaderManager: Set up shader '" + key + "'");
	mDidUpdateShader.emit(key, shader);

	return shader;
}

bool ShaderManager::hasShader(const std::string & key) const {
	auto it = mShaderMap.find(key);
	return it != mShaderMap.end();
}

ci::gl::GlslProgRef ShaderManager::getShader(const std::string & key) {
	auto it = mShaderMap.find(key);
	if (it == mShaderMap.end()) {
		return nullptr;
	}
	return it->second;
}

ci::gl::GlslProgRef ShaderManager::getShaderByAssetPath(const std::string & assetPath, ShaderInfo::FormatCallback formatCallback){
	setupShader(assetPath, ShaderInfo(
		assetPath.empty() ? "" : assetPath + ".vert",
		assetPath.empty() ? "" : assetPath + ".frag",
		assetPath.empty() ? "" : assetPath + ".geom",
		true,
		formatCallback
	));
	return getShader(assetPath);
}

ci::gl::GlslProgRef ShaderManager::loadShader(const ShaderInfo info) {
	gl::GlslProg::Format format;

	try {
		if (!info.vertPath.empty()) {
			format.vertex(loadFile(info.vertPath));
		}
		if (!info.fragPath.empty()) {
			format.fragment(loadFile(info.fragPath));
		}
		if (!info.geomPath.empty()) {
			format.geometry(loadFile(info.geomPath));
		}
		if (info.formatCallback) {
			info.formatCallback(format);
		}
	} catch (Exception e) {
		CI_LOG_EXCEPTION("Could not load shader", e);
		return nullptr;
	}

	try {
		return ci::gl::GlslProg::create(format);

	} catch (Exception e) {
		CI_LOG_EXCEPTION("Could not create shader", e);
		return nullptr;
	}
}

}
}