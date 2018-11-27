#include "WorldLoaderApp.h"
#include <random>
#include <ctime>

int main() {
	
	// allocation
	auto app = new WorldLoaderApp();

	srand((unsigned int)time(NULL));

	// initialise and loop
	app->run("AIE", 1280, 720, false);

	// deallocation
	delete app;

	return 0;
}