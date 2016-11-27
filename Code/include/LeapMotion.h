#pragma once
#include <iostream>
#include <string.h>
#include "Leap.h"

using namespace Leap;
using namespace std;

class LeapMotion {
public:
	LeapMotion();
	~LeapMotion();
	void update(int width, int height);
	int getLeftX();
	int getLeftY();
	int getRightX();
	int getRightY();
	bool isCircleGestureClockWise(Gesture gesture);
	bool isCircle(Gesture gesture);
	bool isSwipe(Gesture gesture);
	bool isKeyTap(Gesture gesture);
	bool isScreenTap(Gesture gesture);
	vector<Gesture> getCurrentGestures();
private:
	Controller controller;
	Frame current, prev;
	vector<Gesture> currentGestures;
	void enableGestures();
	void updateCurrentGesture();
	void updatePointerLocation(int width, int height);
	int leftX, leftY, rightX, rightY;
};
