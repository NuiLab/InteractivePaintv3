#include "Kinect.h"

Kinect::Kinect() {
	NuiInitialize(NUI_INITIALIZE_FLAG_USES_SKELETON);
}

void Kinect::update() {
	NUI_SKELETON_FRAME frame;
	NuiSkeletonGetNextFrame(0, &frame);
	int i = 0;
	//for (int i = 0; i < 6; i++) //kinect can track 6 people
	//{
		if (frame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED) {
			leftHandX[i] = frame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_LEFT].x;
			leftHandY[i] = frame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_LEFT].y;
			rightHandX[i] = frame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT].x;
			rightHandY[i] = frame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT].y;
		}
	//}
	system("cls");//Clear the screen
}

Kinect::~Kinect() {
	NuiShutdown();
}

int* Kinect::getLeftHandX() {
	return leftHandX;
}

int* Kinect::getLeftHandY() {
	return leftHandY;
}

int* Kinect::getRightHandX() {
	return rightHandX;
}

int* Kinect::getRightHandY() {
	return rightHandY;
}