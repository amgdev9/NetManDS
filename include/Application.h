/**
 * @file Application.h
 * @brief 
 */

#ifndef SOURCE_APPLICATION_H_
#define SOURCE_APPLICATION_H_

// Includes 3DS
#include <3ds.h>
#include <citro2d.h>

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
	public:
		static Application *getInstance();
		void initialize();
		void run();
		void fatalError(const char *text, u32 errorCode);
};

}

#endif
