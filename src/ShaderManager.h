#pragma once
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"

namespace bluecadet {
namespace utils {

typedef std::shared_ptr<class ShaderManager> ShaderManagerRef;

class ShaderManager{

public:
	struct ShaderInfo {
		std::string vertPath = "";
		std::string fragPath = "";
		std::string geomPath = "";

		ShaderInfo(
			const std::string vertPath = "",
			const std::string fragPath = "",
			const std::string geomPath = "",
			const bool loadFromAssetsFolder = true
		) {
			if (loadFromAssetsFolder) {
				this->vertPath = vertPath.empty() ? "" : ci::app::getAssetPath(vertPath).string();
				this->fragPath = fragPath.empty() ? "" : ci::app::getAssetPath(fragPath).string();
				this->geomPath = geomPath.empty() ? "" : ci::app::getAssetPath(geomPath).string();
			} else {
				this->vertPath = vertPath;
				this->fragPath = fragPath;
				this->geomPath = geomPath;
			}
		};
	};

	static ShaderManagerRef getInstance(){
		static ShaderManagerRef instance = NULL;
		if (instance == NULL) {
			instance = ShaderManagerRef(new ShaderManager());
		}
		return instance;
	}

public:
	ShaderManager();
	~ShaderManager();

	// Triggered when shaders are reloaded
	ci::signals::Signal<void()>	& getReloadedSignal() { return mReloadedSignal; }

	// Reloads all previously set up shaders
	void reloadAll();
		
	// Sets up a shader based  on the shader info obj. Overwrites any existing shaders with the same key.
	ci::gl::GlslProgRef setupShader(const std::string key, const ShaderInfo info);

	// Checks whether a shader has been set up for `key` previously
	bool hasShader(const std::string & key) const;

	// Retursn the shader for `key` or nullptr if no shader for that key has been set up.
	ci::gl::GlslProgRef getShader(const std::string & key);
		
	//! Creates a shader form the file at filePath by appending .vert and .frag and caches the resulting GlslProgRef.
	//! Geom filters are tested for automatically and added if a file with the .geom extension at filePath is found.
	ci::gl::GlslProgRef getShaderByAssetPath(const std::string & assetPath);


private:
	ci::gl::GlslProgRef loadShader(const ShaderInfo info);

	std::map<std::string, ci::gl::GlslProgRef>	mShaderMap;
	std::map<std::string, ShaderInfo>			mShaderInfoMap;

	ci::signals::Signal<void()>					mReloadedSignal;
};
}
}