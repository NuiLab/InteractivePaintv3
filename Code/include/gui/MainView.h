#pragma once

#include <cinder/app/Window.h>
#include <cinder/app/App.h>
#include "cinder/gl/gl.h"

#include "../blocks/ciNanogui/ciNanoGui.h"
#include <list>
#include <stack>
#include <functional>

#include "DrawQueue.h"
#include "LeapMotion.h"

typedef std::shared_ptr<class MainView> ViewRef;

namespace nanogui {
	class ProgressBar;
};
using namespace ci::app;

class MainView : public nanogui::Screen {
public:
	MainView(std::shared_ptr<DrawQueue> dq, std::shared_ptr<LeapMotion> leap);
	~MainView();
	
	std::shared_ptr<DrawQueue> drawqueue;
	std::shared_ptr<LeapMotion> leap;

	void create(ci::app::WindowRef & ciWindow, const ci::fs::path & assetFolder);
	void updateColor(nanogui::Colour color);
	void draw(double time);
	void resize(glm::ivec2 size);

	// Mouse events
	bool mouseMove(MouseEvent e);
	bool mouseDown(MouseEvent e);
	bool mouseDrag(MouseEvent e);
	bool mouseUp(MouseEvent e);

	// Key events
	bool keyDown(KeyEvent e);
	bool keyUp(KeyEvent e);

	void getNextFrame();

	void updateToolFromGesture();

	void executeQueue();

	// Add function to the draw queue
	template<typename _Callable, typename... _Args>
	void QueueFunction(_Callable&& __f, _Args&&... __args) {
		std::function<void()> func = std::bind(std::forward<_Callable>(__f), std::forward<_Args>(__args)...);
		drawQueue.push_back(func);
	}

private:
	PerfGraph fps, cpuGraph, gpuGraph;
	GPUtimer gpuTimer;
	nanogui::ProgressBar * mProgress = nullptr;
	nanogui::FormHelper *gui;

	enum tools {
		none = 0,
		pencil,
		square,
		circle,
		line,
		text,
		LAST = line
	};

	map<tools, nanogui::Button> toggleButtons;

	tools tool;

	nanogui::Colour color;

	std::list<std::function<void()>> drawQueue;
	std::vector<ci::gl::FboRef> oldState;

	ci::Surface32fRef mSurface;

	ci::gl::FboRef mFbo;
	ci::gl::FboRef drawFbo;

	glm::ivec2 oldWindowSize;
	void drawCircleFromCenter(glm::vec2 center, float radius);
	void drawCenteredRectangle(glm::vec2 ciWindow, glm::vec2 size);
	void drawCircleAtPos(glm::vec2 pos);
	void drawLine(vec2 start, vec2 stop);
	void toggleTool(tools tool);
	void drawSquareFromCenter(glm::vec2 center, float radius);
	glm::vec2 canvasSize;

	struct toolData {
		tools activeTool;
		glm::vec2 initialMousePos;
		glm::vec2 finalMousePos;
		bool active = false;
		bool ready = initialMousePos.x > 0 && !active;
	} typedef toolData;

	toolData currentTool;

	gl::TextureFontRef textureFont;
	gl::TextureFont::Format textFormat;
};