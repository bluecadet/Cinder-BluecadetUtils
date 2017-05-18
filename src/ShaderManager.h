#pragma once
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"

namespace bluecadet {
namespace utils {

typedef std::shared_ptr<class ShaderManager> ShaderManagerRef;

class ShaderManager{

public:
	struct ShaderInfo {
		typedef std::function<void(ci::gl::GlslProg::Format & format)> FormatCallback;

		std::string vertPath = "";
		std::string fragPath = "";
		std::string geomPath = "";
		FormatCallback formatCallback = nullptr;

		ShaderInfo(
			const std::string vertPath = "",
			const std::string geomPath = "",
			const std::string fragPath = "",
			const bool loadFromAssetsFolder = true,
			const FormatCallback formatCallback = nullptr
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
			this->formatCallback = formatCallback;
		};

		ShaderInfo(
			const std::string vertPath,
			const std::string fragPath,
			const bool loadFromAssetsFolder = true,
			const FormatCallback formatCallback = nullptr
		) : ShaderInfo(vertPath, "", fragPath, loadFromAssetsFolder, formatCallback) {
		};
	};

	static ShaderManagerRef getInstance(){
		static ShaderManagerRef instance = nullptr;
		if (instance == nullptr) {
			instance = ShaderManagerRef(new ShaderManager());
		}
		return instance;
	}

public:
	ShaderManager();
	~ShaderManager();

	// Triggered when shaders are reloaded
	ci::signals::Signal<void()>	& getReloadAllSignal() { return mDidReloadAll; }
	ci::signals::Signal<void(std::string key, ci::gl::GlslProgRef shader)>	& getShaderUpdatedSignal() { return mDidUpdateShader; }

	// Reloads all previously set up shaders
	void reloadAll();
		
	// Sets up a shader based  on the shader info obj. Overwrites any existing shaders with the same key.
	ci::gl::GlslProgRef setupShader(const std::string key, const ShaderInfo info);

	// Checks whether a shader has been set up for `key` previously
	bool hasShader(const std::string & key) const;

	// Returns the shader for `key` or nullptr if no shader for that key has been set up.
	ci::gl::GlslProgRef getShader(const std::string & key);
		
	//! Creates a shader form the file at filePath by appending .vert and .frag and caches the resulting GlslProgRef.
	//! Geom filters are tested for automatically and added if a file with the .geom extension at filePath is found.
	ci::gl::GlslProgRef getShaderByAssetPath(const std::string & assetPath, ShaderInfo::FormatCallback formatCallback = nullptr);


private:
	ci::gl::GlslProgRef loadShader(const ShaderInfo info);

	std::map<std::string, ci::gl::GlslProgRef>	mShaderMap;
	std::map<std::string, ShaderInfo>			mShaderInfoMap;

	ci::signals::Signal<void()>					mDidReloadAll;
	ci::signals::Signal<void(std::string key, ci::gl::GlslProgRef shader)>	mDidUpdateShader;
};
}
}