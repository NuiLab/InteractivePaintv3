#include "gui/MainView.h"
#include <cinder/gl/Context.h>
#include <cinder/gl/scoped.h>
#include <cinder/gl/Fbo.h>
#include <cinder/GeomIo.h>
#include <string>
#include <cmath>

using namespace nanogui;
using namespace ci;
using std::cout;
using std::cerr;
using std::endl;

MainView::MainView(std::shared_ptr<DrawQueue> dq, std::shared_ptr<LeapMotion> leap)
	: nanogui::Screen() {

	this->leap = leap;

	drawqueue = dq;
	mTheme = new Theme(mNVGContext);

	color = nanogui::Colour(0.5f, 0.5f, 0.7f, 1.f);

	// Only save 10 old FrameBuffers
	oldState.resize(10);

	textFormat.enableMipmapping();
	textureFont = gl::TextureFont::create(ci::Font("Arial", 24), textFormat);
}

// destructor
MainView::~MainView() {
}

void MainView::create(WindowRef &ciWindow, const fs::path &assetFolder) {
	try {
		oldWindowSize = ciWindow->getSize();
		mSurface = Surface32f::create(ciWindow->getSize().x, ciWindow->getSize().y, true);
		mFbo = gl::Fbo::create(ciWindow->getSize().x, ciWindow->getSize().y);
		drawFbo = gl::Fbo::create(ciWindow->getSize().x, ciWindow->getSize().y, 0.01);
		setSize(ciWindow->getSize());

		gui = new FormHelper(this);
		nanogui::ref<nanogui::Window> window = gui->addWindow(ivec2(15, 15), "Interactive Paint");

		window->setLayout(new BoxLayout(Orientation::Vertical, Alignment::Minimum, 15, 10));
		window->setFixedSize(ivec2(150, 250));
		//gui->setFixedSize(ivec2(200, 300));

		Widget *twoButtons = new Widget(window);
		twoButtons->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Minimum, 0, 10));

		Button *newButton = new Button(twoButtons, "New");
		newButton->setCallback([=] {
			nanogui::ref<nanogui::Window> mWin = gui->addWindow(ivec2(15, 15), "New canvas creation");
			mWin->setModal(true);
			mWin->setLayout(new BoxLayout(Orientation::Vertical, Alignment::Middle, 10, 10));
			Widget * panel1 = new Widget(mWin);
			panel1->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Middle, 10, 40));

			//Label * iconLabel = new Label(panel1, std::string(utf8(icon).data()), "icons");
			//iconLabel->setFontSize(50);

			ci::Area mArea = ci::Area(ivec2(ciWindow->getWidth()*(.5*sqrt(2)), ciWindow->getHeight()*(.5*sqrt(2))), ivec2(ciWindow->getWidth() - ciWindow->getWidth()*(.5*sqrt(2)), ciWindow->getHeight() - ciWindow->getHeight()*(.5*sqrt(2))));
			vec2 defaultSize = vec2(mArea.getWidth(), mArea.getHeight());

			Label *widthLabel = new Label(panel1, "Width:");
			//widthLabel->setFixedWidth(200);
			TxtBox *width = new TxtBox(panel1);
			width->setEditable(true);
			width->setFixedWidth(100);
			char buf[10];
			sprintf_s(buf, sizeof(buf), "%.02f", defaultSize.x);
			width->setValue(buf);
			width->setUnits("px");

			Label *heightLabel = new Label(panel1, "Height:");
			//heightLabel->setFixedWidth(200);
			TxtBox *height = new TxtBox(panel1);
			height->setEditable(true);
			height->setFixedWidth(100);
			sprintf_s(buf, sizeof(buf), "%.02f", defaultSize.y);
			height->setValue(buf);
			height->setUnits("px");

			Widget *panel2 = new Widget(mWin);
			panel2->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 15));
			Button *cancelButton = new Button(panel2, "Cancel", ENTYPO_ICON_CROSS);
			cancelButton->setCallback([mWin]() mutable {
				mWin->dispose();
			});

			Button *doneButton = new Button(panel2, "Done", ENTYPO_ICON_CHECK);
			doneButton->setCallback([=]() mutable {
				auto test = height->value();
				app::console() << test << endl;
				mWin->dispose();
				canvasSize = vec2(atof(width->value().c_str()), atof(height->value().c_str()));
				QueueFunction([=]() mutable {
					gl::translate(getWindowCenter());
					drawCenteredRectangle(ciWindow->getSize(), vec2(atof(width->value().c_str()), atof(height->value().c_str())));
				});
			});
			mWin->center();
			requestFocus();
		});

		Button *saveButton = new Button(twoButtons, "Save");
		saveButton->setCallback([=] {
			std::vector<std::string> extensions;

			extensions.push_back(".png");
			extensions.push_back(".jpg");
			extensions.push_back(".svg");

			fs::path p = ci::app::getSaveFilePath("", extensions);

			app::console() << "Selected path: " << p << endl;

			writeImage(p, Surface8u(mFbo->getColorTexture()->createSource()));
		});

		gui->addButton("Open", [=]() {
			try {
				fs::path p = getOpenFilePath("", ImageIo::getLoadExtensions());
				gl::TextureRef myImage;
				if (!p.empty()) { // an empty string means the user canceled
					myImage = gl::Texture::create(loadImage(p));
					//QueueFunction([&]() {
					drawFbo->bindFramebuffer();
					gl::draw(myImage);
					drawFbo->unbindFramebuffer();
					//});
				}
			}
			catch (Exception &e) {

			}
		});

		Widget *colorLabelPicker = new Widget(window);
		colorLabelPicker->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Minimum, 0, 5));

		Label *colorLabel = new Label(colorLabelPicker, "Choose a color");

		ColorPicker *mColorPicker = new ColorPicker(colorLabelPicker, color);
		mColorPicker->setCallback([=](Colour newColor) {
			color = newColor;
		});

		Widget *toolsWidget = new Widget(window);
		toolsWidget->setLayout(new GroupLayout(0, 0, 0, 0));
		new Label(toolsWidget, "Tools:");
		Widget *tools = new Widget(window);
		tools->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Minimum, 0, 5));
		Button *pencilButton = new ToolButton(tools, ENTYPO_ICON_PENCIL);
		Button *rotateClockwiseButton = new ToolButton(tools, ENTYPO_ICON_CW);
		Button *rotateCounterButton = new ToolButton(tools, ENTYPO_ICON_CCW);

		// Second row
		Widget *tools2 = new Widget(window);
		tools2->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Minimum, 0, 5));
		Button *squareButton = new ToolButton(tools2, ENTYPO_ICON_STOP);
		Button *circleButton = new ToolButton(tools2, ENTYPO_ICON_RECORD);
		Button *textButton = new ToolButton(tools2, ENTYPO_ICON_TEXT_DOC);
		Button *lineButton = new ToolButton(tools2, ENTYPO_ICON_RIGHT_THIN); // Temp icon

		toggleButtons.insert(std::make_pair(square, *squareButton));
		toggleButtons.insert(std::make_pair(circle, *circleButton));
		toggleButtons.insert(std::make_pair(line, *lineButton));
		toggleButtons.insert(std::make_pair(pencil, *pencilButton));
		toggleButtons.insert(std::make_pair(text, *textButton));
		//toggleButtons.insert(std::make_pair<tools, Button>("rotateCCW", squareButton));

		pencilButton->setCallback([=]() {
			toggleTool(pencil);
		});

		rotateClockwiseButton->setCallback([=]() {
			QueueFunction([=]() {
				mFbo->bindFramebuffer();

				gl::translate(ciWindow->getSize().x / 2, ciWindow->getSize().y / 2, 0); // M1 - 2nd translation
				gl::rotate(M_PI / 2, 0, 0, 1);
				gl::translate(-ciWindow->getSize().x / 2, -ciWindow->getSize().y / 2, 0);  // M3 - 1st translation

				mFbo = gl::Fbo::create(mFbo->getSize().x, mFbo->getSize().y);

				mFbo->unbindFramebuffer();
			});
		});

		rotateCounterButton->setCallback([=]() {
			QueueFunction([=]() {
				mFbo->bindFramebuffer();

				gl::translate(ciWindow->getSize().x / 2, ciWindow->getSize().y / 2, 0); // M1 - 2nd translation
				gl::rotate(-M_PI / 2, 0, 0, 1);
				gl::translate(-ciWindow->getSize().x / 2, -ciWindow->getSize().y / 2, 0);  // M3 - 1st translation

				mFbo = gl::Fbo::create(mFbo->getSize().x, mFbo->getSize().y);

				mFbo->unbindFramebuffer();
			});
		});

		squareButton->setCallback([=]() {
			toggleTool(square);
		});

		circleButton->setChangeCallback([=](bool toggle) {
			toggleTool(circle);
		});

		lineButton->setChangeCallback([=](bool toggle) {
			toggleTool(line);
		});

		textButton->setChangeCallback([=](bool toggle) {
			toggleTool(text);
		});

		performLayout(mNVGContext);
	}
	catch (const std::exception & e) {
		std::cout << e.what() << std::endl;
	}
}

