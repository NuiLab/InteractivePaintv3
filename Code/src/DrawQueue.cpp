#include "DrawQueue.h"

void DrawQueue::add(vec2 point) {
	drawVecs.push_back(point);
}

void DrawQueue::execute() {
	for (int i = 0; i < threads.size(); i++) {
		threads.at(i).join();
	}
	threads.clear();
	std::vector<vec2> temp;
	for (int i = 0; i < drawVecs.size(); i++) {
		gl::drawSolidCircle(drawVecs.back(), 8);
		//temp.push_back(drawVecs.back());
		drawVecs.pop_back();
	}
	/*for (int i = 0; i < drawVecs.size(); i++) {
		drawVecs.push_back(temp.back());
		temp.pop_back();
	}*/

}
