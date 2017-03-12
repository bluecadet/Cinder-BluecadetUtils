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
	console() << "ShaderManager: Reloading all shaders..." << endl;

	for (auto & infoPair : mShaderInfoMap) {
		mShaderMap[infoPair.first] = loadShader(infoPair.second);
	}

	mReloadedSignal.emit();
}

gl::GlslProgRef ShaderManager::setupShader(const std::string key, const ShaderInfo info) {
	mShaderInfoMap[key] = info;
	auto shader = loadShader(info);
	mShaderMap[key] = shader;
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

ci::gl::GlslProgRef ShaderManager::getShaderByAssetPath(const std::string & assetPath){
	setupShader(assetPath, ShaderInfo(
		assetPath.empty() ? "" : assetPath + ".vert",
		assetPath.empty() ? "" : assetPath + ".frag",
		assetPath.empty() ? "" : assetPath + ".geom"
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