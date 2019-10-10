/**
 * @file Application.h
 * @brief 
 */

#ifndef SOURCE_APPLICATION_H_
#define SOURCE_APPLICATION_H_

// Includes 3DS
#include <3ds.h>
#include <citro2d.h>

// Defines
#define SOC_ALIGN       0x1000
#define SOC_BUFFERSIZE  0x100000		// 1MB

namespace NetMan {

/**
 * @class Application
 */
class Application {
	private:
		static Application *instance;
		static void destroy();
		Application();
		virtual ~Application();
		u32 held;
		u32 down;
		u32 up;
		touchPosition touch;
		bool init;
		C3D_RenderTarget *screen[2];
		u32 *socket_buffer;
	public:
		static Application *getInstance();
		void initialize();
		void run();
		void fatalError(const char *text, u32 errorCode);
};

}

#endif
