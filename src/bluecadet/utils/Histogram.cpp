#include "Histogram.h"

namespace bluecadet {
namespace utils {

Histogram::Histogram(int size) :
	mSize(size),
	mValues(size) {
	reset();
}

Histogram::~Histogram() {

}

}
}