void MainView::updateColor(nanogui::Colour mColor) {
	/*cout << "Color: " << mColor.r << ", " << mColor.g << ", " << mColor.b << endl;
	//mShader.bind();
	//mShader.setUniform("intensity", vec3(color.r(), color.g(), color.b()));*/
}

bool MainView::mouseMove(MouseEvent e) {
	return cursorPosCallbackEvent(e.getPos().x, e.getPos().y);
}

bool MainView::mouseDown(MouseEvent e) {
	if (!e.isLeft()) return false;
	switch (currentTool.activeTool) {
		case pencil: {
			QueueFunction([=]() {
				mFbo->bindFramebuffer();
				////gl::setMatricesWindow(mFbo->getSize(), false);
				gl::ScopedColor sc;
				gl::color(color);
				drawCircleAtPos(vec2(e.getPos().x, e.getPos().y));
				mFbo->unbindFramebuffer();
			});
			break;
		}
		case square:
			currentTool.initialMousePos = e.getPos();
			break;
		case circle:
			currentTool.initialMousePos = e.getPos();
			break;
		case line:
			currentTool.initialMousePos = e.getPos();
			break;
		case text:
			currentTool.initialMousePos = e.getPos();
			break;
	}
	return mouseButtonCallbackEvent(MOUSE_BUTTON_LEFT, PRESS, 0);
}

