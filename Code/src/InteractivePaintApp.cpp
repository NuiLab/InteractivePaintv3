#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "InteractivePaint.h"

#include "NanoguiModule/nanogui/screen.h"
#include "Util.h"



#include <thread>
using namespace ci;
using namespace ci::app;
using namespace std;

InteractivePaintApp::InteractivePaintApp() : gui(nullptr) {

}
InteractivePaintApp::~InteractivePaintApp() {

};

void InteractivePaintApp::setup() {
	Util::ensureDirectory("icons");
	fs::path p = getAssetPath("icons");
	drawqueue = std::make_shared<DrawQueue>();
	kinect = new Kinect();
	leap = std::make_shared<LeapMotion>();
	realsense = new RealSense();
	drawqueue = ::shared_ptr<DrawQueue>(new DrawQueue);
	auto wnd = getWindow();
	gui = std::make_shared<MainView>(drawqueue, leap);
	gui->create(wnd, p.parent_path());

}

void InteractivePaintApp::mouseMove(MouseEvent event) {
	mousePos = event.getPos();
	gui->mouseMove(event);
}

void InteractivePaintApp::mouseDown(MouseEvent event) {
	usingGui = gui->mouseDown(event);
	// don't update the canvas if we are using the GUI controls
}

void InteractivePaintApp::mouseUp(MouseEvent event) {
	gui->mouseUp(event);
}

void InteractivePaintApp::mouseDrag(MouseEvent event) {
	// don't update cameraUI if we're using the gui
	drawqueue->add(event.getPos());
	gui->mouseDrag(event);
}

void InteractivePaintApp::update() {
	leap->update(1280, 720);
	realsense->update(1280, 720);
	//console() << "X  " + std::to_string(realsense->getX()) + "   Y  " + std::to_string(realsense->getY()) << std::endl;
	drawqueue->add(vec2(realsense->getX(), realsense->getY()));
	drawqueue->add(vec2(leap->getLeftX(), leap->getLeftY()));
	drawqueue->add(vec2(leap->getRightX(), leap->getRightY()));
	//kinect->updateHandLocations();
	//drawqueue->add(vec2(kinect->getLeftHandX()[0], kinect->getLeftHandY()[0]));
	//drawqueue->add(vec2(kinect->getRightHandX()[0], kinect->getRightHandY()[0]));
}

void InteractivePaintApp::draw() {
	render();
	gui->draw(getElapsedSeconds());
}

void InteractivePaintApp::resize() {
	gui->resize(getWindowSize());
}

void InteractivePaintApp::keyDown(KeyEvent event) {
	switch (event.getCode()) {
		// Escape to exit
		case KeyEvent::KEY_ESCAPE:
			quit();
			break;
			// f for fullscreen
		case KeyEvent::KEY_f:
			setFullScreen(!isFullScreen());
			break;
	}

	gui->keyDown(event);
}

void InteractivePaintApp::keyUp(KeyEvent event) {
}

void InteractivePaintApp::touchesBegan(TouchEvent event) {
}

void InteractivePaintApp::touchesEnded(TouchEvent event) {
}

void InteractivePaintApp::touchesMoved(TouchEvent event) {
	for (const auto &touch : event.getTouches()) {
		drawqueue->add(touch.getPos());
	}
}

void InteractivePaintApp::render() {
	gl::clear(bgColor);
	drawqueue->execute();
	gl::pushMatrices();
}

void InteractivePaintApp::cleanup() {

}
