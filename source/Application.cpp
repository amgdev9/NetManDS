/**
 * @file Application.cpp
 * @brief 
 */

// Includes C/C++
#include <stdio.h>
#include <stdlib.h>

// Includes 3DS
#include <3ds.h>

// Own includes
#include "Application.h"

namespace NetMan {

// Static attributes
Application *Application::instance = NULL;

Application::Application() {

	this->init = false;

	// Register singleton destructor
	atexit(Application::destroy);
}

void Application::initialize() {

	if(init) return;

	// Initialize graphics
	gfxInitDefault();

	// Initialize RomFS
	Result rc = romfsInit();
	if(rc) {
		this->fatalError("\n Could not initialize RomFS\n\n Error code 0x%lX\n\n Press any key to exit", (u32)rc);
	}

	// Initialize renderer
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();
	this->screen[0] = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	this->screen[1] = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

	init = true;
}

void Application::run() {

	if(!init) return;

	// Application main loop
	while (aptMainLoop()) {

		// Scan input
		hidScanInput();
		this->held = hidKeysHeld();
		this->down = hidKeysDown();
		this->up = hidKeysUp();
		hidTouchRead(&this->touch);

		// Start new frame
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

		// Draw top screen scene
		C3D_RenderTargetClear(this->screen[0], C3D_CLEAR_ALL, C2D_Color32f(1.0f, 0.0f, 0.0f, 1.0f), 0);
		C3D_FrameDrawOn(this->screen[0]);
		C2D_SceneTarget(this->screen[0]);
			// TODO Draw top 3D
		C2D_Prepare();
			// TODO Draw top 3D
		C2D_Flush();

		// Draw bottom screen scene
		C3D_RenderTargetClear(this->screen[1], C3D_CLEAR_ALL, C2D_Color32f(0.0f, 0.0f, 1.0f, 1.0f), 0);
		C3D_FrameDrawOn(this->screen[1]);
		C2D_SceneTarget(this->screen[1]);
			// TODO Draw bottom 3D
		C2D_Prepare();
			// TODO Draw bottom 3D
		C2D_Flush();

		// Swap buffers
		C3D_FrameEnd(0);
	}
}

Application::~Application() {

	if(!init) return;

	C3D_RenderTargetDelete(this->screen[0]);
	C3D_RenderTargetDelete(this->screen[1]);

	C2D_Fini();
	C3D_Fini();

	// Terminate graphics
	gfxExit();
}

void Application::fatalError(const char *text, u32 errorCode) {

	// Print error
	consoleInit(GFX_TOP, NULL);
	printf(text, errorCode);

	// Idle wait loop
	while(aptMainLoop()) {

		hidScanInput();

		if(hidKeysDown()) {
			exit(0);
		}

		gspWaitForVBlank();
		gfxSwapBuffers();
	}
}

Application *Application::getInstance() {

	if(instance == NULL) {
		instance = new Application();
	}

	return instance;
}

void Application::destroy() {
	delete instance;
}

}
