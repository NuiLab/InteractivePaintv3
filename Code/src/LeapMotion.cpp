#include "LeapMotion.h"

LeapMotion::LeapMotion() {
	leftX = -10;
	leftY = -10;
	rightX = -10;
	rightY = -10;
	enableGestures();
}

LeapMotion::~LeapMotion() {

}

void LeapMotion::update(int width, int height) {
	if (controller.isConnected()) {
		updatePointerLocation(width, height);
		updateCurrentGesture();
	}
}

void LeapMotion::updatePointerLocation(int width, int height) {
	current = controller.frame();
	prev = controller.frame(1);
	InteractionBox iBox = current.interactionBox();
	if (!current.hands().isEmpty()) {
		Hand Lhand = current.hands().leftmost();
		Hand Rhand = current.hands().rightmost();
		Finger Lfinger = Lhand.fingers()[1];
		Finger Rfinger = Rhand.fingers()[1];
		Vector LnormalizedPoint = iBox.normalizePoint(Lfinger.stabilizedTipPosition(), false);
		Vector RnormalizedPoint = iBox.normalizePoint(Rfinger.stabilizedTipPosition(), false);
		leftX = LnormalizedPoint.x * width;
		leftY = (1 - LnormalizedPoint.y) * height;
		rightX = RnormalizedPoint.x * width;
		rightY = (1 - RnormalizedPoint.y) * height;
	}
}

void LeapMotion::updateCurrentGesture() {
	current = controller.frame();
	GestureList cg = current.gestures();
	currentGestures.clear();
	for (int i = 0; i < cg.count(); i++) {
		currentGestures.push_back(cg[i]);
	}
}

bool LeapMotion::isCircleGestureClockWise(Gesture gesture) {
	if (gesture.type() == Gesture::TYPE_CIRCLE) {
		CircleGesture circle = gesture;
		if (circle.pointable().direction().angleTo(circle.normal()) <= PI / 2) {
			return true;
		}
	}
	return false;
}

bool LeapMotion::isCircle(Gesture gesture) {
	return gesture.type() == Gesture::TYPE_CIRCLE;
}

bool LeapMotion::isSwipe(Gesture gesture) {
	return gesture.type() == Gesture::TYPE_SWIPE;
}

bool LeapMotion::isKeyTap(Gesture gesture) {
	return gesture.type() == Gesture::TYPE_KEY_TAP;
}

bool LeapMotion::isScreenTap(Gesture gesture) {
	return gesture.type() == Gesture::TYPE_SCREEN_TAP;
}

void LeapMotion::enableGestures() {
	controller.enableGesture(Gesture::TYPE_SWIPE);
	controller.enableGesture(Gesture::TYPE_CIRCLE);
	controller.enableGesture(Gesture::TYPE_SCREEN_TAP);
	controller.enableGesture(Gesture::TYPE_KEY_TAP);
}

int LeapMotion::getLeftX() {
	return leftX;
}

int LeapMotion::getLeftY() {
	return leftY;
}

int LeapMotion::getRightX() {
	return rightX;
}

int LeapMotion::getRightY() {
	return rightY;
}

vector<Gesture> LeapMotion::getCurrentGestures() {
	return currentGestures;
}