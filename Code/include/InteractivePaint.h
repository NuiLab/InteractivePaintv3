#pragma once

#include "cinder/app/App.h"
#include "cinder/app/Event.h"
#include "cinder/app/RendererGl.h"
#include "cinder/CameraUi.h"
#include "cinder/gl/gl.h"
//#include <boost/property_tree/ptree.hpp>

#include "Resources.h"
#include "gui/MainView.h"

#include "Kinect.h"
#include "DrawQueue.h"
#include "LeapMotion.h"
#include "RealSense.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class InteractivePaintApp : public App {

public:
	InteractivePaintApp();
	~InteractivePaintApp();

	void setup() override;
	void resize() override;
	void update() override;
	void draw() override;

	void mouseMove(MouseEvent event) override;
	void mouseDown(MouseEvent event) override;
	void mouseDrag(MouseEvent event) override;
	void mouseUp(MouseEvent event) override;

	void keyDown(KeyEvent event) override;
	void keyUp(KeyEvent event) override;

	void touchesBegan(TouchEvent event);
	void touchesEnded(TouchEvent event);
	void touchesMoved(TouchEvent event);

private:
	ViewRef gui;
	
	std::vector<std::thread> threads;
	Kinect *kinect;
	std::shared_ptr<LeapMotion> leap;
	RealSense *realsense;
	std::shared_ptr<DrawQueue> drawqueue;
	bool usingGui = false;
	ivec2 mousePos;
	Color bgColor = Color(0.16f, 0.16f, 0.16f);
	void render();
	void cleanup() override;
};

CINDER_APP(InteractivePaintApp, RendererGl(RendererGl::Options().stencil().msaa(16)),
	[&](App::Settings * settings) {
	settings->setWindowSize(appWidth, appHeight);
	settings->setFrameRate(60.0f);
	settings->setTitle(getTitle());
	settings->setHighDensityDisplayEnabled();
	settings->setMultiTouchEnabled(true);
})