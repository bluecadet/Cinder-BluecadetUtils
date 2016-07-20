#include "ScreenLayout.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace bluecadet;
using namespace bluecadet::views;

ScreenLayout::ScreenLayout() : BaseView(){
}

ScreenLayout::~ScreenLayout(){
}

void ScreenLayout::setup() {
}

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

// Screen bounds in world space; 0-based col and row from top left to bottom right
ci::Rectf ScreenLayout::getScreenBounds(int column, int row) {
	return ci::Rectf(
		(float)(column * mDisplayWidth),
		(float)(row * mDisplayHeight),
		(float)((column + 1.0f) * mDisplayWidth),
		(float)((row + 1.0f) * mDisplayHeight));
}

void ScreenLayout::update(double deltaTime) {
}

void ScreenLayout::draw() {
}