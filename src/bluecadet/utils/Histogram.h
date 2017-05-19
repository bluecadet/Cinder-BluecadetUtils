#pragma once

#include <memory>
#include <vector>
#include <algorithm>

namespace bluecadet {
namespace utils {

typedef std::shared_ptr<class Histogram> HistogramRef;

class Histogram {

public:
	Histogram(int size);
	~Histogram();

	inline void reset();
	inline void add(const int index);

	inline int minIndex();
	inline int maxIndex();

	inline int minValue();
	inline int maxValue();

	inline void addNormalized(const float normalizedIndex);
	inline float getNormalizedValue(const size_t index);

	inline int size() const;
	inline int at(const size_t index) const;

	inline const std::vector<int> & values() const;

	int & operator[](const size_t index);
	const int & operator[](const size_t index) const;

private:
	inline void compute();

	bool mDirty = true;

	int mMinValue = 0;
	int mMaxValue = 0;
	int mMinIndex = 0;
	int mMaxIndex = 0;

	int mSize;
	std::vector<int> mValues;
};


//==================================================
// Inline implementations
// 

inline int Histogram::maxIndex() {
	if (mDirty) compute();
	return mMaxIndex;
}

inline int Histogram::minIndex() {
	if (mDirty) compute();
	return mMinIndex;
}

inline int Histogram::maxValue() {
	if (mDirty) compute();
	return mMaxValue;
}

inline int Histogram::minValue() {
	if (mDirty) compute();
	return mMinValue;
}

inline void Histogram::add(const int index) {
	mValues[std::max(0, std::min(mSize - 1, index))]++;
	mDirty = true;
}

inline void Histogram::addNormalized(const float normalizedIndex) {
	const int index = (int)std::floor(normalizedIndex * (float)mSize);
	mValues[std::max(0, std::min(mSize - 1, index))]++;
	mDirty = true;
}

inline float Histogram::getNormalizedValue(const size_t index) {
	if (mDirty) compute();
	return (float)mValues[index] / (float)mMaxValue;
}

inline int Histogram::size() const {
	return mSize;
}

inline int Histogram::at(const size_t index) const {
	return mValues[index];
}

inline const std::vector<int>& Histogram::values() const {
	return mValues;
}

inline int & Histogram::operator[](const size_t index) {
	return mValues[index];
}

inline const int & Histogram::operator[](const size_t index) const {
	return mValues[index];
}

inline void Histogram::compute() {
	mMinIndex = 0;
	mMaxIndex = 0;
	mMinValue = INT_MAX;
	mMaxValue = INT_MIN;

	for (int i = 0; i < mSize; ++i) {
		int v = mValues[i];
		if (v > mMaxValue) {
			mMaxIndex = i;
			mMaxValue = v;

		}
		if (v < mMinValue) {
			mMinIndex = i;
			mMinValue = v;
		}
	}

	mDirty = false;
}

inline void Histogram::reset() {
	for (int i = 0; i < mSize; ++i) {
		mValues[i] = 0;
	}
	mDirty = true;
}

}
}
