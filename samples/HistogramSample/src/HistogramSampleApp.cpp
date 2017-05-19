#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "bluecadet/utils/Histogram.h"

using namespace ci;
using namespace ci::app;
using namespace std;

using namespace bluecadet::utils;

class HistogramSampleApp : public App {
public:
	HistogramSampleApp();
	void mouseDrag(MouseEvent event) override;
	void draw() override;
	Histogram mHistogram;
};

HistogramSampleApp::HistogramSampleApp() :
	mHistogram(128)
{
}

void HistogramSampleApp::mouseDrag(MouseEvent event) {
	mHistogram.addNormalized((float)event.getX() / (float)getWindowWidth());
}

void HistogramSampleApp::draw() {
	gl::clear(Color(0, 0, 0));
	
	const float w = (float)getWindowWidth() / (float)mHistogram.size();
	const float maxH = (float)getWindowHeight();

	for (int i = 0; i < mHistogram.size(); ++i) {
		float h = maxH * mHistogram.getNormalizedValue(i);
		float x = (float)i * w;
		float y = maxH - h;

		gl::color(hsvToRgb(vec3((float)i / (float)mHistogram.size(), 1, 1)));
		gl::drawSolidRect(Rectf(x, y, x + w, y + h));
	}

	gl::drawString("Drag your cursor left/right", vec2(0, 0), Color::white(), Font("Arial", 64));
}

CINDER_APP(HistogramSampleApp, RendererGl)
