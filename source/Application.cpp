/**
 * @file Application.cpp
 * @brief Application singleton
 */

// Includes C/C++
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

// Includes 3DS
#include <3ds.h>

// Own includes
#include "Application.h"

namespace NetMan {

/**
 * @brief Constructor for an Application
 */
Application::Application() {

	this->init = false;
}

/**
 * @brief Inicialize the Application
 */
void Application::initialize() {

	if(init) return;

	// Initialize graphics
	gfxInitDefault();

	// Initialize RomFS
	Result rc = romfsInit();
	if(rc) {
		this->fatalError("Could not initialize RomFS", (u32)rc);
	}

	// Allocate memory for socket service
	this->socket_buffer = std::unique_ptr<u32>((u32*) memalign (SOC_ALIGN, SOC_BUFFERSIZE));
	if(this->socket_buffer == nullptr) {
		this->fatalError("Could not create socket buffer", 1);
	}

	// Initialize socket service
	rc = socInit(socket_buffer.get(), SOC_BUFFERSIZE);
	if(rc) {
		this->fatalError("Could not initialize sockets", (u32)rc);
	}

	// Initialize renderer
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();
	this->screen[0] = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	this->screen[1] = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
	if(this->screen[0] == NULL || this->screen[1] == NULL) {
		this->fatalError("Could not create render targets", 1);
	}

	// Inicialization done
	init = true;
}

/**
 * @brief Run the Application
 */
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
			// TODO Draw top 2D
		C2D_Flush();

		// Draw bottom screen scene
		C3D_RenderTargetClear(this->screen[1], C3D_CLEAR_ALL, C2D_Color32f(0.0f, 0.0f, 1.0f, 1.0f), 0);
		C3D_FrameDrawOn(this->screen[1]);
		C2D_SceneTarget(this->screen[1]);
		C2D_Prepare();
			// TODO Draw bottom 2D
		C2D_Flush();

		// Swap buffers
		C3D_FrameEnd(0);
	}
}

/**
 * @brief Destructor for an Application
 */
Application::~Application() {

	if(!init) return;

	// Terminate sockets
	socExit();

	// Delete render targets
	C3D_RenderTargetDelete(this->screen[0]);
	C3D_RenderTargetDelete(this->screen[1]);

	// Terminate graphics
	C2D_Fini();
	C3D_Fini();
	gfxExit();
}

/**
 * @brief Show a fatal error
 * @param text Error string to show
 * @param errorCode Error code
 */
void Application::fatalError(const std::string &text, u32 errorCode) {

	// Print error
	consoleInit(GFX_TOP, NULL);
	printf("\n %s\n\n Error code 0x%lX\n\n Press any key to exit", text.c_str(), errorCode);

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

/**
 * @brief Get the singleton instance for an Application
 * @return The singleton instance
 */
Application &Application::getInstance() {

	static Application app;
	return app;
}

}
