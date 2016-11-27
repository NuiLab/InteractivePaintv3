#include "cinder/app/App.h"

#include "Util.h"

using namespace std;
using namespace ci;

// ensures that a directory exists at a path relative to the current working directory
void Util::ensureDirectory(string path) {
	fs::path p(fs::current_path());
	p /= path;

	fs::path temp;
	for (auto& part : p) {
		temp /= part;
		if (!fs::exists(fs::path(temp))) {
			if (!fs::create_directory(temp)) {
				// TODO: report an error
			}
		}
		if (!fs::is_directory(temp)) {
			if (!fs::create_directory(temp)) {
				// TODO: report an error
			}
		}
	}
}
