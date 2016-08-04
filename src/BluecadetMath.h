#pragma once

#include "cinder/app/App.h"
#include "BaseView.h"

namespace bluecadet {
namespace helpers {

class Math {
public:
	Math::Math() {};
	Math::~Math() {};

	static ci::vec2 Math::perToPix(const ci::vec2& percentPos = ci::vec2(0), const ci::vec2 resolution = ci::app::getWindowSize()) {
		ci::vec2 pixelPos;
		pixelPos = resolution * (percentPos / ci::vec2(100));
		return pixelPos;
	}

	static ci::vec2 Math::pixToPer(const ci::vec2& pixelPos = ci::vec2(0), const ci::vec2 resolution = ci::app::getWindowSize()) {
		ci::vec2 percentPos;
		percentPos = ci::vec2(100) * (pixelPos / resolution);
		return percentPos;
	}

	//returns a vec2 that is the mid point of the line between 
	static ci::vec2 Math::getMidpoint(const ci::vec2& point1, const ci::vec2& point2) {
		ci::vec2 midpoint;
		midpoint.x = (point1.x + point2.x) / 2;
		midpoint.y = (point1.y + point2.y) / 2;
		return midpoint;
	}

	//returns the distance between two points as a float
	static double Math::getDistance(const ci::vec2& point1, const ci::vec2& point2) {
		double distance = sqrt(abs(pow(point2.x - point1.x, 2.0) - pow(point2.y - point1.y, 2.0)));
		return distance;
	}

	static float Math::getDuration(const double& distance, const int& pixelsPerSecond) {
		float duration = (float)distance / (float)pixelsPerSecond;
		return duration;
	}

};

}
}