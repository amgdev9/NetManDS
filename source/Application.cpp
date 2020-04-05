/**
 * @file Application.cpp
 * @brief Application singleton
 */

// Includes C/C++
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>

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
    this->resourceManager = std::unordered_map<std::string, C2D_SpriteSheet>();
    this->contextData = nullptr;
}

/**
 * @brief Inicialize the Application
 * @param topLayoutPath     Initial layout path
 */
void Application::initialize(const std::string &layoutPath) {

	if(init) return;

	// Initialize random seed
	srand(time(NULL));
	
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

    // Initializa http service
    rc = httpcInit(HTTP_BUFFERSIZE);
    if(rc) {
		this->fatalError("Could not initialzie http service", (u32)rc);
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

    // Load initial layouts
    topLayout = nullptr;
    bottomLayout = nullptr;
    requestLayoutChange(layoutPath);

    // Initialize audio
    rc = ndspInit();
    if(rc) {
        this->fatalError("ndspInit", (u32)rc);
    }
    ndspSetOutputMode(NDSP_OUTPUT_STEREO);
	ndspSetOutputCount(1);

    // Load beep sound (used for trap and log receiving)
    try {
        beepAudio = std::unique_ptr<WaveAudio>(new WaveAudio("romfs:/snd/beep.wav"));
    } catch (const std::bad_alloc &e) {
        this->fatalError(e.what(), 0);
    } catch (const std::runtime_error &e) {
        this->fatalError(e.what(), 0);
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

        // Process the loading screen
        processLoadingScreen();

		// Scan input
		hidScanInput();
		this->held = hidKeysHeld();
		this->down = hidKeysDown();
		this->up = hidKeysUp();
		hidTouchRead(&this->touch);

        // Process input on bottom screen
        if(bottomLayout != nullptr && loadingState == LOADINGSTATE_NONE) {
            bottomLayout->input(held, down, up, touch);
        }

		// Start new frame
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

		// Draw top screen scene
		C3D_RenderTargetClear(this->screen[0], C3D_CLEAR_ALL, C2D_Color32f(1.0f, 0.4f, 0.4f, 0.4f), 0);
		C3D_FrameDrawOn(this->screen[0]);
		C2D_SceneTarget(this->screen[0]);
		C2D_Prepare();
			if(topLayout != nullptr) topLayout->draw();
            C2D_DrawRectSolid(0, 0, 0, SCREEN_WIDTH_TOP, SCREEN_HEIGHT_TOP, C2D_Color32(0, 0, 0, fadeAlpha));
		C2D_Flush();

		// Draw bottom screen scene
		C3D_RenderTargetClear(this->screen[1], C3D_CLEAR_ALL, C2D_Color32f(1.0f, 0.2f, 0.2f, 0.2f), 0);
		C3D_FrameDrawOn(this->screen[1]);
		C2D_SceneTarget(this->screen[1]);
		C2D_Prepare();
			if(bottomLayout != nullptr) bottomLayout->draw();
            C2D_DrawRectSolid(0, 0, 0, SCREEN_WIDTH_BOTTOM, SCREEN_HEIGHT_BOTTOM, C2D_Color32(0, 0, 0, fadeAlpha));
		C2D_Flush();

		// Swap buffers
		C3D_FrameEnd(0);
	}
}

/**
 * @brief Process the loading screen
 */
void Application::processLoadingScreen() {

    switch(loadingState) {
        case LOADINGSTATE_FADEOUT:
            fadeAlpha += FADE_SPEED;
            if(fadeAlpha >= 0xFF) {
                fadeAlpha = 0xFF;
                loadingState = LOADINGSTATE_LOADING;
            }
            break;
        case LOADINGSTATE_LOADING:

            // Unload resources
            unloadResources();

            // Load new layout
            try {
                topLayout = std::make_shared<GuiLayout>(loadingLayoutName + "_top");
                bottomLayout = std::make_shared<GuiLayout>(loadingLayoutName + "_bottom");
            } catch (const std::bad_alloc &e) {
                this->fatalError(e.what(), 1);
            } catch (const std::runtime_error &e) {
                this->fatalError(e.what(), 1);
            }
            loadingState = LOADINGSTATE_FADEIN;
            break;
        case LOADINGSTATE_FADEIN:
            fadeAlpha -= FADE_SPEED;
            if(fadeAlpha <= 0) {
                fadeAlpha = 0;
                loadingState = LOADINGSTATE_NONE;
            }
            break;
        default: break;
    }
}

/**
 * @brief Unload image resources
 */
void Application::unloadResources() {
    for(auto resource : resourceManager) {
        C2D_SpriteSheetFree(resource.second);
    }
    resourceManager.clear();
}

/**
 * @brief Request a layout change
 * @param layoutName    Name of the layout
 */
void Application::requestLayoutChange(const std::string &layoutName) {

    if(topLayout == nullptr || bottomLayout == nullptr) {
        loadingState = LOADINGSTATE_LOADING;
        fadeAlpha = 0xFF;
    } else {
        loadingState = LOADINGSTATE_FADEOUT;
        fadeAlpha = 0;
    }

    loadingLayoutName = layoutName;
}

/**
 * @brief Destructor for an Application
 */
Application::~Application() {

	if(!init) return;

    // Unload resources
    unloadResources();

	// Terminate sockets
	socExit();
    httpcExit();

    // Terminate audio
    ndspChnWaveBufClear(BEEP_AUDIO_CHANNEL);
    ndspExit();

	// Delete render targets
	C3D_RenderTargetDelete(this->screen[0]);
	C3D_RenderTargetDelete(this->screen[1]);

	// Terminate graphics
	C2D_Fini();
	C3D_Fini();
	gfxExit();
}

/**
 * @brief Get an image resource
 * @param resourceName  Name of the image resource (without extension, lookup folder is romfs:/gfx/)
 * @return The loaded image resource
 */
C2D_SpriteSheet Application::getImageResource(const char *resourceName) {
    auto it = resourceManager.find(resourceName);
    if(it == resourceManager.end()) {
        std::string path = std::string("romfs:/gfx/") + resourceName + ".t3x";
        C2D_SpriteSheet imageData = C2D_SpriteSheetLoad(path.c_str());
        if(imageData == NULL) {
            throw std::runtime_error(std::string("Can't load: ") + resourceName);
        }
        resourceManager[resourceName] = imageData;
        return imageData;
    } else {
        return it->second;
    }
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
