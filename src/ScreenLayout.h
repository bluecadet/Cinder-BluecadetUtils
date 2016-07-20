//+---------------------------------------------------------------------------
//  Bluecadet Interactive 2016
//  Contents: 
//  Comments: 
//----------------------------------------------------------------------------
#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "BaseView.h"

typedef std::shared_ptr<class ScreenLayout> ScreenLayoutRef;

class ScreenLayout : public bluecadet::views::BaseView {

public:
	ScreenLayout();
	~ScreenLayout();

	//! Singleton
	static ScreenLayoutRef getInstance() {
		static ScreenLayoutRef instance = nullptr;
		if (!instance) instance = ScreenLayoutRef(new ScreenLayout());
		return instance;
	}

	void setup();
	virtual void update(double deltaTime) override;
	virtual void draw() override;

	//! Draw outline around each display
	void drawDisplayBounds(const ci::ColorA& color = ci::ColorA(1.0f, 0.0f, 0.0f, 0.25f), const float& lineWidth = 2.0f);

	//! Display properties
	void setDisplayWidth(const int width) { mDisplayWidth = width; };
	int& getDisplayWidth() { return mDisplayWidth; };

	void setDisplayHeight(const int height) { mDisplayHeight = height; };
	int& getDisplayHeight() { return mDisplayHeight; };

	void setDisplayTotalRows(const int totalRows) { mDisplayTotalRows = totalRows; };
	int& getDisplayTotalRows() { return mDisplayTotalRows; };

	void setDisplayTotalColumns(const int totalColumns) { mDisplayTotalColumns = totalColumns; };
	int& getDisplayTotalColumns() { return mDisplayTotalColumns; };

protected:

	ci::Rectf getScreenBounds(int column, int row);

	//! Display
	int				mDisplayWidth;
	int				mDisplayHeight;
	int				mDisplayTotalRows;
	int				mDisplayTotalColumns;

private:

//	BaseViewRef		mDisplayOutlines;

};
