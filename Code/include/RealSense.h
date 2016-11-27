#pragma once

#include "pxcfacedata.h"
#include "pxccursorconfiguration.h"
#include "pxccursordata.h"
#include "pxchandcursormodule.h"
#include "pxcfaceconfiguration.h"
#include "pxcdefs.h"
#include "pxcsensemanager.h"
#include "pxcfacemodule.h"

using namespace std;

class RealSense {
public:
	RealSense();
	~RealSense();
	void update(int width, int height);
	int getX();
	int getY();
	bool getLeftBrowGesture();
	bool getRightBrowGesture();
	bool getSmileGesture();
	bool getTongueGesture();
	bool getKissGesture();
	bool getLeftCheekGesture();
	bool getRightCheekGesture();

private:
	void updateCursor(int width, int height);
	void updateFacialExpressions();

	void browDetection(PXCFaceData::ExpressionsData *expressionData);
	void smileDetection(PXCFaceData::ExpressionsData *expressionData);
	void tongueDetection(PXCFaceData::ExpressionsData *expressionData);
	void kissDetection(PXCFaceData::ExpressionsData *expressionData);
	void cheekDetection(PXCFaceData::ExpressionsData *expressionData);

	PXCSenseManager *sessionManager = 0;

	PXCCursorConfiguration *cursorConfig;
	PXCHandCursorModule *cursorAnalyzer = 0;
	PXCCursorData *cursorData;

	PXCFaceModule *faceAnalyzer;
	PXCFaceData *faceData;
	PXCFaceConfiguration *faceConfig;

	int x, y;
	bool kissGesture, leftBrowGesture, rightBrowGesture, leftCheekGesture, rightCheekGesture, smileGesture, tongueGesture;
};