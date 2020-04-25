/**
 * @file Application.h
 * @brief Application singleton
 */

#ifndef _APPLICATION_H_
#define _APPLICATION_H_

// Includes C/C++
#include <memory>
#include <unordered_map>

// Includes 3DS
#include <3ds.h>
#include <citro2d.h>

// Own includes
#include "gui/GuiLayout.h"
#include "socket/UdpSocket.h"
#include "socket/TcpSocket.h"
#include "asn1/BerOid.h"

// Defines
#define SOC_ALIGN               0x1000
#define SOC_BUFFERSIZE          0x100000		// 1MB
#define HTTP_BUFFERSIZE         0x100000        // 1MB
#define FADE_SPEED              4
#define SCREEN_WIDTH_TOP        400
#define SCREEN_HEIGHT_TOP       240
#define SCREEN_WIDTH_BOTTOM     320
#define SCREEN_HEIGHT_BOTTOM    240

namespace NetMan {

/**
 * @enum LoadingState
 */
enum LoadingState {
    LOADINGSTATE_FADEOUT = 0,
    LOADINGSTATE_LOADING,
    LOADINGSTATE_FADEIN,
    LOADINGSTATE_NONE,
};

/**
 * @struct PduData
 */
typedef struct {
    std::shared_ptr<BerOid> oid;
    std::string oidText;
    char type;
    std::string value;
} PduField;

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
        std::shared_ptr<GuiLayout> topLayout;
        std::shared_ptr<GuiLayout> bottomLayout;
        LoadingState loadingState;
        std::string loadingLayoutName;
        s16 fadeAlpha;
        std::unordered_map<std::string, C2D_SpriteSheet> resourceManager;
        std::shared_ptr<void> contextData;
        std::shared_ptr<UdpSocket> trapv1Sock;
        std::shared_ptr<UdpSocket> trapv2Sock;
        std::shared_ptr<UdpSocket> trapv3Sock;
        std::shared_ptr<UdpSocket> syslogUdpSock;
        std::shared_ptr<TcpSocket> syslogTcpSock;
        std::vector<PduField> pduFields;
		Application();
		virtual ~Application();
        void processLoadingScreen();
        void unloadResources();
	public:
		static Application &getInstance();
		void initialize(const std::string &layoutPath);
		void run();
		void fatalError(const std::string &text, u32 errorCode);
        void requestLayoutChange(const std::string &layoutName, std::shared_ptr<void> contextData = nullptr);
        C2D_SpriteSheet getImageResource(const char *resourceName);
        void messageBox(const std::string &text);
        inline std::shared_ptr<void> getContextData() { return this->contextData; }
        inline u32 getDown() { return down; }
        inline touchPosition &getTouch() { return touch; }
        inline std::shared_ptr<UdpSocket> getTrapv1Sock() { return trapv1Sock; }
        inline std::shared_ptr<UdpSocket> getTrapv2Sock() { return trapv2Sock; }
        inline std::shared_ptr<UdpSocket> getTrapv3Sock() { return trapv3Sock; }
        inline std::shared_ptr<UdpSocket> getSyslogUdpSock() { return syslogUdpSock; }
        inline std::shared_ptr<TcpSocket> getSyslogTcpSock() { return syslogTcpSock; }
        inline std::vector<PduField> &getPduFields() { return pduFields; }
};

}

#endif