bool MainView::mouseDrag(MouseEvent e) {
	if (!e.isLeftDown()) return false;
	switch (currentTool.activeTool) {
		case pencil: {
			QueueFunction([=]() {
				mFbo->bindFramebuffer();
				////gl::setMatricesWindow(mFbo->getSize(), false);
				gl::ScopedColor sc;
				gl::color(color);
				drawCircleAtPos(vec2(e.getPos().x, e.getPos().y));
				mFbo->unbindFramebuffer();
			});
			break;
		}
		case square: {
			float hyp = sqrt(pow(currentTool.initialMousePos.x, 2) + pow(currentTool.initialMousePos.y, 2));
			float vector = sqrt(pow(e.getX(), 2) + pow(e.getY(), 2));
			QueueFunction([=]() {
				drawFbo->bindFramebuffer();
				gl::clear(ColorA(0, 0, 0, 0.01));
				glEnable(GL_BLEND);
				glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

				drawSquareFromCenter(currentTool.initialMousePos, sqrt(pow(currentTool.initialMousePos.x - e.getX(), 2) + pow(currentTool.initialMousePos.y - e.getY(), 2)));
				drawFbo->unbindFramebuffer();
				glDisable(GL_BLEND);
			});
			break;
		}
		case circle: {
			float vector = sqrt(pow(currentTool.initialMousePos.x - e.getX(), 2) + pow(currentTool.initialMousePos.y - e.getY(), 2));
			QueueFunction([=]() {
				drawFbo->bindFramebuffer();
				gl::clear(ColorA(0, 0, 0, 0.01));
				glEnable(GL_BLEND);
				glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

				drawCircleFromCenter(currentTool.initialMousePos, vector);
				drawFbo->unbindFramebuffer();
				glDisable(GL_BLEND);
			});
			break;
		}
		case line: {
			float vector = sqrt(pow(currentTool.initialMousePos.x - e.getX(), 2) + pow(currentTool.initialMousePos.y - e.getY(), 2));
			QueueFunction([=]() {
				drawFbo->bindFramebuffer();
				gl::clear(ColorA(0, 0, 0, 0.01));
				glEnable(GL_BLEND);
				glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
				drawLine(currentTool.initialMousePos, e.getPos());
				drawFbo->unbindFramebuffer();
				glDisable(GL_BLEND);
			});
			break;
		}
	}
	return cursorPosCallbackEvent(e.getPos().x, e.getPos().y);
}

