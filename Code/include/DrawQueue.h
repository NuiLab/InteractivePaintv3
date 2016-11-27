#pragma once

#include "cinder/gl/gl.h"

#include <thread>

using namespace ci;

class DrawQueue {
public:
	void DrawQueue::execute();
	void DrawQueue::add(vec2 point);
private:
	ci::gl::FboRef mFbo;
	std::vector<vec2> drawVecs;
	std::vector<std::thread> threads;
};