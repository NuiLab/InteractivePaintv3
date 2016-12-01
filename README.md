Interactive Paint is a showcase of what can be done using input devices such as the Intel RealSense, Leap Motion, Kinect and Multi-touch Display. It also serves as a framework for future projects involving these input devices.

Interactive Paint allows users to draw to a canvas just like Microsoft Paint but it allows you to do so with a variety of input devices. Drawing can be done with a mouse as well as a Multi-touch Display but it can also be done using the hand tracking functionality of these input devices.

This application can also be controlled using the various gestures supported by the input devices. While not all supported gestures are used in the application, they are being recognized by the application and can be given functionality in future iterations.

Kinect Guide

    The Kinect gave us multiple issues as only one of our Windows computers was able to recognize the Kinect and it didn't recognize it with good consistency. Not sure if it is an issue with my Kinect or the Microsoft SDK.
	Drawing -
		Drawing is performed by lifting your hand up in front of the Kinect and moving it around to draw.


Leap Motion Guide

	Drawing -
		Drawing is performed by lifting your hand over the Leap Motion and moving it around to draw.
	Gesture Recognition -
		The Leap Motion can recognized different hand gestures. While the component that handles input from the Leap Motion can recognize all different gestures only the swipe gesture is supported.
        Swipe your hand over the Leap Motion to change your currently selected tool.


Intel Real Sense Guide

	Drawing -
		Drawing is performed by lifting your hand up in front of the RealSense and moving it around to draw.
	Facial Expression Recognition -
		The RealSense provides facial expression recognition and our component that handles input from the RealSense can recognize facial expressions but currently our application does not use that.