bool MainView::mouseUp(MouseEvent e) {
	if (!e.isLeft()) return false;
	if (currentTool.initialMousePos != vec2(0, 0)) {
		switch (currentTool.activeTool) {
			case square: {
				currentTool.finalMousePos = e.getPos();
				// draw the final square to the main fbo
				mFbo->bindFramebuffer();
				float vector = sqrt(pow(currentTool.initialMousePos.x - e.getX(), 2) + pow(currentTool.initialMousePos.y - e.getY(), 2));
				drawSquareFromCenter(currentTool.initialMousePos, vector);
				mFbo->unbindFramebuffer();
				drawFbo->bindFramebuffer();
				gl::clear(ColorA(0, 0, 0, 0.01));
				drawFbo->unbindFramebuffer();
				currentTool.active = false;
				break;
			}
			case circle: {
				currentTool.finalMousePos = e.getPos();
				mFbo->bindFramebuffer();
				float vector = sqrt(pow(currentTool.initialMousePos.x - e.getX(), 2) + pow(currentTool.initialMousePos.y - e.getY(), 2));
				drawCircleFromCenter(currentTool.initialMousePos, vector);
				mFbo->unbindFramebuffer();
				drawFbo->bindFramebuffer();
				gl::clear(ColorA(0, 0, 0, 0.01));
				drawFbo->unbindFramebuffer();
				break;
			}
			case line: {
				currentTool.finalMousePos = e.getPos();
				mFbo->bindFramebuffer();
				drawLine(currentTool.initialMousePos, e.getPos());
				mFbo->unbindFramebuffer();
				drawFbo->bindFramebuffer();
				gl::clear(ColorA(0, 0, 0, 0.01));
				drawFbo->unbindFramebuffer();
				break;
			}
			case text: {
				if (currentTool.ready) {
					currentTool.active = true;
					currentTool.finalMousePos = e.getPos();
					nanogui::ref<nanogui::Window> mWin = gui->addWindow(ivec2(15, 15), "New canvas creation");
					mWin->setModal(true);
					mWin->setLayout(new BoxLayout(Orientation::Vertical, Alignment::Middle, 10, 10));
					Widget * panel1 = new Widget(mWin);
					panel1->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Middle, 10, 40));

					//Label * iconLabel = new Label(panel1, std::string(utf8(icon).data()), "icons");
					//iconLabel->setFontSize(50);

					Label *textLabel = new Label(panel1, "Text:");
					//widthLabel->setFixedWidth(200);
					TxtBox *text = new TxtBox(panel1);
					text->setEditable(true);
					text->setFixedWidth(100);
					char buf[10];
					sprintf_s(buf, sizeof(buf), "%s", "Text here");
					text->setValue(buf);
					text->setUnits("");

					Widget *panel2 = new Widget(mWin);
					panel2->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 15));
					Button *cancelButton = new Button(panel2, "Cancel", ENTYPO_ICON_CROSS);
					cancelButton->setCallback([=]() mutable {
						mWin->dispose();
						currentTool.active = false;
					});

					Button *doneButton = new Button(panel2, "Done", ENTYPO_ICON_CHECK);
					doneButton->setCallback([=]() mutable {
						mWin->dispose();
						QueueFunction([=]() mutable {
							mFbo->bindFramebuffer();
							textureFont->drawString(text->value(), vec2(currentTool.finalMousePos.x, currentTool.finalMousePos.y));
							mFbo->unbindFramebuffer();
						});
						currentTool.active = false;
					});
					mWin->center();
					requestFocus();
				}
				break;
			}
		}
	}
	return mouseButtonCallbackEvent(MOUSE_BUTTON_LEFT, RELEASE, 0);
}

bool MainView::keyDown(KeyEvent e) {
	if (e.getChar() > 32)
		charCallbackEvent(e.getChar());
	return keyCallbackEvent(e.getCode(), 0, PRESS, 0);
}

bool MainView::keyUp(KeyEvent e) {
	return false;
}

void MainView::draw(double time) {
	getNextFrame();
	updateToolFromGesture();
	nanogui::Screen::drawWidgets();
}

void MainView::getNextFrame() {
	/*gl::draw(mFbo->getColorTexture());
	executeQueue();*/
	//mSurface = drawFbo->getColorTexture();
	if (drawQueue.empty()) {
		gl::draw(mFbo->getColorTexture());
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		gl::draw(drawFbo->getColorTexture());
		glDisable(GL_BLEND);
	}
	else {
		gl::draw(mFbo->getColorTexture());
		gl::ScopedFramebuffer fboScope(drawFbo);
		executeQueue();
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		gl::draw(drawFbo->getColorTexture());
		glDisable(GL_BLEND);
	}
}

