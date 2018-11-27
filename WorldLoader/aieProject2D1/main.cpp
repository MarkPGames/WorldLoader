#include "aieProject2D1App.h"

int main() {
	
	// allocation
	auto app = new aieProject2D1App();

	// initialise and loop
	app->run("AIE", 1280, 720, false);

	// deallocation
	delete app;

	return 0;
}