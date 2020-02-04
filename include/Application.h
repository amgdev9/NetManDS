/**
 * @file Application.h
 * @brief Application singleton
 */

#ifndef SOURCE_APPLICATION_H_
#define SOURCE_APPLICATION_H_

// Includes C/C++
#include <memory>

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
		u32 held;										/**< Keys held */
		u32 down;										/**< Keys down */
		u32 up;											/**< Keys up */
		touchPosition touch;							/**< Touchpad state */
		bool init;										/**< Initialized? */
		C3D_RenderTarget *screen[2];					/**< Render targets */
		std::unique_ptr<u32> socket_buffer;				/**< Socket buffer */

		Application();
		virtual ~Application();
	public:
		static Application &getInstance();
		void initialize();
		void run();
		void fatalError(const std::string &text, u32 errorCode);
};

}

#endif