void MainView::updateToolFromGesture() {
	auto gestures = leap->getCurrentGestures();

	for_each(gestures.begin(), gestures.end(), [=](Gesture gesture) {
		if (gesture.type() == Gesture::TYPE_CIRCLE) {
			toggleButtons.at(currentTool.activeTool).setPushed(false);
			currentTool.activeTool = tools((currentTool.activeTool + 1) % tools::LAST);
			toggleButtons.at(currentTool.activeTool).setPushed(true);
		}
	});
}

void MainView::executeQueue() {
	// Store the old state, draw, save new state
	if (oldState.empty()) {
		//oldState.push(gl::Context::create(gl::Context::getCurrent()));
	}
	for (std::list<std::function<void()>>::iterator it = drawQueue.begin(); it != drawQueue.end(); ++it) {
		std::function<void()> f = drawQueue.front();
		f();
		//mSurface = drawFbo->getColorTexture();
		// After every action, push the FBO to the oldState stack
		//oldState.push(gl::Context::create(gl::Context::getCurrent()));
	}
	//gl::draw(mFbo->getColorTexture());
	//drawFbo->blitFromScreen(Area(getWindow()->getBounds()), Area(getWindow()->getBounds()));

	//gl::clear();

	drawQueue.clear();
}

void MainView::resize(ivec2 size) {
	// TODO: blit from (screen to new screen)? using a filter to rescale everything
	//mFbo = ci::gl::Fbo::create(size.x, size.y);
	vec2 scaleFactor = vec2(oldWindowSize.x / size.x, oldWindowSize.y / size.y);
	//ci::gl::scale(oldWindowSize / size);
	setSize(size);
	oldWindowSize = size;
}

// Makes a square where the corners are at the intersections of the tangent
// lines of a circle at 0, 90, 180, 270 degrees
void MainView::drawSquareFromCenter(vec2 center, float radius) {
	//gl::setMatricesWindow(mFbo->getSize(), false);
	gl::ScopedColor sc;

	gl::color(color);

	vec2 TL, BR;
	TL.x = center.x + radius;
	TL.y = center.y + radius;
	BR.x = center.x - radius;
	BR.y = center.y - radius;

	ci::Area rect = ci::Area(TL, BR);
	gl::drawSolidRect(rect);
}

// Draws a circle with radius of radius
void MainView::drawCircleFromCenter(vec2 center, float radius) {
	//gl::setMatricesWindow(drawFbo->getSize(), false);
	gl::ScopedColor sc;

	gl::color(color);

	gl::drawSolidCircle(center, radius);
}

void MainView::drawCenteredRectangle(vec2 screenSize, vec2 size) {
	mFbo->bindFramebuffer();
	//gl::setMatricesWindow(mFbo->getSize(), false);
	gl::ScopedColor sc;
	vec2 TL, BR;
	TL.x = screenSize.x / 2 - size.x / 2;
	TL.y = screenSize.y / 2 + size.y / 2;
	BR.x = screenSize.x / 2 + size.x / 2;
	BR.y = screenSize.y / 2 - size.y / 2;
	gl::color(Color(1, 1, 1));
	ci::Area centeredRectangle = ci::Area(TL, BR);
	gl::drawSolidRect(centeredRectangle);
	mFbo->unbindFramebuffer();
}

void MainView::drawCircleAtPos(vec2 pos) {
	mFbo->bindFramebuffer();
	//gl::setMatricesWindow(mFbo->getSize(), false);
	gl::ScopedColor sc;

	gl::color(color.r, color.g, color.b, color.a);
	gl::drawSolidCircle(pos, 10);

	mFbo->unbindFramebuffer();
}

void MainView::drawLine(vec2 a, vec2 b) {
	gl::ScopedColor sc;

	gl::color(color.r, color.g, color.b, color.a);
	gl::drawLine(a, b);
}

void MainView::toggleTool(tools t) {
	tools originalTool = currentTool.activeTool;

	currentTool.initialMousePos = vec2(0, 0);
	currentTool.finalMousePos = vec2(0, 0);
	currentTool.active = false;

	if (currentTool.activeTool == t) {
		currentTool.activeTool = none;
	}
	else {
		currentTool.activeTool = t;	}
}
