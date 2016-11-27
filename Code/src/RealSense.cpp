#include "RealSense.h"

RealSense::RealSense() {
	sessionManager = PXCSenseManager::CreateInstance();
	sessionManager->EnableStream(PXCCapture::STREAM_TYPE_COLOR, 640, 480);

	sessionManager->EnableHandCursor();
	sessionManager->EnableFace();

	cursorAnalyzer = sessionManager->QueryHandCursor();
	faceAnalyzer = sessionManager->QueryFace();

	sessionManager->Init();
	
	//configuring hand cursor tracking
	cursorData = cursorAnalyzer->CreateOutput();
	cursorConfig = cursorAnalyzer->CreateActiveConfiguration();
	cursorConfig->ApplyChanges();

	//configuring face recognition
	faceData = faceAnalyzer->CreateOutput();
	faceConfig = faceAnalyzer->CreateActiveConfiguration();
	faceConfig->SetTrackingMode(PXCFaceConfiguration::TrackingModeType::FACE_MODE_COLOR_PLUS_DEPTH);
	faceConfig->QueryExpressions()->Enable();
	faceConfig->QueryExpressions()->EnableAllExpressions();
	faceConfig->ApplyChanges();

	leftBrowGesture = false;
	rightBrowGesture = false;
	smileGesture = false;
	kissGesture = false;
	leftCheekGesture = false;
	rightCheekGesture = false;
	tongueGesture = false;
}

RealSense::~RealSense() {

}

void RealSense::update(int width, int height) {
	if (sessionManager->AcquireFrame(true) >= PXC_STATUS_NO_ERROR) {
		updateCursor(width, height);
		updateFacialExpressions();
		
		sessionManager->ReleaseFrame();
	}
}

void RealSense::updateCursor(int width, int height) {
	cursorData->Update();
	PXCCursorData::ICursor* cursor;

	for (int i = 0; i < cursorData->QueryNumberOfCursors(); i++) {
		cursorData->QueryCursorData(PXCCursorData::ACCESS_ORDER_BY_TIME, i, cursor);
		x = width - ((cursor->QueryCursorImagePoint().x / 640) * width);
		y = (cursor->QueryCursorImagePoint().y / 480) * height;
	}
}

void RealSense::updateFacialExpressions() {
	faceData->Update();
	PXCFaceData::ExpressionsData *expressionData = nullptr;
	leftBrowGesture = false;
	rightBrowGesture = false;
	smileGesture = false;
	kissGesture = false;
	leftCheekGesture = false;
	rightCheekGesture = false;
	tongueGesture = false;
	for (pxcU16 i = 0; i <= faceData->QueryNumberOfDetectedFaces(); i++) {
		PXCFaceData::Face *trackedFace = faceData->QueryFaceByIndex(i);

		if (trackedFace != NULL) {
			expressionData = trackedFace->QueryExpressions();
			if (expressionData != NULL) {
				browDetection(expressionData);
				smileDetection(expressionData);
				kissDetection(expressionData);
				cheekDetection(expressionData);
				tongueDetection(expressionData);
			}
		}

	}

}

void RealSense::browDetection(PXCFaceData::ExpressionsData *expressionData) {
	PXCFaceData::ExpressionsData::FaceExpressionResult expressionLeftBrow;
	leftBrowGesture = expressionData->QueryExpression(PXCFaceData::ExpressionsData::EXPRESSION_BROW_RAISER_LEFT, &expressionLeftBrow);
	leftBrowGesture = leftBrowGesture & expressionLeftBrow.intensity == 100;
	PXCFaceData::ExpressionsData::FaceExpressionResult expressionRightBrow;
	rightBrowGesture = expressionData->QueryExpression(PXCFaceData::ExpressionsData::EXPRESSION_BROW_RAISER_RIGHT, &expressionRightBrow);
	rightBrowGesture = rightBrowGesture & expressionRightBrow.intensity == 100;
}

void RealSense::smileDetection(PXCFaceData::ExpressionsData *expressionData) {
	PXCFaceData::ExpressionsData::FaceExpressionResult expressionSmile;
	smileGesture = expressionData->QueryExpression(PXCFaceData::ExpressionsData::EXPRESSION_SMILE, &expressionSmile);
	smileGesture = smileGesture & expressionSmile.intensity > 25;
}

void RealSense::kissDetection(PXCFaceData::ExpressionsData *expressionData) {
	PXCFaceData::ExpressionsData::FaceExpressionResult expressionKiss;
	kissGesture = expressionData->QueryExpression(PXCFaceData::ExpressionsData::EXPRESSION_KISS, &expressionKiss);
	kissGesture = kissGesture & expressionKiss.intensity == 100;
}

void RealSense::cheekDetection(PXCFaceData::ExpressionsData *expressionData) {
	PXCFaceData::ExpressionsData::FaceExpressionResult expressionLeftCheek;
	leftCheekGesture = expressionData->QueryExpression(PXCFaceData::ExpressionsData::EXPRESSION_PUFF_LEFT, &expressionLeftCheek);
	leftCheekGesture = leftCheekGesture & expressionLeftCheek.intensity == 100;
	PXCFaceData::ExpressionsData::FaceExpressionResult expressionRightCheek;
	rightCheekGesture = expressionData->QueryExpression(PXCFaceData::ExpressionsData::EXPRESSION_PUFF_RIGHT, &expressionRightCheek);
	rightCheekGesture = rightCheekGesture & expressionRightCheek.intensity == 100;
}

void RealSense::tongueDetection(PXCFaceData::ExpressionsData *expressionData) {
	PXCFaceData::ExpressionsData::FaceExpressionResult expressionTongue;
	tongueGesture = expressionData->QueryExpression(PXCFaceData::ExpressionsData::EXPRESSION_TONGUE_OUT, &expressionTongue);
	tongueGesture = tongueGesture & expressionTongue.intensity == 100;
}

bool RealSense::getLeftBrowGesture() {
	return leftBrowGesture;
}

bool RealSense::getRightBrowGesture() {
	return rightBrowGesture;
}

bool RealSense::getSmileGesture() {
	return smileGesture;
}

bool RealSense::getKissGesture() {
	return kissGesture;
}

bool RealSense::getLeftCheekGesture() {
	return leftCheekGesture;
}

bool RealSense::getRightCheekGesture() {
	return rightCheekGesture;
}

bool RealSense::getTongueGesture() {
	return tongueGesture;
}

int RealSense::getX() {
	return x;
}

int RealSense::getY() {
	return y;
}