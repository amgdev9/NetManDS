#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>

#include "Application.h"

using namespace NetMan;

int main(int argc, char **argv) {

	Application *app = Application::getInstance();

	app->initialize();

	app->run();

	return 0;
}
