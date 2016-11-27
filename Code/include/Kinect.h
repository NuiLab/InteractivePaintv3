#pragma once
// Windows Header Files
#include <windows.h>
#include <objbase.h>
#include <iostream>
#include <vector>
#include "NuiApi.h"

using namespace std;

class Kinect {
public:
	Kinect();
	~Kinect();
	void update();
	int* getLeftHandX();
	int* getLeftHandY();
	int* getRightHandX();
	int* getRightHandY();

private:
	int leftHandX[6], leftHandY[6], rightHandX[6], rightHandY[6];
};