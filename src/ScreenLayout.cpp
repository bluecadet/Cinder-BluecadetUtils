#include "ScreenLayout.h"
#include "SettingsManager.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace utils {

ScreenLayout::ScreenLayout() :
	mDisplayWidth(1920),
	mDisplayHeight(1080),
	mDisplayTotalRows(1),
	mDisplayTotalColumns(1)
{
}

ScreenLayout::~ScreenLayout() {
}

void ScreenLayout::setup() {
	// Load display settings. Don't pass the json and it will use the default appSettings.json
	SettingsManager::getInstance()->setFieldFromJson(&mDisplayWidth, "settings.display.width");
	SettingsManager::getInstance()->setFieldFromJson(&mDisplayHeight, "settings.display.height");
	SettingsManager::getInstance()->setFieldFromJson(&mDisplayTotalRows, "settings.display.totalColumns");
	SettingsManager::getInstance()->setFieldFromJson(&mDisplayTotalColumns, "settings.display.totalRows");
}

//void ScreenLayout::update(double deltaTime) {
//}

//void ScreenLayout::draw() {
//}

void ScreenLayout::drawDisplayBounds(const ci::ColorA& color, const float& lineWidth) {
	ci::gl::ScopedColor scopedColor;

	ci::gl::color(color);
	ci::gl::lineWidth(lineWidth);

	for (int row = 0; row < mDisplayTotalRows; ++row) {
		for (int col = 0; col < mDisplayTotalColumns; ++col) {
			ci::Rectf bounds = getScreenBounds(col, row);
			ci::gl::drawStrokedRect(bounds);
		}
	}
}

// Returns bounds of single screen at column/row as rectangle dimensions
ci::Rectf ScreenLayout::getScreenBounds(int column, int row) {
	return ci::Rectf(
		(float)(column * mDisplayWidth),
		(float)(row * mDisplayHeight),
		(float)((column + 1.0f) * mDisplayWidth),
		(float)((row + 1.0f) * mDisplayHeight));
}

}
}